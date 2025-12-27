#!/usr/bin/env python3
"""
TPU Coprocessor Interface Library
=================================
Modular interface for streaming inference via UART to FPGA TPU.

Architecture:
    Host (Python) <--UART--> FPGA TPU Coprocessor

The FPGA implements a 3x3 systolic array with:
- 256-bit wide unified buffer (32 bytes per entry)
- Weight memory for systolic array weights
- 20-instruction ISA for ML operations

Design Principles:
- Modular: Each component (UART, Memory, ISA) is separate
- Synchronous: All operations wait for acknowledgment
- Streaming: Supports continuous data flow for inference
"""

import serial
import struct
import time
import numpy as np
from dataclasses import dataclass
from typing import Optional, List, Tuple
from enum import IntEnum


# =============================================================================
# ISA DEFINITIONS (matches rtl/tpu_controller.sv)
# =============================================================================

class Opcode(IntEnum):
    """TPU Instruction Set Architecture - 6-bit opcodes (matches tpu_controller.sv)"""
    NOP         = 0x00  # No operation
    RD_HOST_MEM = 0x01  # Read from host memory
    WR_HOST_MEM = 0x02  # Write to host memory
    RD_WEIGHT   = 0x03  # Read weights from weight memory
    LD_UB       = 0x04  # Load from Unified Buffer to systolic array
    ST_UB       = 0x05  # Store from accumulators to Unified Buffer
    MATMUL      = 0x10  # Matrix multiply
    CONV2D      = 0x11  # 2D convolution
    MATMUL_ACC  = 0x12  # Matrix multiply with accumulate
    RELU        = 0x18  # ReLU activation function
    RELU6       = 0x19  # ReLU6 activation (clamp to 6)
    SIGMOID     = 0x1A  # Sigmoid approximation
    TANH        = 0x1B  # Tanh approximation
    MAX_POOL    = 0x20  # Max pooling
    AVG_POOL    = 0x21  # Average pooling
    ADD_BIAS    = 0x22  # Add bias
    BATCH_NORM  = 0x23  # Batch normalization
    SYNC        = 0x30  # Synchronization barrier
    CFG_REG     = 0x31  # Configure register
    HALT        = 0x3F  # Halt execution


class UARTCommand(IntEnum):
    """UART DMA Commands (matches rtl/uart_dma_basys3.sv)"""
    WRITE_UB    = 0x01  # Write to Unified Buffer
    WRITE_WT    = 0x02  # Write to Weight Memory
    WRITE_INSTR = 0x03  # Write instruction to program memory
    READ_UB     = 0x04  # Read from Unified Buffer
    EXECUTE     = 0x05  # Start TPU execution
    READ_STATUS = 0x06  # Read status register
    READ_DEBUG  = 0x14  # Read debug counters


@dataclass
class TPUStatus:
    """TPU Status Register Decoded"""
    sys_busy: bool
    sys_done: bool
    vpu_busy: bool
    vpu_done: bool
    ub_busy: bool
    ub_done: bool
    raw: int

    @classmethod
    def from_byte(cls, status: int) -> 'TPUStatus':
        return cls(
            sys_busy = bool(status & 0x01),
            sys_done = bool(status & 0x02),
            vpu_busy = bool(status & 0x04),
            vpu_done = bool(status & 0x08),
            ub_busy  = bool(status & 0x10),
            ub_done  = bool(status & 0x20),
            raw      = status
        )

    def is_idle(self) -> bool:
        return not self.sys_busy and not self.vpu_busy and not self.ub_busy


# =============================================================================
# INSTRUCTION ENCODER
# =============================================================================

