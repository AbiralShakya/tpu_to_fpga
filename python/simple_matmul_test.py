#!/usr/bin/env python3
"""
Simple MATMUL test to isolate the issue
"""

import serial
import struct
import time
import numpy as np
from dataclasses import dataclass
from typing import Optional, List, Tuple
from enum import IntEnum

class Opcode(IntEnum):
    RD_WEIGHT   = 0x03
    LD_UB       = 0x04
    ST_UB       = 0x05
    MATMUL      = 0x10
    HALT        = 0x3F

class UARTCommand(IntEnum):
    WRITE_UB    = 0x01
    WRITE_WT    = 0x02
    WRITE_INSTR = 0x03
    READ_UB     = 0x04
    EXECUTE     = 0x05
    READ_STATUS = 0x06

@dataclass
class TPUStatus:
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

class InstructionEncoder:
    @staticmethod
    def encode(opcode: Opcode, arg1: int = 0, arg2: int = 0,
               arg3: int = 0, flags: int = 0) -> int:
        return ((opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags)

    @staticmethod
    def load_weights(start_tile: int, num_tiles: int = 1, buf_sel: int = 0) -> int:
        return InstructionEncoder.encode(Opcode.RD_WEIGHT, start_tile, num_tiles, 0, buf_sel)

    @staticmethod
    def load_ub(ub_addr: int, num_blocks: int = 1) -> int:
        return InstructionEncoder.encode(Opcode.LD_UB, ub_addr, num_blocks, 0)

    @staticmethod
    def store_ub(ub_addr: int, num_blocks: int = 1) -> int:
        return InstructionEncoder.encode(Opcode.ST_UB, ub_addr, num_blocks, 0)

    @staticmethod
    def matmul(ub_input_addr: int, acc_output_addr: int, num_rows: int = 3,
               transpose: bool = False, signed: bool = True) -> int:
        flags = (1 if transpose else 0) | (2 if signed else 0)
        return InstructionEncoder.encode(Opcode.MATMUL, ub_input_addr, acc_output_addr, num_rows, flags)

    @staticmethod
    def halt() -> int:
        return InstructionEncoder.encode(Opcode.HALT)

class UARTTransport:
    ACK_BYTE_UB = 0xAA
    ACK_BYTE_WT = 0xBB
    NACK_BYTE = 0xFF

    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 2.0):
        self.ser = serial.Serial(port, baudrate, timeout=timeout)
        time.sleep(0.1)
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

    def close(self):
        self.ser.close()

    def send_command(self, cmd: UARTCommand, addr_hi: int = 0, addr_lo: int = 0,
                     len_hi: int = 0, len_lo: int = 0) -> None:
        header = bytes([cmd, addr_hi, addr_lo, len_hi, len_lo])
        self.ser.write(header)

    def send_data(self, data: bytes) -> None:
        self.ser.write(data)

    def wait_ack(self, expected: int = None, timeout: float = 1.0) -> bool:
        self.ser.timeout = timeout
        ack = self.ser.read(1)
        if not ack:
            return False
        if expected is not None:
            return ack[0] == expected
        return ack[0] in (self.ACK_BYTE_UB, self.ACK_BYTE_WT)

    def read_bytes(self, count: int) -> bytes:
        return self.ser.read(count)

    def flush(self) -> None:
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

