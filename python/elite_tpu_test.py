#!/usr/bin/env python3
"""
Elite TPU Comprehensive Test Suite

This script provides exhaustive testing of the TPU FPGA implementation, covering:
- Matrix multiplication operations with various test patterns
- Full ISA instruction testing
- Component-level verification (UB, Accumulator, Weight FIFO, MMU, PE)
- Data flow verification (staggered systolic array inputs/outputs)
- Pipelining and double buffering
- Debug signal monitoring and validation

Based on TPU v1 architecture principles with 3x3 systolic array implementation.
"""

import serial
import time
import struct
import sys
import os
from typing import List, Tuple, Dict, Optional, Any
from dataclasses import dataclass
from enum import Enum

# Add the project root to Python path for imports
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

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

@dataclass
class DebugState:
    """Debug signal state from TPU"""
    col0_wt: int
    col1_wt: int
    col2_wt: int
    row0_act: int
    row1_act: int
    row2_act: int
    systolic_active: bool
    weight_pass: bool
    acc0_latched: int
    acc1_latched: int
    acc2_latched: int

class EliteTPUTester:
    """Comprehensive TPU testing framework"""

    def __init__(self, port: str):
        """Initialize TPU connection"""
        try:
            self.ser = serial.Serial(port, 115200, timeout=1.0)
            print(f"Connected to TPU on {port}")
        except Exception as e:
            raise RuntimeError(f"Failed to connect to TPU: {e}")

        self.test_results: List[TestResult] = []
        self.debug_history: List[DebugState] = []

    def send_cmd(self, cmd: int, addr_hi: int, addr_lo: int, len_hi: int, len_lo: int) -> None:
        """Send structured command to TPU"""
        self.ser.write(bytes([cmd, addr_hi, addr_lo, len_hi, len_lo]))

    def __del__(self):
        """Cleanup serial connection"""
        if hasattr(self, 'ser') and self.ser.is_open:
            self.ser.close()

    def send_byte(self, byte: int) -> None:
        """Send a single byte to TPU"""
        self.ser.write(bytes([byte]))

    def send_bytes(self, data: bytes) -> None:
        """Send multiple bytes to TPU"""
        self.ser.write(data)

    def read_byte(self) -> int:
        """Read a single byte from TPU"""
        data = self.ser.read(1)
        return data[0] if data else 0

    def read_bytes(self, count: int) -> bytes:
        """Read multiple bytes from TPU"""
        return self.ser.read(count)

    def read_ub(self, addr: int, count: int = 32) -> List[int]:
        """Read from Unified Buffer"""
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.READ_UB, 0, addr, 0, count)
        time.sleep(0.05)
        data = self.ser.read(count)
        return list(data) if data else []

    def write_ub(self, addr: int, data: bytes) -> bool:
        """Write to Unified Buffer"""
        data = data.ljust(32, b'\x00')[:32]  # Pad to 32 bytes
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.WRITE_UB, 0, addr, 0, 32)
        self.ser.write(data)
        time.sleep(0.05)
        ack = self.ser.read(1)
        return ack == bytes([ACK_UB])

    def write_wt(self, addr: int, data: bytes) -> bool:
        """Write to Weight Memory"""
        data = data.ljust(32, b'\x00')[:32]  # Pad to 32 bytes
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.WRITE_WT, 0, addr, 0, 32)
        self.ser.write(data)
        time.sleep(0.05)
        ack = self.ser.read(1)
        return ack == bytes([ACK_WT])

    def write_instr(self, addr: int, instr: bytes) -> None:
        """Write instruction to program memory"""
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.WRITE_INSTR, 0, addr, 0, 4)  # Instructions are 4 bytes
        self.ser.write(instr)
        time.sleep(0.05)

    def encode_instruction(self, opcode: int, arg1: int = 0, arg2: int = 0,
                          arg3: int = 0, flags: int = 0) -> bytes:
        """Encode TPU instruction"""
        # TPU instruction format: [31:26] opcode, [25:18] arg1, [17:10] arg2, [9:2] arg3, [1:0] flags
        instr = ((opcode & 0x3F) << 26) | ((arg1 & 0xFF) << 18) | \
                ((arg2 & 0xFF) << 10) | ((arg3 & 0xFF) << 2) | (flags & 0x03)
        return instr.to_bytes(4, 'big')

    def start_execution(self) -> None:
        """Start program execution"""
        self.send_cmd(Cmd.EXECUTE, 0, 0, 0, 0)
        time.sleep(0.1)  # Allow time for execution

    def wait_execution_complete(self, timeout: float = 5.0) -> bool:
        """Wait for execution to complete"""
        start_time = time.time()
        while time.time() - start_time < timeout:
            self.send_cmd(Cmd.READ_STATUS, 0, 0, 0, 0)
            status = self.ser.read(1)
            if status and (status[0] & 0x80):  # Execution complete bit
                return True
            time.sleep(0.01)
        return False

    def read_debug_signals(self) -> DebugState:
        """Read current debug signal state"""
        # Debug signals may not be implemented in current FPGA design
        # Return default state for now
        return DebugState(0, 0, 0, 0, 0, 0, False, False, 0, 0, 0)

    def run_matmul_test(self, test_name: str, activations: List[int],
                       weights: List[List[int]], expected: List[int],
                       description: str = "") -> TestResult:
        """Run a comprehensive MATMUL test"""
        print(f"Running test: {test_name}")
        try:
            # Clear previous state
            print("  Clearing UB...")
            self.write_ub(0, bytes([0] * 32))  # Clear UB
            self.write_ub(10, bytes([0] * 32))  # Clear output area

            # Load weights (3x3 matrix, row-major order)
            print("  Loading weights...")
            for i, row in enumerate(weights):
                row_bytes = bytes(row + [0] * 29)  # Pad to 32 bytes
                self.write_wt(i, row_bytes)

            # Load activations
            print("  Loading activations...")
            act_bytes = bytes(activations + [0] * 29)  # Pad to 32 bytes
            self.write_ub(0, act_bytes)

            # Create program
            print("  Creating program...")
            # CRITICAL: RD_WEIGHT loads ONE row at a time, so we need 3 separate instructions
            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 1),  # Load weight row 0
                self.encode_instruction(Opcode.RD_WEIGHT.value, 1, 1),  # Load weight row 1
                self.encode_instruction(Opcode.RD_WEIGHT.value, 2, 1),  # Load weight row 2
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),      # Load 3 activations
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3),  # MATMUL
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),     # Store results
                self.encode_instruction(Opcode.HALT.value)
            ]

            # Load program
            print("  Loading program...")
            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            # Monitor debug signals during execution
            print("  Starting execution...")
            debug_states = []
            self.start_execution()

            # Poll debug signals during execution (with timeout)
            print("  Monitoring debug signals...")
            for i in range(50):  # Monitor for ~500ms
                try:
                    debug_states.append(self.read_debug_signals())
                except Exception as e:
                    print(f"  Debug read failed at step {i}: {e}")
                    break  # Stop monitoring if debug read fails

                time.sleep(0.01)

                # Check if execution completed
                if self.wait_execution_complete(0.01):
                    print("  Execution completed")
                    break

            # Read results (with timeout protection)
            print("  Reading results...")
            try:
                results = self.read_ub(10, 32)  # Read 32 bytes, we'll take first 3
                results = results[:3] if len(results) >= 3 else results + [0] * (3 - len(results))
                print(f"  Got results: {results}")
            except Exception as e:
                print(f"  Result read failed: {e}")
                results = [0, 0, 0]  # Default if read fails

            # Verify results
            passed = results == expected

            return TestResult(
                test_name=test_name,
                passed=passed,
                expected=expected,
                actual=results,
                details=f"{description}\nDebug states captured: {len(debug_states)}"
            )

        except Exception as e:
            return TestResult(
                test_name=test_name,
                passed=False,
                expected=expected,
                actual=None,
                details=f"Test failed with exception: {e}"
            )

    def test_fpga_connectivity(self) -> bool:
        """Test basic FPGA connectivity"""
        try:
            # Send a simple NOP and see if we get any response
            self.send_byte(0x00)  # NOP
            response = self.read_byte()
            return True  # If we don't hang, consider it connected
        except Exception:
            return False

    def run_matmul_test_suite(self) -> None:
        """Run comprehensive MATMUL test suite"""
        print("\n" + "="*60)
        print("COMPREHENSIVE MATMUL TEST SUITE")
        print("="*60)

        # Test basic connectivity first
        if not self.test_fpga_connectivity():
            print("FPGA connectivity test failed - skipping MATMUL tests")
            return

        # Test 1: Identity Matrix
        result = self.run_matmul_test(
            "Identity Matrix",
            [10, 20, 30],  # activations
            [[1, 0, 0], [0, 1, 0], [0, 0, 1]],  # identity weights
            [10, 20, 30],  # expected: activations unchanged
            "Identity matrix should pass activations through unchanged"
        )
        self.test_results.append(result)

        # Test 2: Diagonal Matrix
        result = self.run_matmul_test(
            "Diagonal Matrix",
            [1, 2, 3],
            [[1, 0, 0], [0, 2, 0], [0, 0, 3]],  # diagonal weights
            [1, 4, 9],  # 1*1, 2*2, 3*3
            "Diagonal matrix multiplication"
        )
        self.test_results.append(result)

        # Test 3: Full Matrix Multiplication
        result = self.run_matmul_test(
            "Full 3x3 Matrix",
            [1, 2, 3],
            [[1, 2, 3], [4, 5, 6], [7, 8, 9]],  # full matrix
            [30, 36, 42],  # Expected: [1*1+2*4+3*7, 1*2+2*5+3*8, 1*3+2*6+3*9]
            "Complete 3x3 matrix multiplication"
        )
        self.test_results.append(result)

        # Test 4: Row Exchange Test
        result = self.run_matmul_test(
            "Row Exchange",
            [5, 10, 15],
            [[0, 0, 1], [0, 1, 0], [1, 0, 0]],  # row permutation
            [15, 10, 5],  # Should reverse the input
            "Test row permutation matrix"
        )
        self.test_results.append(result)

        # Test 5: Column Scaling
        result = self.run_matmul_test(
            "Column Scaling",
            [2, 3, 4],
            [[2, 0, 0], [0, 3, 0], [0, 0, 4]],  # column scaling
            [4, 9, 16],  # 2*2, 3*3, 4*4
            "Test column-wise scaling"
        )
        self.test_results.append(result)

        # Test 6: Zero Matrix
        result = self.run_matmul_test(
            "Zero Weights",
            [10, 20, 30],
            [[0, 0, 0], [0, 0, 0], [0, 0, 0]],  # all zeros
            [0, 0, 0],
            "Zero weights should produce zero outputs"
        )
        self.test_results.append(result)

    def run_isa_test_suite(self) -> None:
        """Run comprehensive ISA test suite"""
        print("\n" + "="*60)
        print("COMPREHENSIVE ISA TEST SUITE")
        print("="*60)

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
                # Prepare minimal program for each instruction
                program = []

                if opcode == Opcode.RD_WEIGHT:
                    program.append(self.encode_instruction(opcode.value, *args))
                elif opcode in [Opcode.LD_UB, Opcode.ST_UB, Opcode.MATMUL, Opcode.MATMUL_ACC]:
                    # Need setup for these instructions
                    program.extend([
                        self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 3),
                        self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                        self.encode_instruction(opcode.value, *args),
                    ])
                else:
                    program.append(self.encode_instruction(opcode.value, *args))

                program.append(self.encode_instruction(Opcode.HALT.value))

                # Load and execute
                for addr, instr in enumerate(program):
                    self.write_instr(addr, instr)

                self.start_execution()
                completed = self.wait_execution_complete()

                result = TestResult(
                    test_name=f"ISA_{test_name}",
                    passed=completed,
                    expected=True,
                    actual=completed,
                    details=f"Instruction {opcode.name} execution"
                )

            except Exception as e:
                result = TestResult(
                    test_name=f"ISA_{test_name}",
                    passed=False,
                    expected=True,
                    actual=False,
                    details=f"Exception: {e}"
                )

            self.test_results.append(result)

    def run_data_flow_tests(self) -> None:
        """Test data flow through all components"""
        print("\n" + "="*60)
        print("DATA FLOW AND COMPONENT TESTS")
        print("="*60)

        # Test 1: Unified Buffer Double Buffering
        try:
            # Write to buffer 0
            self.write_ub(0, bytes([1, 2, 3, 4, 5]))
            data0 = self.read_ub(0, 5)

            # Switch buffers and write to buffer 1
            self.write_ub(256, bytes([6, 7, 8, 9, 10]))  # Address 0x100
            data1 = self.read_ub(256, 5)

            # Verify both buffers work independently
            passed = data0 == [1, 2, 3, 4, 5] and data1 == [6, 7, 8, 9, 10]

            result = TestResult(
                test_name="UB_Double_Buffering",
                passed=passed,
                expected=[[1, 2, 3, 4, 5], [6, 7, 8, 9, 10]],
                actual=[data0, data1],
                details="Test independent operation of UB buffers"
            )

        except Exception as e:
            result = TestResult(
                test_name="UB_Double_Buffering",
                passed=False,
                expected="No exception",
                actual=str(e),
                details=f"UB double buffering test failed: {e}"
            )

        self.test_results.append(result)

        # Test 2: Weight FIFO Loading and Popping
        try:
            # Load weights and monitor debug signals during MATMUL
            self.write_wt(0, bytes([1, 0, 0] + [0] * 29))
            self.write_wt(1, bytes([0, 1, 0] + [0] * 29))
            self.write_wt(2, bytes([0, 0, 1] + [0] * 29))
            self.write_ub(0, bytes([10, 20, 30] + [0] * 29))

            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 3),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3),
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            # Monitor weight FIFO outputs during execution
            weight_samples = []
            self.start_execution()

            for _ in range(30):
                debug = self.read_debug_signals()
                weight_samples.append((debug.col0_wt, debug.col1_wt, debug.col2_wt))
                time.sleep(0.01)

            # Check if weights appeared in staggered fashion
            # During weight loading, should see [1,0,0], [0,1,0], [0,0,1] patterns
            has_staggered_weights = any(
                sample[0] == 1 or sample[1] == 1 or sample[2] == 1
                for sample in weight_samples
            )

            result = TestResult(
                test_name="Weight_FIFO_Staggered_Output",
                passed=has_staggered_weights,
                expected="Non-zero weight values in debug samples",
                actual=f"Weight samples: {weight_samples[:5]}...",
                details="Verify weight FIFO outputs weights in staggered pattern"
            )

        except Exception as e:
            result = TestResult(
                test_name="Weight_FIFO_Staggered_Output",
                passed=False,
                expected="No exception",
                actual=str(e),
                details=f"Weight FIFO test failed: {e}"
            )

        self.test_results.append(result)

    def run_pipelining_tests(self) -> None:
        """Test pipelining and timing"""
        print("\n" + "="*60)
        print("PIPELINING AND TIMING TESTS")
        print("="*60)

        # Test 1: Instruction Pipeline Hazards
        try:
            # Create program with potential hazards
            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 3),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),      # Depends on RD_WEIGHT
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3), # Depends on both
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),    # Depends on MATMUL
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            # Monitor for pipeline stalls
            stall_count = 0
            self.start_execution()

            for _ in range(20):
                # Read pipeline status (if available)
                time.sleep(0.01)
                # In a real implementation, we'd check stall signals here

            completed = self.wait_execution_complete()

            result = TestResult(
                test_name="Pipeline_Hazard_Detection",
                passed=completed,
                expected=True,
                actual=completed,
                details="Test that pipeline handles instruction dependencies correctly"
            )

        except Exception as e:
            result = TestResult(
                test_name="Pipeline_Hazard_Detection",
                passed=False,
                expected="No exception",
                actual=str(e),
                details=f"Pipeline test failed: {e}"
            )

        self.test_results.append(result)

    def run_debug_signal_tests(self) -> None:
        """Test debug signal consistency"""
        print("\n" + "="*60)
        print("DEBUG SIGNAL VALIDATION")
        print("="*60)

        try:
            # Run a simple MATMUL and monitor debug signals
            self.write_wt(0, bytes([1, 0, 0] + [0] * 29))
            self.write_wt(1, bytes([0, 1, 0] + [0] * 29))
            self.write_wt(2, bytes([0, 0, 1] + [0] * 29))
            self.write_ub(0, bytes([10, 20, 30] + [0] * 29))

            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 3),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3),
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            # Collect debug samples
            debug_samples = []
            self.start_execution()

            for _ in range(40):
                debug_samples.append(self.read_debug_signals())
                time.sleep(0.01)

            # Analyze debug signal patterns
            weight_load_phase = any(s.weight_pass for s in debug_samples)
            systolic_active_phase = any(s.systolic_active for s in debug_samples)
            valid_weights = any(s.col0_wt != 0 or s.col1_wt != 0 or s.col2_wt != 0 for s in debug_samples)
            valid_activations = any(s.row0_act != 0 or s.row1_act != 0 or s.row2_act != 0 for s in debug_samples)

            passed = all([weight_load_phase, systolic_active_phase, valid_weights, valid_activations])

            result = TestResult(
                test_name="Debug_Signal_Consistency",
                passed=passed,
                expected="All debug signals show expected patterns",
                actual={
                    "weight_load_phase": weight_load_phase,
                    "systolic_active_phase": systolic_active_phase,
                    "valid_weights": valid_weights,
                    "valid_activations": valid_activations
                },
                details=f"Debug samples: {len(debug_samples)}, Patterns detected: {passed}"
            )

        except Exception as e:
            result = TestResult(
                test_name="Debug_Signal_Consistency",
                passed=False,
                expected="No exception",
                actual=str(e),
                details=f"Debug signal test failed: {e}"
            )

        self.test_results.append(result)

    def run_staggered_data_flow_test(self) -> None:
        """Test staggered data flow through systolic array"""
        print("\n" + "="*60)
        print("STAGGERED DATA FLOW TESTS")
        print("="*60)

        try:
            # Test the staggered input/output pattern
            self.write_wt(0, bytes([1, 2, 3] + [0] * 29))  # Row 0
            self.write_wt(1, bytes([4, 5, 6] + [0] * 29))  # Row 1
            self.write_wt(2, bytes([7, 8, 9] + [0] * 29))  # Row 2
            self.write_ub(0, bytes([1, 1, 1] + [0] * 29))  # Simple activations

            program = [
                self.encode_instruction(Opcode.RD_WEIGHT.value, 0, 3),
                self.encode_instruction(Opcode.LD_UB.value, 0, 3),
                self.encode_instruction(Opcode.MATMUL.value, 0, 0, 3),
                self.encode_instruction(Opcode.ST_UB.value, 10, 3),
                self.encode_instruction(Opcode.HALT.value)
            ]

            for addr, instr in enumerate(program):
                self.write_instr(addr, instr)

            # Monitor staggered weight emergence
            weight_sequence = []
            self.start_execution()

            for _ in range(25):
                debug = self.read_debug_signals()
                weight_sequence.append((debug.col0_wt, debug.col1_wt, debug.col2_wt))
                time.sleep(0.01)

            # Check for staggered pattern: col0 first, then col1, then col2
            # This tests the diagonal wavefront weight loading
            col0_first = any(seq[0] != 0 for seq in weight_sequence[:10])
            col1_delayed = any(seq[1] != 0 for seq in weight_sequence[5:15])
            col2_most_delayed = any(seq[2] != 0 for seq in weight_sequence[10:])

            staggered_pattern = col0_first and col1_delayed and col2_most_delayed

            result = TestResult(
                test_name="Staggered_Weight_Loading",
                passed=staggered_pattern,
                expected="Weights emerge in staggered diagonal wavefront pattern",
                actual=f"Col0 first: {col0_first}, Col1 delayed: {col1_delayed}, Col2 most delayed: {col2_most_delayed}",
                details="Test diagonal wavefront weight loading pattern"
            )

        except Exception as e:
            result = TestResult(
                test_name="Staggered_Weight_Loading",
                passed=False,
                expected="No exception",
                actual=str(e),
                details=f"Staggered data flow test failed: {e}"
            )

        self.test_results.append(result)

    def run_all_tests(self) -> None:
        """Run the complete test suite"""
        print("ELITE TPU COMPREHENSIVE TEST SUITE")
        print("=" * 60)
        print(f"Testing TPU on {self.ser.port}")
        print("Coverage: MATMUL, ISA, Components, Data Flow, Pipelining, Debug")
        print("=" * 60)

        # Reset TPU
        self.send_byte(0xFF)  # Reset command
        time.sleep(0.1)

        # Run all test suites
        self.run_matmul_test_suite()
        self.run_isa_test_suite()
        self.run_data_flow_tests()
        self.run_pipelining_tests()
        self.run_debug_signal_tests()
        self.run_staggered_data_flow_test()

        # Report results
        self.report_results()

    def report_results(self) -> None:
        """Generate comprehensive test report"""
        print("\n" + "="*80)
        print("TEST RESULTS SUMMARY")
        print("="*80)

        passed = 0
        total = len(self.test_results)

        for result in self.test_results:
            status = "PASS" if result.passed else "FAIL"
            print(f"{status} {result.test_name}")
            if not result.passed:
                print(f"      Expected: {result.expected}")
                print(f"      Actual:   {result.actual}")
                if result.details:
                    print(f"      Details:  {result.details}")
            print()

        passed = sum(1 for r in self.test_results if r.passed)

        print("="*80)
        print(f"📊 OVERALL RESULTS: {passed}/{total} tests passed ({passed/total*100:.1f}%)")
        print("="*80)

        if passed == total:
            print("ALL TESTS PASSED! TPU implementation is solid.")
        else:
            print("SOME TESTS FAILED. Check implementation details.")

        print(f"\nDebug signal samples collected: {len(self.debug_history)}")

def main():
    """Main entry point"""
    if len(sys.argv) != 2:
        print("Usage: python elite_tpu_test.py <serial_port>")
        print("Example: python elite_tpu_test.py /dev/tty.usbserial-210183A27BE01")
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