class InstructionEncoder:
    """
    Encodes TPU instructions into 32-bit format.

    Format: [31:26] Opcode | [25:18] Arg1 | [17:10] Arg2 | [9:2] Arg3 | [1:0] Flags
    """

    @staticmethod
    def encode(opcode: Opcode, arg1: int = 0, arg2: int = 0,
               arg3: int = 0, flags: int = 0) -> int:
        """Encode instruction fields into 32-bit word"""
        assert 0 <= opcode <= 0x3F, f"Opcode out of range: {opcode}"
        assert 0 <= arg1 <= 0xFF, f"Arg1 out of range: {arg1}"
        assert 0 <= arg2 <= 0xFF, f"Arg2 out of range: {arg2}"
        assert 0 <= arg3 <= 0xFF, f"Arg3 out of range: {arg3}"
        assert 0 <= flags <= 0x03, f"Flags out of range: {flags}"

        return ((opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags)

    @staticmethod
    def decode(instr: int) -> Tuple[int, int, int, int, int]:
        """Decode 32-bit instruction into fields"""
        opcode = (instr >> 26) & 0x3F
        arg1   = (instr >> 18) & 0xFF
        arg2   = (instr >> 10) & 0xFF
        arg3   = (instr >> 2)  & 0xFF
        flags  = instr & 0x03
        return (opcode, arg1, arg2, arg3, flags)

    # Convenience methods for common instructions
    @classmethod
    def nop(cls) -> int:
        return cls.encode(Opcode.NOP)

    @classmethod
    def halt(cls) -> int:
        return cls.encode(Opcode.HALT)

    @classmethod
    def load_weights(cls, start_tile: int, num_tiles: int = 1, buf_sel: int = 0) -> int:
        """RD_WEIGHT: Load weights from weight memory into systolic array
           arg1=start_tile, arg2=num_tiles, flags=buffer_select"""
        return cls.encode(Opcode.RD_WEIGHT, start_tile, num_tiles, 0, buf_sel)

    @classmethod
    def load_ub(cls, ub_addr: int, num_blocks: int = 1) -> int:
        """LD_UB: Load activations from unified buffer into systolic array
           arg1=ub_addr, arg2=num_blocks"""
        return cls.encode(Opcode.LD_UB, ub_addr, num_blocks, 0)

    @classmethod
    def store_ub(cls, ub_addr: int, num_blocks: int = 1) -> int:
        """ST_UB: Store accumulator results to unified buffer
           arg1=ub_addr, arg2=num_blocks"""
        return cls.encode(Opcode.ST_UB, ub_addr, num_blocks, 0)

    @classmethod
    def matmul(cls, ub_input_addr: int, acc_output_addr: int, num_rows: int = 3,
               transpose: bool = False, signed: bool = True) -> int:
        """MATMUL: Execute matrix multiply on systolic array
           arg1=ub_input_addr, arg2=acc_output_addr, arg3=num_rows
           flags: bit0=transpose, bit1=signed"""
        flags = (1 if transpose else 0) | (2 if signed else 0)
        return cls.encode(Opcode.MATMUL, ub_input_addr, acc_output_addr, num_rows, flags)

    @classmethod
    def matmul_acc(cls, ub_input_addr: int, acc_addr: int, num_rows: int = 3) -> int:
        """MATMUL_ACC: Accumulate matrix multiply (add to existing accumulator)"""
        return cls.encode(Opcode.MATMUL_ACC, ub_input_addr, acc_addr, num_rows)

    @classmethod
    def relu(cls, acc_input_addr: int, ub_output_addr: int, num_elements: int = 9) -> int:
        """RELU: Apply ReLU activation
           arg1=acc_input_addr, arg2=ub_output_addr, arg3=num_elements"""
        return cls.encode(Opcode.RELU, acc_input_addr, ub_output_addr, num_elements)

    @classmethod
    def add_bias(cls, acc_input_addr: int, ub_bias_addr: int, num_elements: int = 3) -> int:
        """ADD_BIAS: Add bias vector to accumulator values"""
        return cls.encode(Opcode.ADD_BIAS, acc_input_addr, ub_bias_addr, num_elements)

    @classmethod
    def sync(cls, sync_mask: int = 0x0F, timeout_hi: int = 0, timeout_lo: int = 0x10) -> int:
        """SYNC: Synchronization barrier
           arg1=sync_mask, arg2=timeout_high, arg3=timeout_low"""
        return cls.encode(Opcode.SYNC, sync_mask, timeout_hi, timeout_lo)


# =============================================================================
# UART TRANSPORT LAYER
# =============================================================================

class UARTTransport:
    """
    Low-level UART communication with FPGA.

    Handles byte-level protocol with proper synchronization.
    """

    ACK_BYTE = 0xAA
    NACK_BYTE = 0xFF

    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 2.0):
        self.ser = serial.Serial(port, baudrate, timeout=timeout)
        time.sleep(0.1)  # Allow UART to stabilize
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

    def close(self):
        self.ser.close()

    def send_command(self, cmd: UARTCommand, addr_hi: int = 0, addr_lo: int = 0,
                     len_hi: int = 0, len_lo: int = 0) -> None:
        """Send 5-byte command header"""
        header = bytes([cmd, addr_hi, addr_lo, len_hi, len_lo])
        self.ser.write(header)

    def send_data(self, data: bytes) -> None:
        """Send raw data bytes"""
        self.ser.write(data)

    def wait_ack(self, timeout: float = 1.0) -> bool:
        """Wait for ACK byte from FPGA"""
        self.ser.timeout = timeout
        ack = self.ser.read(1)
        return ack == bytes([self.ACK_BYTE])

    def read_bytes(self, count: int) -> bytes:
        """Read specified number of bytes"""
        return self.ser.read(count)

    def flush(self) -> None:
        """Flush buffers"""
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()


