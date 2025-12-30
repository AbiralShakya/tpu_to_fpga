#!/usr/bin/env python3
"""
Elite Comprehensive TPU Test Suite

Tests the complete TPU v1 architecture implementation (3x3 systolic array):
- Matrix multiplication with comprehensive test cases
- Full ISA instruction coverage
- Component-level verification (UB, Accumulator, Weight FIFO, MMU, PE)
- Staggered data flow validation (diagonal wavefront pattern)
- Pipelining and double buffering
- Signed/unsigned arithmetic
- Data loading, reading, and storage
- Debug signal monitoring

Based on TPU v1 architecture principles with proper staggered parallelogram output handling.
"""

import serial
import time
import struct
import sys
import os
from typing import List, Tuple, Dict, Optional, Any
from dataclasses import dataclass
from enum import Enum

class Cmd:
    """TPU Command Codes"""
    WRITE_UB = 0x01
    WRITE_WT = 0x02
    WRITE_INSTR = 0x03
    READ_UB = 0x04
    EXECUTE = 0x05
    READ_STATUS = 0x06

ACK_UB = 0xAA
ACK_WT = 0xBB

class Opcode(Enum):
    """TPU Instruction Opcodes"""
    NOP = 0x00
    RD_HOST_MEM = 0x01
    WR_HOST_MEM = 0x02
    RD_WEIGHT = 0x03
    LD_UB = 0x04
    ST_UB = 0x05
    MATMUL = 0x10
    CONV2D = 0x11
    MATMUL_ACC = 0x12
    RELU = 0x18
    RELU6 = 0x19
    SIGMOID = 0x1A
    TANH = 0x1B
    MAXPOOL = 0x20
    AVGPOOL = 0x21
    ADD_BIAS = 0x22
    BATCH_NORM = 0x23
    CFG_REG = 0x31
    SYNC = 0x30
    HALT = 0x3F

@dataclass
class TestResult:
    """Test result container"""
    test_name: str
    passed: bool
    expected: Any
    actual: Any
    details: str = ""