class TPUCoprocessor:
    def __init__(self, port: str, baudrate: int = 115200):
        self.uart = UARTTransport(port, baudrate)
        self.encoder = InstructionEncoder()
        print(f"TPU Coprocessor connected on {port}")

    def close(self):
        self.uart.close()

    def read_status(self) -> TPUStatus:
        self.uart.send_command(UARTCommand.READ_STATUS)
        time.sleep(0.02)
        status_byte = self.uart.read_bytes(1)
        if status_byte:
            return TPUStatus.from_byte(status_byte[0])
        return TPUStatus.from_byte(0)

    def wait_idle(self, timeout: float = 5.0) -> bool:
        start = time.time()
        while time.time() - start < timeout:
            status = self.read_status()
            if status.is_idle():
                return True
            time.sleep(0.01)
        return False

    def write_unified_buffer(self, addr: int, data: bytes) -> bool:
        self.uart.flush()
        if len(data) < 32:
            data = data + bytes(32 - len(data))
        elif len(data) > 32:
            data = data[:32]

        self.uart.send_command(UARTCommand.WRITE_UB, 0, addr, 0, len(data))
        self.uart.send_data(data)
        time.sleep(0.05)
        return self.uart.wait_ack(expected=UARTTransport.ACK_BYTE_UB)

    def read_unified_buffer(self, addr: int, length: int = 32) -> bytes:
        self.uart.send_command(UARTCommand.READ_UB, 0, addr, 0, length)
        time.sleep(0.05)
        return self.uart.read_bytes(length)

    def write_weights(self, addr: int, data: bytes) -> bool:
        self.uart.flush()
        if len(data) < 8:
            data = data + bytes(8 - len(data))

        self.uart.send_command(UARTCommand.WRITE_WT, 0, addr, 0, len(data))
        self.uart.send_data(data)
        time.sleep(0.05)
        return self.uart.wait_ack(expected=UARTTransport.ACK_BYTE_WT)

    def write_instruction(self, addr: int, instr: int) -> None:
        cmd = bytes([UARTCommand.WRITE_INSTR, 0, addr])
        self.uart.ser.write(cmd)
        self.uart.ser.write(struct.pack('>I', instr))
        time.sleep(0.01)

    def load_program(self, instructions: List[int]) -> None:
        for addr, instr in enumerate(instructions):
            self.write_instruction(addr, instr)

    def execute(self) -> bool:
        self.uart.ser.write(bytes([UARTCommand.EXECUTE]))
        time.sleep(0.1)
        return self.wait_idle(timeout=2.0)

def test_data_movement(tpu: TPUCoprocessor):
    """Test 1: Can we move data from UB[0] to UB[1] without computation?"""
    print("=== TEST 1: Data Movement (LD_UB + ST_UB) ===")

    # Test data
    test_data = bytes([0x11, 0x22, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00])

    # Write to UB[0]
    print(f"Writing {test_data.hex()} to UB[0]...")
    if not tpu.write_unified_buffer(0, test_data):
        print("❌ Failed to write to UB[0]")
        return False

    # Verify write
    readback = tpu.read_unified_buffer(0, 32)
    if readback[:8] != test_data:
        print(f"❌ Write verification failed: wrote {test_data.hex()}, read {readback[:8].hex()}")
        return False
    print("✅ Write to UB[0] verified")

    # Copy UB[0] to UB[1] using LD_UB + ST_UB
    program = [
        InstructionEncoder.load_ub(0, 1),    # LD_UB(0, 1, 0)
        InstructionEncoder.store_ub(1, 1),   # ST_UB(1, 1, 0)
        InstructionEncoder.halt()
    ]

    print("Loading copy program...")
    tpu.load_program(program)

    print("Executing copy...")
    if not tpu.execute():
        print("❌ Copy execution failed")
        return False

    # Read from UB[1]
    result = tpu.read_unified_buffer(1, 32)
    print(f"Read from UB[1]: {result[:8].hex()}")

    if result[:8] == test_data:
        print("✅ Data movement test PASSED")
        return True
    else:
        print(f"❌ Data movement test FAILED: expected {test_data.hex()}, got {result[:8].hex()}")
        return False

def test_weight_loading(tpu: TPUCoprocessor):
    """Test 2: Can we load weights?"""
    print("\n=== TEST 2: Weight Loading ===")

    # Simple weight data
    weight_data = bytes([0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])  # [1, 0, 0]

    print(f"Writing weight {weight_data.hex()} to address 0...")
    if not tpu.write_weights(0, weight_data):
        print("❌ Failed to write weight")
        return False

    print("✅ Weight loading test PASSED (write succeeded)")
    return True