# =============================================================================
# TPU COPROCESSOR HIGH-LEVEL INTERFACE
# =============================================================================

class TPUCoprocessor:
    """
    High-level interface to TPU FPGA coprocessor.

    Provides clean API for:
    - Loading weights and activations
    - Programming instruction sequences
    - Executing inference
    - Reading results
    """

    SYSTOLIC_SIZE = 3  # 3x3 systolic array
    UB_WORD_SIZE = 32  # 256-bit = 32 bytes per UB entry

    def __init__(self, port: str, baudrate: int = 115200):
        self.uart = UARTTransport(port, baudrate)
        self.encoder = InstructionEncoder()
        print(f"TPU Coprocessor connected on {port}")

    def close(self):
        self.uart.close()

    # -------------------------------------------------------------------------
    # Status & Debug
    # -------------------------------------------------------------------------

    def read_status(self) -> TPUStatus:
        """Read TPU status register"""
        self.uart.send_command(UARTCommand.READ_STATUS)
        time.sleep(0.02)
        status_byte = self.uart.read_bytes(1)
        if status_byte:
            return TPUStatus.from_byte(status_byte[0])
        return TPUStatus.from_byte(0)

    def wait_idle(self, timeout: float = 5.0) -> bool:
        """Wait for TPU to become idle"""
        start = time.time()
        while time.time() - start < timeout:
            status = self.read_status()
            if status.is_idle():
                return True
            time.sleep(0.01)
        return False

    # -------------------------------------------------------------------------
    # Memory Operations
    # -------------------------------------------------------------------------

    def write_unified_buffer(self, addr: int, data: bytes) -> bool:
        """
        Write data to Unified Buffer.

        Args:
            addr: UB address (0-127 per bank)
            data: Data bytes (padded to 32 bytes if needed)
        """
        # Pad to 32 bytes
        if len(data) < self.UB_WORD_SIZE:
            data = data + bytes(self.UB_WORD_SIZE - len(data))
        elif len(data) > self.UB_WORD_SIZE:
            data = data[:self.UB_WORD_SIZE]

        self.uart.send_command(UARTCommand.WRITE_UB, 0, addr, 0, len(data))
        self.uart.send_data(data)
        time.sleep(0.05)
        return self.uart.wait_ack()

    def read_unified_buffer(self, addr: int, length: int = 32) -> bytes:
        """
        Read data from Unified Buffer.

        Args:
            addr: UB address
            length: Number of bytes to read (default 32)
        """
        self.uart.send_command(UARTCommand.READ_UB, 0, addr, 0, length)
        time.sleep(0.05)
        return self.uart.read_bytes(length)

    def write_weights(self, addr: int, data: bytes) -> bool:
        """
        Write weights to Weight Memory.

        Args:
            addr: Weight memory address
            data: Weight data (8 bytes for 3x3 array row)
        """
        # Weight memory expects 8 bytes (for 3x3 systolic array)
        if len(data) < 8:
            data = data + bytes(8 - len(data))

        self.uart.send_command(UARTCommand.WRITE_WT, 0, addr, 0, len(data))
        self.uart.send_data(data)
        time.sleep(0.02)
        return self.uart.wait_ack()

    # -------------------------------------------------------------------------
    # Instruction Programming
    # -------------------------------------------------------------------------

    def write_instruction(self, addr: int, instr: int) -> None:
        """Write single instruction to program memory"""
        cmd = bytes([UARTCommand.WRITE_INSTR, 0, addr])
        self.uart.ser.write(cmd)
        self.uart.ser.write(struct.pack('>I', instr))
        time.sleep(0.01)

    def load_program(self, instructions: List[int]) -> None:
        """Load a program (list of instructions) to instruction memory"""
        for addr, instr in enumerate(instructions):
            self.write_instruction(addr, instr)

    # -------------------------------------------------------------------------
    # Execution
    # -------------------------------------------------------------------------

    def execute(self) -> bool:
        """Start TPU execution"""
        self.uart.ser.write(bytes([UARTCommand.EXECUTE]))
        time.sleep(0.1)
        return self.wait_idle(timeout=2.0)

    # -------------------------------------------------------------------------
    # High-Level Inference API
    # -------------------------------------------------------------------------

    def matrix_multiply(self, weights: np.ndarray, inputs: np.ndarray) -> np.ndarray:
        """
        Perform matrix multiplication: output = weights @ inputs

        This is the primary inference operation for neural network layers.

        Args:
            weights: Weight matrix (3x3 for systolic array)
            inputs: Input vector/matrix

        Returns:
            Result matrix after multiplication
        """
        # Validate dimensions
        assert weights.shape == (3, 3), f"Weights must be 3x3, got {weights.shape}"

        # Convert to int8 for hardware
        w_int8 = np.clip(weights, -128, 127).astype(np.int8)
        i_int8 = np.clip(inputs, -128, 127).astype(np.int8)

        # Load weights to weight memory (row by row)
        for row in range(3):
            row_data = bytes(w_int8[row, :].tobytes().ljust(8, b'\x00'))
            self.write_weights(row, row_data)

        # Load inputs to unified buffer
        input_bytes = i_int8.flatten().tobytes()
        self.write_unified_buffer(0, input_bytes)

        # Create and load the compute program
        # ISA: RD_WEIGHT(tile, count), LD_UB(addr, blocks), MATMUL(ub_in, acc_out, rows), ST_UB(addr, blocks)
        program = [
            self.encoder.load_weights(0, 1),        # Load weight tile 0
            self.encoder.load_ub(0, 1),             # Load input from UB addr 0
            self.encoder.matmul(0, 0, 3),           # MATMUL: ub[0] -> acc[0], 3 rows
            self.encoder.store_ub(1, 1),            # Store acc[0] result to UB addr 1
            self.encoder.halt()                      # Done
        ]
        self.load_program(program)

        # Execute
        if not self.execute():
            print("Warning: Execution may not have completed")

        # Read results
        result_bytes = self.read_unified_buffer(1, 32)

        # Convert back to numpy (assuming int32 accumulator output)
        result = np.frombuffer(result_bytes[:12], dtype=np.int8).reshape(3, 1)

        return result