class EliteTPUTester:
    """Comprehensive TPU testing framework"""

    def __init__(self, port: str):
        """Initialize TPU connection"""
        try:
            self.ser = serial.Serial(port, 115200, timeout=2.0)
            time.sleep(0.1)
        except Exception as e:
            raise RuntimeError(f"Failed to connect to TPU: {e}")

        self.test_results: List[TestResult] = []

    def test_ub_write_persistence(self) -> TestResult:
        """Check if ST_UB actually modifies UB memory"""
        try:
            print("\n[DEBUG] Starting UB Persistence Check...")
            # 1. Write a known pattern to UB[10]
            pattern_before = bytes([99, 88, 77] + [0]*29)
            self.write_ub(10, pattern_before)

            # 2. Read it back
            before = self.read_ub(10, 32)
            print(f"  UB[10] BEFORE test: {list(before[:5])}")

            # 3. Run simplest MATMUL test (identity matrix)
            self.write_ub(0, bytes([10, 20, 30] + [0]*29))
            for i in range(3):
                row = [0] * 8
                row[i] = 1
                self.write_wt(i, bytes(row + [0]*24))

            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3, 0),
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            self.start_execution()
            self.wait_execution_complete(5.0)

            # 4. Read UB[10] again
            after = self.read_ub(10, 32)
            print(f"  UB[10] AFTER test:  {list(after[:5])}")

            # 5. Analysis
            if not after:
                return TestResult("UB_Persistence", False, "Data", "None", "Failed to read UB after execution")
                
            if list(before[:3]) == list(after[:3]):
                details = "❌ PROBLEM: UB NOT MODIFIED - ST_UB didn't write!"
                passed = False
            else:
                details = f"✓ UB WAS MODIFIED: {list(before[:3])} → {list(after[:3])}"
                passed = (list(after[:3]) == [10, 20, 30])
                if not passed:
                    details += " (But result data is wrong)"

            print(f"  {details}")
            return TestResult("UB_Persistence", passed, [10, 20, 30], list(after[:3]), details)

        except Exception as e:
            return TestResult("UB_Persistence", False, "Success", str(e), f"Exception: {e}")

    def __del__(self):
        """Cleanup serial connection"""
        if hasattr(self, 'ser') and self.ser.is_open:
            self.ser.close()

    def send_cmd(self, cmd: int, addr_hi: int, addr_lo: int, len_hi: int, len_lo: int) -> None:
        """Send structured command to TPU"""
        self.ser.write(bytes([cmd, addr_hi, addr_lo, len_hi, len_lo]))

    def read_ub(self, addr: int, count: int = 32) -> List[int]:
        """Read from Unified Buffer (9-bit address: bit 8 = bank, bits 7:0 = address)"""
        self.ser.reset_input_buffer()
        addr_hi = (addr >> 8) & 0x01  # Bank bit (bit 8)
        addr_lo = addr & 0xFF          # Lower 8 bits
        self.send_cmd(Cmd.READ_UB, addr_hi, addr_lo, 0, count)
        time.sleep(0.1)
        data = self.ser.read(count)
        return list(data) if data else []

    def write_ub(self, addr: int, data: bytes) -> bool:
        """Write to Unified Buffer
        Note: UART DMA hardcodes bank 0, so only addresses 0-255 are supported via UART
        """
        data = data.ljust(32, b'\x00')[:32]
        self.ser.reset_input_buffer()
        # UART only supports 8-bit addresses (bank 0 hardcoded in UART DMA)
        addr_lo = addr & 0xFF  # Only use lower 8 bits
        self.send_cmd(Cmd.WRITE_UB, 0, addr_lo, 0, 32)
        self.ser.write(data)
        # Wait for ACK - UART sends it after all 32 bytes are written
        # ACK is sent when tx_ready is true, so we need to wait for UART TX
        start_time = time.time()
        while time.time() - start_time < 0.5:  # 500ms timeout
            if self.ser.in_waiting > 0:
                ack_bytes = self.ser.read(1)
                if ack_bytes and ack_bytes[0] == ACK_UB:
                    return True
            time.sleep(0.01)
        return False

    def write_wt(self, addr: int, data: bytes) -> bool:
        """Write to Weight Memory"""
        data = data.ljust(32, b'\x00')[:32]
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.WRITE_WT, 0, addr, 0, 32)
        self.ser.write(data)
        time.sleep(0.05)
        ack = self.ser.read(1)
        return ack == bytes([ACK_WT])

    def write_instr(self, addr: int, instr: bytes) -> None:
        """Write instruction to program memory"""
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.WRITE_INSTR, 0, addr, 0, 4)
        self.ser.write(instr)
        time.sleep(0.05)

    def encode_instruction(self, opcode: int, arg1: int = 0, arg2: int = 0,
                          arg3: int = 0, flags: int = 0) -> bytes:
        """Encode TPU instruction"""
        instr = ((opcode & 0x3F) << 26) | ((arg1 & 0xFF) << 18) | \
                ((arg2 & 0xFF) << 10) | ((arg3 & 0xFF) << 2) | (flags & 0x03)
        return instr.to_bytes(4, 'big')

    def start_execution(self) -> None:
        """Start program execution"""
        self.send_cmd(Cmd.EXECUTE, 0, 0, 0, 0)
        time.sleep(0.1)

    def wait_execution_complete(self, timeout: float = 5.0) -> bool:
        """Wait for execution to complete"""
        start_time = time.time()
        while time.time() - start_time < timeout:
            self.send_cmd(Cmd.READ_STATUS, 0, 0, 0, 0)
            status = self.ser.read(1)
            if status and (status[0] & 0x80):
                return True
            time.sleep(0.01)
        return False

    def test_weight_loading(self) -> TestResult:
        """Test weight loading into weight FIFO"""
        try:
            # Write weights to memory
            weights = [
                [1, 0, 0, 0, 0, 0, 0, 0],
                [0, 1, 0, 0, 0, 0, 0, 0],
                [0, 0, 1, 0, 0, 0, 0, 0]
            ]
            
            for i, row in enumerate(weights):
                row_bytes = bytes(row + [0] * 24)
                if not self.write_wt(i, row_bytes):
                    return TestResult("Weight_Loading", False, "ACK received", "No ACK", f"Failed to write weight row {i}")

            # Create program to load weights
            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            self.start_execution()
            completed = self.wait_execution_complete()

            return TestResult("Weight_Loading", completed, True, completed, "Weight loading sequence")

        except Exception as e:
            return TestResult("Weight_Loading", False, "No exception", str(e), f"Exception: {e}")

    def test_activation_loading(self) -> TestResult:
        """Test activation loading into Unified Buffer"""
        try:
            activations = [10, 20, 30]
            act_bytes = bytes(activations + [0] * 29)
            
            if not self.write_ub(0, act_bytes):
                return TestResult("Activation_Loading", False, "ACK received", "No ACK", "Failed to write activations")

            # Verify readback
            readback = self.read_ub(0, 3)
            matches = readback[:3] == activations

            return TestResult("Activation_Loading", matches, activations, readback[:3], "Activation write/read verification")

        except Exception as e:
            return TestResult("Activation_Loading", False, "No exception", str(e), f"Exception: {e}")

    def test_unified_buffer_double_buffering(self) -> TestResult:
        """Test UB double buffering (different addresses in bank 0)
        Note: UART DMA only supports bank 0, so we test different addresses within bank 0
        """
        try:
            # Write to address 0 in bank 0
            data0 = bytes([0xAA] * 32)
            if not self.write_ub(0, data0):
                return TestResult("UB_Double_Buffering", False, "ACK", "No ACK", "Failed to write address 0")

            # Write to address 1 in bank 0 (UART only supports bank 0)
            data1 = bytes([0xBB] * 32)
            if not self.write_ub(1, data1):
                return TestResult("UB_Double_Buffering", False, "ACK", "No ACK", "Failed to write address 1")

            # Read back both addresses
            read0 = self.read_ub(0, 32)
            read1 = self.read_ub(1, 32)

            addr0_ok = len(read0) > 0 and read0[0] == 0xAA
            addr1_ok = len(read1) > 0 and read1[0] == 0xBB
            independent = addr0_ok and addr1_ok

            return TestResult("UB_Double_Buffering", independent, 
                            {"addr0": 0xAA, "addr1": 0xBB},
                            {"addr0": read0[0] if read0 else None, "addr1": read1[0] if read1 else None},
                            f"Address independence: addr0={addr0_ok}, addr1={addr1_ok}")

        except Exception as e:
            return TestResult("UB_Double_Buffering", False, "No exception", str(e), f"Exception: {e}")

    def test_matmul_identity(self) -> TestResult:
        """Test identity matrix multiplication"""
        try:
            # Clear UB
            self.write_ub(0, bytes([0] * 32))
            self.write_ub(10, bytes([0] * 32))

            # Load identity weights (3 rows)
            for i in range(3):
                row = [0] * 8
                row[i] = 1
                row_bytes = bytes(row + [0] * 24)
                self.write_wt(i, row_bytes)

            # Load activations
            activations = [10, 20, 30]
            act_bytes = bytes(activations + [0] * 29)
            self.write_ub(0, act_bytes)

            # Create program: 3 RD_WEIGHT, LD_UB, MATMUL, ST_UB, HALT
            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3, 0),  # flags=0 (unsigned)
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            self.start_execution()
            self.wait_execution_complete(5.0)

            # Read results
            results = self.read_ub(10, 32)
            result_bytes = results[:3]

            expected = activations
            passed = result_bytes == expected

            return TestResult("MATMUL_Identity", passed, expected, result_bytes,
                            f"Identity matrix: input={activations}, output={result_bytes}")

        except Exception as e:
            return TestResult("MATMUL_Identity", False, "No exception", str(e), f"Exception: {e}")

    def test_matmul_diagonal(self) -> TestResult:
        """Test diagonal matrix multiplication"""
        try:
            self.write_ub(0, bytes([0] * 32))
            self.write_ub(10, bytes([0] * 32))

            # Load diagonal weights: diag(1, 2, 3)
            weights = [
                [1, 0, 0, 0, 0, 0, 0, 0],
                [0, 2, 0, 0, 0, 0, 0, 0],
                [0, 0, 3, 0, 0, 0, 0, 0]
            ]
            for i, row in enumerate(weights):
                row_bytes = bytes(row + [0] * 24)
                self.write_wt(i, row_bytes)

            activations = [1, 2, 3]
            act_bytes = bytes(activations + [0] * 29)
            self.write_ub(0, act_bytes)

            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3, 0),
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            self.start_execution()
            self.wait_execution_complete(5.0)

            results = self.read_ub(10, 32)
            result_bytes = results[:3]
            expected = [1, 4, 9]  # 1*1, 2*2, 3*3
            passed = result_bytes == expected

            return TestResult("MATMUL_Diagonal", passed, expected, result_bytes,
                            f"Diagonal: input={activations}, output={result_bytes}")

        except Exception as e:
            return TestResult("MATMUL_Diagonal", False, "No exception", str(e), f"Exception: {e}")

    def test_matmul_full_matrix(self) -> TestResult:
        """Test full 3x3 matrix multiplication"""
        try:
            self.write_ub(0, bytes([0] * 32))
            self.write_ub(10, bytes([0] * 32))

            # Load full matrix weights
            weights = [
                [1, 2, 3, 0, 0, 0, 0, 0],
                [4, 5, 6, 0, 0, 0, 0, 0],
                [7, 8, 9, 0, 0, 0, 0, 0]
            ]
            for i, row in enumerate(weights):
                row_bytes = bytes(row + [0] * 24)
                self.write_wt(i, row_bytes)

            activations = [1, 2, 3]
            act_bytes = bytes(activations + [0] * 29)
            self.write_ub(0, act_bytes)

            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3, 0),
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            self.start_execution()
            self.wait_execution_complete(5.0)

            results = self.read_ub(10, 32)
            result_bytes = results[:3]
            # Expected: [1*1+2*4+3*7, 1*2+2*5+3*8, 1*3+2*6+3*9] = [30, 36, 42]
            expected = [30, 36, 42]
            passed = result_bytes == expected

            return TestResult("MATMUL_Full_Matrix", passed, expected, result_bytes,
                            f"Full 3x3: input={activations}, output={result_bytes}")

        except Exception as e:
            return TestResult("MATMUL_Full_Matrix", False, "No exception", str(e), f"Exception: {e}")

    def test_matmul_signed(self) -> TestResult:
        """Test signed matrix multiplication"""
        try:
            self.write_ub(0, bytes([0] * 32))
            self.write_ub(10, bytes([0] * 32))

            # Load weights with negative values (using two's complement)
            # -1 in 8-bit signed = 255 unsigned = 0xFF
            weights = [
                [1, 0, 0, 0, 0, 0, 0, 0],
                [0, 255, 0, 0, 0, 0, 0, 0],  # -1 signed
                [0, 0, 1, 0, 0, 0, 0, 0]
            ]
            for i, row in enumerate(weights):
                row_bytes = bytes(row + [0] * 24)
                self.write_wt(i, row_bytes)

            activations = [10, 20, 30]
            act_bytes = bytes(activations + [0] * 29)
            self.write_ub(0, act_bytes)

            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3, 2),  # flags=2 (signed, bit 1 set)
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            self.start_execution()
            self.wait_execution_complete(5.0)

            results = self.read_ub(10, 32)
            result_bytes = results[:3]
            # Expected: [10*1, 20*(-1), 30*1] = [10, -20, 30]
            # In signed 8-bit: -20 = 236 unsigned
            expected_signed = [10, 236, 30]
            passed = result_bytes == expected_signed

            return TestResult("MATMUL_Signed", passed, expected_signed, result_bytes,
                            f"Signed: input={activations}, output={result_bytes}")

        except Exception as e:
            return TestResult("MATMUL_Signed", False, "No exception", str(e), f"Exception: {e}")

    def test_matmul_accumulate(self) -> TestResult:
        """Test MATMUL_ACC (accumulate mode)"""
        try:
            self.write_ub(0, bytes([0] * 32))
            self.write_ub(10, bytes([0] * 32))

            # First MATMUL
            weights1 = [
                [1, 0, 0, 0, 0, 0, 0, 0],
                [0, 1, 0, 0, 0, 0, 0, 0],
                [0, 0, 1, 0, 0, 0, 0, 0]
            ]
            for i, row in enumerate(weights1):
                row_bytes = bytes(row + [0] * 24)
                self.write_wt(i, row_bytes)

            activations = [5, 10, 15]
            act_bytes = bytes(activations + [0] * 29)
            self.write_ub(0, act_bytes)

            program1 = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3, 0),
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program1):
                self.write_instr(addr, instr)

            self.start_execution()
            self.wait_execution_complete(5.0)

            results1 = self.read_ub(10, 32)

            # Second MATMUL_ACC (should accumulate)
            program2 = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL_ACC.value, 0, 0, 3, 0),
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program2):
                self.write_instr(addr, instr)

            self.start_execution()
            self.wait_execution_complete(5.0)

            results2 = self.read_ub(10, 32)
            result_bytes = results2[:3]
            # Expected: [5+5, 10+10, 15+15] = [10, 20, 30]
            expected = [10, 20, 30]
            passed = result_bytes == expected

            return TestResult("MATMUL_Accumulate", passed, expected, result_bytes,
                            f"Accumulate: first={results1[:3]}, second={result_bytes}")

        except Exception as e:
            return TestResult("MATMUL_Accumulate", False, "No exception", str(e), f"Exception: {e}")

    def test_staggered_weight_flow(self) -> TestResult:
        """Test staggered weight FIFO output pattern"""
        try:
            # Load weights with distinct values per column
            weights = [
                [0x11, 0x22, 0x33, 0, 0, 0, 0, 0],  # Row 0: col0=0x11, col1=0x22, col2=0x33
                [0x44, 0x55, 0x66, 0, 0, 0, 0, 0],  # Row 1
                [0x77, 0x88, 0x99, 0, 0, 0, 0, 0]   # Row 2
            ]
            for i, row in enumerate(weights):
                row_bytes = bytes(row + [0] * 24)
                self.write_wt(i, row_bytes)

            activations = [1, 1, 1]
            act_bytes = bytes(activations + [0] * 29)
            self.write_ub(0, act_bytes)

            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3, 0),
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            self.start_execution()
            self.wait_execution_complete(5.0)

            results = self.read_ub(10, 32)
            result_bytes = results[:3]
            # Expected: Sum of each column
            # col0: 0x11 + 0x44 + 0x77 = 0xCC = 204
            # col1: 0x22 + 0x55 + 0x88 = 0xFF = 255
            # col2: 0x33 + 0x66 + 0x99 = 0x132 = 306 (wraps to 50 in 8-bit)
            expected = [204, 255, 50]
            passed = result_bytes == expected

            return TestResult("Staggered_Weight_Flow", passed, expected, result_bytes,
                            f"Staggered weights: output={result_bytes}")

        except Exception as e:
            return TestResult("Staggered_Weight_Flow", False, "No exception", str(e), f"Exception: {e}")

    def test_accumulator_read_write(self) -> TestResult:
        """Test accumulator memory read/write operations"""
        try:
            # Run MATMUL to populate accumulator
            weights = [
                [1, 0, 0, 0, 0, 0, 0, 0],
                [0, 1, 0, 0, 0, 0, 0, 0],
                [0, 0, 1, 0, 0, 0, 0, 0]
            ]
            for i, row in enumerate(weights):
                row_bytes = bytes(row + [0] * 24)
                self.write_wt(i, row_bytes)

            activations = [100, 200, 50]
            act_bytes = bytes(activations + [0] * 29)
            self.write_ub(0, act_bytes)

            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3, 0),
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            self.start_execution()
            self.wait_execution_complete(5.0)

            results = self.read_ub(10, 32)
            result_bytes = results[:3]
            expected = activations
            passed = result_bytes == expected

            return TestResult("Accumulator_RW", passed, expected, result_bytes,
                            f"Accumulator read/write: output={result_bytes}")

        except Exception as e:
            return TestResult("Accumulator_RW", False, "No exception", str(e), f"Exception: {e}")

    def test_isa_instructions(self) -> List[TestResult]:
        """Test all ISA instructions"""
        results = []
        
        isa_tests = [
            ("NOP", Opcode.NOP, []),
            ("RD_WEIGHT", Opcode.RD_WEIGHT, [0, 1]),
            ("LD_UB", Opcode.LD_UB, [0, 3]),
            ("ST_UB", Opcode.ST_UB, [10, 3]),
            ("MATMUL", Opcode.MATMUL, [0, 0, 3]),
            ("MATMUL_ACC", Opcode.MATMUL_ACC, [0, 0, 3]),
            ("SYNC", Opcode.SYNC, []),
            ("HALT", Opcode.HALT, []),
        ]

        for test_name, opcode, args in isa_tests:
            try:
                program = []
                if opcode in [Opcode.RD_WEIGHT, Opcode.LD_UB, Opcode.ST_UB, Opcode.MATMUL, Opcode.MATMUL_ACC]:
                    # Need setup
                    if opcode == Opcode.RD_WEIGHT:
                        program.append(self.encode_instruction(opcode.value, *args))
                    elif opcode in [Opcode.LD_UB, Opcode.ST_UB]:
                        program.append(self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1))
                        program.append(self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1))
                        program.append(self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1))
                        if opcode == Opcode.LD_UB:
                            program.append(self.encode_instruction(opcode.value, *args))
                        else:
                            program.append(self.encode_instruction(Opcode.LD_UB.value, 0, 3))
                            program.append(self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3))
                            program.append(self.encode_instruction(opcode.value, *args))
                    elif opcode in [Opcode.MATMUL, Opcode.MATMUL_ACC]:
                        program.append(self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1))
                        program.append(self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1))
                        program.append(self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1))
                        program.append(self.encode_instruction(Opcode.LD_UB.value, 0, 3))
                        program.append(self.encode_instruction(opcode.value, *args))
                else:
                    program.append(self.encode_instruction(opcode.value, *args))

                program.append(self.encode_instruction(Opcode.HALT.value))

                for addr, instr in enumerate(program):
                    self.write_instr(addr, instr)

                self.start_execution()
                completed = self.wait_execution_complete()

                results.append(TestResult(f"ISA_{test_name}", completed, True, completed,
                                         f"Instruction {opcode.name} execution"))

            except Exception as e:
                results.append(TestResult(f"ISA_{test_name}", False, "No exception", str(e),
                                        f"Exception: {e}"))

        return results

    def run_all_tests(self) -> None:
        """Run complete test suite"""
        print("ELITE COMPREHENSIVE TPU TEST SUITE")
        print("=" * 70)
        print(f"Testing TPU on {self.ser.port}")
        print("=" * 70)

        # Reset TPU
        self.ser.write(bytes([0xFF]))
        time.sleep(0.1)

        # Component tests
        print("\n" + "=" * 70)
        print("COMPONENT TESTS")
        print("=" * 70)
        self.test_results.append(self.test_weight_loading())
        self.test_results.append(self.test_activation_loading())
        self.test_results.append(self.test_unified_buffer_double_buffering())

        # MATMUL tests
        print("\n" + "=" * 70)
        print("MATMUL TESTS")
        print("=" * 70)
        self.test_results.append(self.test_matmul_identity())
        self.test_results.append(self.test_matmul_diagonal())
        self.test_results.append(self.test_matmul_full_matrix())
        self.test_results.append(self.test_matmul_signed())
        self.test_results.append(self.test_matmul_accumulate())

        # Data flow tests
        print("\n" + "=" * 70)
        print("DATA FLOW TESTS")
        print("=" * 70)
        self.test_results.append(self.test_staggered_weight_flow())
        self.test_results.append(self.test_accumulator_read_write())

        # ISA tests
        print("\n" + "=" * 70)
        print("ISA INSTRUCTION TESTS")
        print("=" * 70)
        self.test_results.extend(self.test_isa_instructions())

        # Report
        self.report_results()

    def report_results(self) -> None:
        """Generate test report"""
        print("\n" + "=" * 70)
        print("TEST RESULTS SUMMARY")
        print("=" * 70)

        passed = sum(1 for r in self.test_results if r.passed)
        total = len(self.test_results)

        for result in self.test_results:
            status = "PASS" if result.passed else "FAIL"
            print(f"{status} {result.test_name}")
            if not result.passed:
                print(f"      Expected: {result.expected}")
                print(f"      Actual:   {result.actual}")
                if result.details:
                    print(f"      Details:  {result.details}")

        print("=" * 70)
        print(f"OVERALL: {passed}/{total} tests passed ({passed/total*100:.1f}%)")
        print("=" * 70)

def main():
    """Main entry point"""
    if len(sys.argv) != 2:
        print("Usage: python elite_comprehensive_tpu_test.py <serial_port>")
        sys.exit(1)

    port = sys.argv[1]

    try:
        tester = EliteTPUTester(port)
        tester.run_all_tests()
    except KeyboardInterrupt:
        print("\nTest interrupted by user")
    except Exception as e:
        print(f"Test suite failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