def test_matmul_minimal(tpu: TPUCoprocessor):
    """Test 3: Minimal MATMUL with known inputs"""
    print("\n=== TEST 3: Minimal MATMUL ===")

    # Identity weights: [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
    weights = [
        bytes([0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),  # [1, 0, 0]
        bytes([0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),  # [0, 1, 0]
        bytes([0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00])   # [0, 0, 1]
    ]

    # Input vector: [10, 20, 30]
    input_data = bytes([0x0A, 0x14, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00])

    # Load weights
    print("Loading weights...")
    for i, weight in enumerate(weights):
        if not tpu.write_weights(i, weight):
            print(f"❌ Failed to load weight {i}")
            return False
        print(f"  Weight {i}: {weight.hex()}")

    # Load input
    print(f"\nLoading input: {input_data.hex()}")
    if not tpu.write_unified_buffer(0, input_data):
        print("❌ Failed to load input")
        return False

    # MATMUL program
    program = [
        InstructionEncoder.load_weights(0, 1),  # RD_WEIGHT(0, 1, 0)
        InstructionEncoder.load_weights(1, 1),  # RD_WEIGHT(1, 1, 0)
        InstructionEncoder.load_weights(2, 1),  # RD_WEIGHT(2, 1, 0)
        InstructionEncoder.load_ub(0, 1),        # LD_UB(0, 1, 0)
        InstructionEncoder.matmul(0, 0, 3),      # MATMUL(0, 0, 3) flags=2
        InstructionEncoder.store_ub(1, 1),        # ST_UB(1, 1, 0)
        InstructionEncoder.halt()
    ]

    print("\nLoading MATMUL program...")
    for i, instr in enumerate(program):
        opcode = (instr >> 26) & 0x3F
        arg1 = (instr >> 18) & 0xFF
        arg2 = (instr >> 10) & 0xFF
        arg3 = (instr >> 2) & 0xFF
        flags = instr & 0x03
        op_names = {0x03: "RD_WEIGHT", 0x04: "LD_UB", 0x10: "MATMUL", 0x05: "ST_UB", 0x3F: "HALT"}
        op_name = op_names.get(opcode, f"0x{opcode:02X}")
        print(f"  [{i}] {op_name}({arg1}, {arg2}, {arg3}) flags={flags}")

    tpu.load_program(program)

    print("\nExecuting MATMUL...")
    if not tpu.execute():
        print("❌ MATMUL execution failed")
        return False

    # Read result
    result = tpu.read_unified_buffer(1, 32)
    print(f"\nRaw result: {result.hex()}")

    # Extract lower 8 bits of each 32-bit accumulator result
    acc0 = result[0]  # Lower 8 bits of accumulator 0
    acc1 = result[1]  # Lower 8 bits of accumulator 1
    acc2 = result[2]  # Lower 8 bits of accumulator 2

    print(f"Accumulator results: [{acc0}, {acc1}, {acc2}] (as int8: [{acc0 if acc0 < 128 else acc0-256}, {acc1 if acc1 < 128 else acc1-256}, {acc2 if acc2 < 128 else acc2-256}])")

    # Expected: [10, 20, 30]
    expected = [10, 20, 30]
    actual = [acc0, acc1, acc2]

    if actual == expected:
        print("✅ MATMUL test PASSED!")
        return True
    else:
        print(f"❌ MATMUL test FAILED: expected {expected}, got {actual}")
        return False

def main():
    import sys

    if len(sys.argv) < 2:
        print("Usage: python3 simple_matmul_test.py <UART_PORT>")
        sys.exit(1)

    port = sys.argv[1]

    try:
        tpu = TPUCoprocessor(port)

        # Run tests
        test1_pass = test_data_movement(tpu)
        test2_pass = test_weight_loading(tpu)
        test3_pass = test_matmul_minimal(tpu)

        tpu.close()

        print("\n=== SUMMARY ===")
        print(f"Test 1 (Data Movement): {'PASS' if test1_pass else 'FAIL'}")
        print(f"Test 2 (Weight Loading): {'PASS' if test2_pass else 'FAIL'}")
        print(f"Test 3 (MATMUL): {'PASS' if test3_pass else 'FAIL'}")

        if test1_pass and test2_pass and test3_pass:
            print("\n🎉 ALL TESTS PASSED!")
        else:
            print("\n❌ SOME TESTS FAILED")
            if not test1_pass:
                print("  - Data movement (LD_UB/ST_UB) is broken")
            if not test2_pass:
                print("  - Weight loading is broken")
            if not test3_pass:
                print("  - MATMUL computation is broken")

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nInterrupted")
        sys.exit(1)

if __name__ == '__main__':
    main()