# =============================================================================
# DEMO APPLICATION
# =============================================================================

def demo_basic_communication(tpu: TPUCoprocessor):
    """Test basic UART communication"""
    print("\n" + "="*60)
    print("DEMO 1: Basic Communication Test")
    print("="*60)

    # Check status
    status = tpu.read_status()
    print(f"  TPU Status: {status}")
    print(f"  Is Idle: {status.is_idle()}")

    # Write and read back test pattern
    test_data = bytes(range(32))
    print(f"\n  Writing test pattern to UB[0]: {test_data[:8].hex()}...")

    if tpu.write_unified_buffer(0, test_data):
        print("  Write: OK")
    else:
        print("  Write: FAILED")
        return False

    readback = tpu.read_unified_buffer(0, 32)
    print(f"  Readback from UB[0]: {readback[:8].hex()}...")

    if readback == test_data:
        print("  PASS: Data matches!")
        return True
    else:
        print("  FAIL: Data mismatch!")
        return False


def demo_instruction_execution(tpu: TPUCoprocessor):
    """Test instruction execution"""
    print("\n" + "="*60)
    print("DEMO 2: Instruction Execution Test")
    print("="*60)

    # Simple program: NOP -> NOP -> HALT
    program = [
        InstructionEncoder.nop(),
        InstructionEncoder.nop(),
        InstructionEncoder.halt()
    ]

    print(f"  Loading program: {len(program)} instructions")
    for i, instr in enumerate(program):
        opcode, a1, a2, a3, flags = InstructionEncoder.decode(instr)
        print(f"    [{i}] 0x{instr:08X} - Op:{opcode:02X} Args:{a1},{a2},{a3} Flags:{flags}")

    tpu.load_program(program)

    print("\n  Executing...")
    if tpu.execute():
        print("  Execution completed successfully!")
        return True
    else:
        print("  Execution timeout or error")
        return False


def demo_simple_inference(tpu: TPUCoprocessor):
    """Demo simple matrix multiply inference"""
    print("\n" + "="*60)
    print("DEMO 3: Simple Matrix Multiply Inference")
    print("="*60)

    # Identity-like weights (scaled for int8)
    weights = np.array([
        [1, 0, 0],
        [0, 1, 0],
        [0, 0, 1]
    ], dtype=np.int8)

    # Simple input
    inputs = np.array([10, 20, 30], dtype=np.int8).reshape(3, 1)

    print(f"\n  Weights (3x3):\n{weights}")
    print(f"\n  Inputs:\n{inputs.flatten()}")

    # Expected result (CPU reference)
    expected = weights @ inputs
    print(f"\n  Expected output (CPU): {expected.flatten()}")

    # Load weights
    print("\n  Loading weights to FPGA...")
    for row in range(3):
        row_data = bytes(weights[row, :].tobytes().ljust(8, b'\x00'))
        if not tpu.write_weights(row, row_data):
            print(f"    Weight row {row}: FAILED")
        else:
            print(f"    Weight row {row}: OK")

    # Load inputs to UB
    input_bytes = inputs.flatten().tobytes().ljust(32, b'\x00')
    print(f"\n  Loading inputs to UB[0]: {input_bytes[:8].hex()}")
    tpu.write_unified_buffer(0, input_bytes)

    # Create compute program
    enc = InstructionEncoder()
    program = [
        enc.load_weights(0, 1),         # RD_WEIGHT: Load weight tile 0
        enc.load_ub(0, 1),              # LD_UB: Load input from UB[0]
        enc.matmul(0, 0, 3),            # MATMUL: ub[0] -> acc[0], 3 rows
        enc.store_ub(1, 1),             # ST_UB: Store acc to UB[1]
        enc.halt()
    ]

    print(f"\n  Loading compute program ({len(program)} instructions):")
    for i, instr in enumerate(program):
        opcode, a1, a2, a3, flags = enc.decode(instr)
        try:
            op_name = Opcode(opcode).name
        except ValueError:
            op_name = f"0x{opcode:02X}"
        print(f"    [{i}] {op_name}({a1}, {a2}, {a3}) flags={flags}")

    tpu.load_program(program)

    # Execute
    print("\n  Executing inference on FPGA...")
    start_time = time.time()
    success = tpu.execute()
    elapsed = (time.time() - start_time) * 1000

    if success:
        print(f"  Execution completed in {elapsed:.2f} ms")
    else:
        print(f"  Execution timeout after {elapsed:.2f} ms")

    # Read results
    result_bytes = tpu.read_unified_buffer(1, 32)
    print(f"\n  Result from UB[1]: {result_bytes[:8].hex()}")

    # Interpret as int8 (or int32 depending on accumulator)
    result = np.frombuffer(result_bytes[:3], dtype=np.int8)
    print(f"  Interpreted result: {result}")

    return success


def demo_streaming_inference(tpu: TPUCoprocessor):
    """Demo streaming inference - multiple batches through coprocessor"""
    print("\n" + "="*60)
    print("DEMO 4: Streaming Inference (Batch Processing)")
    print("="*60)

    # Fixed weights (like a trained layer)
    weights = np.array([
        [2, 0, 1],
        [0, 3, 0],
        [1, 0, 2]
    ], dtype=np.int8)

    # Multiple input batches to process
    batches = [
        np.array([1, 2, 3], dtype=np.int8),
        np.array([4, 5, 6], dtype=np.int8),
        np.array([7, 8, 9], dtype=np.int8),
        np.array([10, 0, -10], dtype=np.int8),
    ]

    print(f"\n  Weights:\n{weights}")
    print(f"\n  Processing {len(batches)} batches...")

    # Load weights once
    for row in range(3):
        row_data = bytes(weights[row, :].tobytes().ljust(8, b'\x00'))
        tpu.write_weights(row, row_data)

    # Pre-load program (stays in instruction memory)
    enc = InstructionEncoder()
    program = [
        enc.load_weights(0, 1),     # RD_WEIGHT: Load weight tile 0
        enc.load_ub(0, 1),          # LD_UB: Load input from UB[0]
        enc.matmul(0, 0, 3),        # MATMUL: ub[0] -> acc[0], 3 rows
        enc.store_ub(1, 1),         # ST_UB: Store acc to UB[1]
        enc.halt()
    ]
    tpu.load_program(program)

    # Stream batches
    results = []
    total_time = 0

    for i, batch in enumerate(batches):
        # Load input
        input_bytes = batch.tobytes().ljust(32, b'\x00')
        tpu.write_unified_buffer(0, input_bytes)

        # Execute
        start = time.time()
        tpu.execute()
        elapsed = time.time() - start
        total_time += elapsed

        # Read result
        result_bytes = tpu.read_unified_buffer(1, 32)
        result = np.frombuffer(result_bytes[:3], dtype=np.int8)
        results.append(result)

        # CPU reference
        expected = weights @ batch.reshape(3, 1)

        print(f"\n  Batch {i+1}: Input={batch} -> FPGA={result} (Expected={expected.flatten()})")

    avg_latency = (total_time / len(batches)) * 1000
    print(f"\n  Average inference latency: {avg_latency:.2f} ms")
    print(f"  Throughput: {len(batches)/total_time:.1f} batches/sec")

    return True


# =============================================================================
# MAIN
# =============================================================================

def main():
    import sys

    if len(sys.argv) < 2:
        print("TPU Coprocessor Demo")
        print("====================")
        print(f"Usage: {sys.argv[0]} <UART_PORT>")
        print(f"Example: {sys.argv[0]} /dev/tty.usbserial-210183A27BE01")
        sys.exit(1)

    port = sys.argv[1]

    print("="*60)
    print("TPU FPGA COPROCESSOR - STREAMING INFERENCE DEMO")
    print("="*60)
    print(f"\nConnecting to FPGA on {port}...")

    try:
        tpu = TPUCoprocessor(port)

        # Run demos
        demos = [
            ("Basic Communication", demo_basic_communication),
            ("Instruction Execution", demo_instruction_execution),
            ("Simple Inference", demo_simple_inference),
            ("Streaming Inference", demo_streaming_inference),
        ]

        results = []
        for name, demo_fn in demos:
            try:
                success = demo_fn(tpu)
                results.append((name, success))
            except Exception as e:
                print(f"  ERROR: {e}")
                results.append((name, False))

        # Summary
        print("\n" + "="*60)
        print("DEMO SUMMARY")
        print("="*60)
        for name, success in results:
            status = "PASS" if success else "FAIL"
            print(f"  {name}: {status}")

        passed = sum(1 for _, s in results if s)
        print(f"\n  Total: {passed}/{len(results)} demos passed")

        tpu.close()

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nInterrupted")
        sys.exit(1)


if __name__ == '__main__':
    main()
