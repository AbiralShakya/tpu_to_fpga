#!/usr/bin/env python3
"""
test_all_instructions.py - Comprehensive Test Script for TPU Coprocessor

This script tests all 20 instructions in the TPU ISA via UART streaming mode.
It validates the complete instruction set on the Basys 3 FPGA.

Usage:
    python3 test_all_instructions.py /dev/tty.usbserial-XXXX

Requirements:
    - pyserial
    - Basys 3 FPGA programmed with TPU bitstream
    - UART connection at 115200 baud
"""

import sys
import time
import struct
import numpy as np
from tpu_coprocessor_driver import TPU_Coprocessor, Opcode, Instruction

# Color codes for terminal output
class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'


def print_test_header(test_name, test_num, total_tests):
    """Print formatted test header"""
    print(f"\n{Colors.HEADER}{'='*80}{Colors.ENDC}")
    print(f"{Colors.BOLD}Test {test_num}/{total_tests}: {test_name}{Colors.ENDC}")
    print(f"{Colors.HEADER}{'='*80}{Colors.ENDC}\n")


def print_success(msg):
    """Print success message"""
    print(f"{Colors.OKGREEN}✓ {msg}{Colors.ENDC}")


def print_failure(msg):
    """Print failure message"""
    print(f"{Colors.FAIL}✗ {msg}{Colors.ENDC}")


def print_info(msg):
    """Print info message"""
    print(f"{Colors.OKCYAN}ℹ {msg}{Colors.ENDC}")


class TPUInstructionTester:
    """Comprehensive TPU instruction test suite"""

    def __init__(self, port):
        """Initialize TPU coprocessor connection"""
        print(f"{Colors.BOLD}Initializing TPU Coprocessor on {port}...{Colors.ENDC}")
        self.tpu = TPU_Coprocessor(port)
        print_success("TPU Coprocessor connected")

        self.tests_passed = 0
        self.tests_failed = 0
        self.total_tests = 20

    def __del__(self):
        """Cleanup on exit"""
        if hasattr(self, 'tpu'):
            self.tpu.close()

    def run_all_tests(self):
        """Run all 20 instruction tests"""
        print(f"\n{Colors.BOLD}{Colors.HEADER}Starting TPU ISA Test Suite{Colors.ENDC}")
        print(f"{Colors.BOLD}Testing all 20 instructions in streaming mode{Colors.ENDC}\n")

        # Control Instructions
        self.test_nop()              # Test 1
        self.test_sync()             # Test 2
        self.test_cfg_reg()          # Test 3

        # Memory Operations
        self.test_rd_host_mem()      # Test 4
        self.test_wr_host_mem()      # Test 5
        self.test_rd_weight()        # Test 6
        self.test_ld_ub()            # Test 7
        self.test_st_ub()            # Test 8

        # Compute Operations
        self.test_matmul()           # Test 9
        self.test_conv2d()           # Test 10
        self.test_matmul_acc()       # Test 11
        self.test_add_bias()         # Test 12
        self.test_batch_norm()       # Test 13

        # Activation Functions
        self.test_relu()             # Test 14
        self.test_relu6()            # Test 15
        self.test_sigmoid()          # Test 16
        self.test_tanh()             # Test 17

        # Pooling Operations
        self.test_maxpool()          # Test 18
        self.test_avgpool()          # Test 19

        # Program Control
        self.test_halt()             # Test 20

        # Print summary
        self.print_summary()

    def test_nop(self):
        """Test 1: NOP - No Operation"""
        print_test_header("NOP - No Operation", 1, self.total_tests)

        try:
            # Enable streaming mode
            self.tpu.enable_stream_mode()
            print_info("Streaming mode enabled")

            # Stream NOP instructions
            for i in range(5):
                instr = Instruction(Opcode.NOP, 0, 0, 0, 0)
                self.tpu.stream_instruction(instr)
                print_info(f"Sent NOP instruction {i+1}/5")

            # Send HALT to complete
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)
            print_info("Sent HALT instruction")

            # Wait for completion
            if self.tpu.wait_done(timeout=2.0):
                print_success("NOP test passed - TPU executed and halted")
                self.tests_passed += 1
            else:
                print_failure("NOP test failed - timeout waiting for completion")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"NOP test failed with exception: {e}")
            self.tests_failed += 1

    def test_sync(self):
        """Test 2: SYNC - Synchronize Operations"""
        print_test_header("SYNC - Synchronize Operations", 2, self.total_tests)

        try:
            self.tpu.enable_stream_mode()

            # Stream SYNC instruction
            # arg1[3:0] = sync_mask (wait for all units: 0xF)
            # arg2:arg3 = timeout (0x1000 = 4096 cycles)
            instr = Instruction(Opcode.SYNC, 0x0F, 0x10, 0x00, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent SYNC instruction (mask=0xF, timeout=0x1000)")

            # HALT
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=2.0):
                print_success("SYNC test passed")
                self.tests_passed += 1
            else:
                print_failure("SYNC test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"SYNC test failed: {e}")
            self.tests_failed += 1

    def test_cfg_reg(self):
        """Test 3: CFG_REG - Configure Register"""
        print_test_header("CFG_REG - Configure Register", 3, self.total_tests)

        try:
            self.tpu.enable_stream_mode()

            # Write configuration register
            # arg1 = register address (0x10)
            # arg2:arg3 = data value (0xABCD)
            instr = Instruction(Opcode.CFG_REG, 0x10, 0xAB, 0xCD, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent CFG_REG instruction (addr=0x10, data=0xABCD)")

            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=2.0):
                print_success("CFG_REG test passed")
                self.tests_passed += 1
            else:
                print_failure("CFG_REG test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"CFG_REG test failed: {e}")
            self.tests_failed += 1

    def test_rd_host_mem(self):
        """Test 4: RD_HOST_MEM - Read from Host Memory (DMA)"""
        print_test_header("RD_HOST_MEM - DMA Read from Host", 4, self.total_tests)

        try:
            # Write test data to unified buffer first
            test_data = bytes([i for i in range(32)])
            self.tpu.write_ub(0x00, test_data)
            print_info(f"Wrote {len(test_data)} bytes to UB addr 0x00")

            # Enable streaming and issue RD_HOST_MEM
            self.tpu.enable_stream_mode()

            # arg1 = UB address (0x10)
            # arg2:arg3 = length (32 bytes = 0x0020)
            # flags = element size (00 = 8-bit)
            instr = Instruction(Opcode.RD_HOST_MEM, 0x10, 0x00, 0x20, 0b00)
            self.tpu.stream_instruction(instr)
            print_info("Sent RD_HOST_MEM instruction (ub_addr=0x10, len=32)")

            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=2.0):
                print_success("RD_HOST_MEM test passed")
                self.tests_passed += 1
            else:
                print_failure("RD_HOST_MEM test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"RD_HOST_MEM test failed: {e}")
            self.tests_failed += 1

    def test_wr_host_mem(self):
        """Test 5: WR_HOST_MEM - Write to Host Memory (DMA)"""
        print_test_header("WR_HOST_MEM - DMA Write to Host", 5, self.total_tests)

        try:
            self.tpu.enable_stream_mode()

            # arg1 = UB address (0x00)
            # arg2:arg3 = length (32 bytes)
            # flags = element size (00 = 8-bit)
            instr = Instruction(Opcode.WR_HOST_MEM, 0x00, 0x00, 0x20, 0b00)
            self.tpu.stream_instruction(instr)
            print_info("Sent WR_HOST_MEM instruction (ub_addr=0x00, len=32)")

            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=2.0):
                print_success("WR_HOST_MEM test passed")
                self.tests_passed += 1
            else:
                print_failure("WR_HOST_MEM test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"WR_HOST_MEM test failed: {e}")
            self.tests_failed += 1

    def test_rd_weight(self):
        """Test 6: RD_WEIGHT - Read Weight Data"""
        print_test_header("RD_WEIGHT - Load Weights to FIFO", 6, self.total_tests)

        try:
            # Write test weights
            test_weights = bytes([i & 0xFF for i in range(64)])  # 8 bytes
            self.tpu.write_weights(0x00, test_weights)
            print_info(f"Wrote {len(test_weights)} bytes of weights")

            self.tpu.enable_stream_mode()

            # arg1 = tile index (0x00)
            # arg2 = number of tiles (0x01)
            # flags[0] = buffer select (0)
            instr = Instruction(Opcode.RD_WEIGHT, 0x00, 0x01, 0x00, 0b00)
            self.tpu.stream_instruction(instr)
            print_info("Sent RD_WEIGHT instruction (tile=0, num_tiles=1)")

            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=2.0):
                print_success("RD_WEIGHT test passed")
                self.tests_passed += 1
            else:
                print_failure("RD_WEIGHT test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"RD_WEIGHT test failed: {e}")
            self.tests_failed += 1

    def test_ld_ub(self):
        """Test 7: LD_UB - Load from Unified Buffer"""
        print_test_header("LD_UB - Load from Unified Buffer", 7, self.total_tests)

        try:
            # Write data to UB
            test_data = bytes([0x11, 0x22, 0x33, 0x44] * 8)
            self.tpu.write_ub(0x00, test_data)
            print_info("Wrote test data to UB")

            self.tpu.enable_stream_mode()

            # arg1 = UB address (0x00)
            # arg2 = count (0x01)
            instr = Instruction(Opcode.LD_UB, 0x00, 0x01, 0x00, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent LD_UB instruction (addr=0x00, count=1)")

            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=2.0):
                print_success("LD_UB test passed")
                self.tests_passed += 1
            else:
                print_failure("LD_UB test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"LD_UB test failed: {e}")
            self.tests_failed += 1

    def test_st_ub(self):
        """Test 8: ST_UB - Store to Unified Buffer"""
        print_test_header("ST_UB - Store to Unified Buffer", 8, self.total_tests)

        try:
            self.tpu.enable_stream_mode()

            # arg1 = UB address (0x10)
            # arg2 = count (0x01)
            instr = Instruction(Opcode.ST_UB, 0x10, 0x01, 0x00, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent ST_UB instruction (addr=0x10, count=1)")

            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=2.0):
                print_success("ST_UB test passed")
                self.tests_passed += 1
            else:
                print_failure("ST_UB test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"ST_UB test failed: {e}")
            self.tests_failed += 1

    def test_matmul(self):
        """Test 9: MATMUL - Matrix Multiplication"""
        print_test_header("MATMUL - Matrix Multiplication (3x3)", 9, self.total_tests)

        try:
            # Prepare test matrices
            # Matrix A: 3x3
            A = np.array([[1, 2, 3],
                          [4, 5, 6],
                          [7, 8, 9]], dtype=np.int8)

            # Matrix B (weights): 3x3
            B = np.array([[1, 0, 0],
                          [0, 1, 0],
                          [0, 0, 1]], dtype=np.int8)  # Identity matrix

            print_info(f"Matrix A (3x3):\n{A}")
            print_info(f"Matrix B (3x3) - Identity:\n{B}")

            # Write matrices to TPU
            self.tpu.write_ub(0x00, A.tobytes())
            self.tpu.write_weights(0x00, B.tobytes())
            print_info("Wrote matrices to TPU memory")

            self.tpu.enable_stream_mode()

            # Load weights
            self.tpu.stream_instruction(Instruction(Opcode.RD_WEIGHT, 0x00, 0x01, 0x00, 0))

            # Load UB data
            self.tpu.stream_instruction(Instruction(Opcode.LD_UB, 0x00, 0x01, 0x00, 0))

            # Execute MATMUL
            # arg1 = UB address (0x00)
            # arg2 = accumulator address (0x00)
            # arg3 = number of rows (0x03)
            # flags[1] = signed (1), flags[0] = transpose (0)
            instr = Instruction(Opcode.MATMUL, 0x00, 0x00, 0x03, 0b10)
            self.tpu.stream_instruction(instr)
            print_info("Sent MATMUL instruction (3x3, signed)")

            # SYNC to wait for completion
            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))

            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=5.0):
                print_success("MATMUL test passed")
                self.tests_passed += 1
            else:
                print_failure("MATMUL test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"MATMUL test failed: {e}")
            self.tests_failed += 1

    def test_conv2d(self):
        """Test 10: CONV2D - 2D Convolution"""
        print_test_header("CONV2D - 2D Convolution", 10, self.total_tests)

        try:
            self.tpu.enable_stream_mode()

            # arg1 = UB address (0x00)
            # arg2 = accumulator address (0x00)
            # arg3 = rows (0x03)
            # flags[1] = signed, flags[0] = transpose
            instr = Instruction(Opcode.CONV2D, 0x00, 0x00, 0x03, 0b10)
            self.tpu.stream_instruction(instr)
            print_info("Sent CONV2D instruction")

            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=5.0):
                print_success("CONV2D test passed")
                self.tests_passed += 1
            else:
                print_failure("CONV2D test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"CONV2D test failed: {e}")
            self.tests_failed += 1

    def test_matmul_acc(self):
        """Test 11: MATMUL_ACC - Accumulate Matrix Multiplication"""
        print_test_header("MATMUL_ACC - Accumulating Matrix Multiply", 11, self.total_tests)

        try:
            self.tpu.enable_stream_mode()

            # arg1 = UB address
            # arg2 = accumulator address
            # arg3 = rows
            # flags: sys_acc_clear = 0 (accumulate, don't clear)
            instr = Instruction(Opcode.MATMUL_ACC, 0x00, 0x00, 0x03, 0b10)
            self.tpu.stream_instruction(instr)
            print_info("Sent MATMUL_ACC instruction (accumulate mode)")

            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=5.0):
                print_success("MATMUL_ACC test passed")
                self.tests_passed += 1
            else:
                print_failure("MATMUL_ACC test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"MATMUL_ACC test failed: {e}")
            self.tests_failed += 1

    def test_add_bias(self):
        """Test 12: ADD_BIAS - Add Bias Vector"""
        print_test_header("ADD_BIAS - Add Bias Vector", 12, self.total_tests)

        try:
            # Write bias vector to UB
            bias = np.array([10, 20, 30], dtype=np.int8)
            self.tpu.write_ub(0x10, bias.tobytes())
            print_info(f"Wrote bias vector: {bias}")

            self.tpu.enable_stream_mode()

            # arg1 = accumulator input address
            # arg2 = UB bias address (0x10)
            # arg3 = length (3)
            instr = Instruction(Opcode.ADD_BIAS, 0x00, 0x10, 0x03, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent ADD_BIAS instruction")

            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=3.0):
                print_success("ADD_BIAS test passed")
                self.tests_passed += 1
            else:
                print_failure("ADD_BIAS test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"ADD_BIAS test failed: {e}")
            self.tests_failed += 1

    def test_batch_norm(self):
        """Test 13: BATCH_NORM - Batch Normalization"""
        print_test_header("BATCH_NORM - Batch Normalization", 13, self.total_tests)

        try:
            self.tpu.enable_stream_mode()

            # arg1 = UB input address
            # arg2 = UB output address
            # arg3 = config register base index
            instr = Instruction(Opcode.BATCH_NORM, 0x00, 0x10, 0x00, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent BATCH_NORM instruction")

            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=3.0):
                print_success("BATCH_NORM test passed")
                self.tests_passed += 1
            else:
                print_failure("BATCH_NORM test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"BATCH_NORM test failed: {e}")
            self.tests_failed += 1

    def test_relu(self):
        """Test 14: RELU - Rectified Linear Unit"""
        print_test_header("RELU - Rectified Linear Unit", 14, self.total_tests)

        try:
            # Write test data with negative values
            test_data = np.array([-5, -2, 0, 3, 7, 10, -8, 15], dtype=np.int8)
            self.tpu.write_ub(0x00, test_data.tobytes())
            print_info(f"Input data: {test_data}")
            print_info(f"Expected output: {np.maximum(0, test_data)}")

            self.tpu.enable_stream_mode()

            # arg1 = accumulator input address
            # arg2 = UB output address
            # arg3 = length
            instr = Instruction(Opcode.RELU, 0x00, 0x10, 0x08, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent RELU instruction")

            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=3.0):
                print_success("RELU test passed")
                self.tests_passed += 1
            else:
                print_failure("RELU test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"RELU test failed: {e}")
            self.tests_failed += 1

    def test_relu6(self):
        """Test 15: RELU6 - ReLU6 Activation"""
        print_test_header("RELU6 - ReLU6 Activation (MobileNet)", 15, self.total_tests)

        try:
            test_data = np.array([-2, 0, 3, 6, 8, 10], dtype=np.int8)
            self.tpu.write_ub(0x00, test_data.tobytes())
            print_info(f"Input data: {test_data}")
            print_info(f"Expected output (clip to [0,6]): {np.clip(test_data, 0, 6)}")

            self.tpu.enable_stream_mode()

            instr = Instruction(Opcode.RELU6, 0x00, 0x10, 0x06, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent RELU6 instruction")

            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=3.0):
                print_success("RELU6 test passed")
                self.tests_passed += 1
            else:
                print_failure("RELU6 test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"RELU6 test failed: {e}")
            self.tests_failed += 1

    def test_sigmoid(self):
        """Test 16: SIGMOID - Sigmoid Activation"""
        print_test_header("SIGMOID - Sigmoid Activation", 16, self.total_tests)

        try:
            self.tpu.enable_stream_mode()

            instr = Instruction(Opcode.SIGMOID, 0x00, 0x10, 0x08, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent SIGMOID instruction")

            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=3.0):
                print_success("SIGMOID test passed")
                self.tests_passed += 1
            else:
                print_failure("SIGMOID test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"SIGMOID test failed: {e}")
            self.tests_failed += 1

    def test_tanh(self):
        """Test 17: TANH - Hyperbolic Tangent"""
        print_test_header("TANH - Hyperbolic Tangent", 17, self.total_tests)

        try:
            self.tpu.enable_stream_mode()

            instr = Instruction(Opcode.TANH, 0x00, 0x10, 0x08, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent TANH instruction")

            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=3.0):
                print_success("TANH test passed")
                self.tests_passed += 1
            else:
                print_failure("TANH test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"TANH test failed: {e}")
            self.tests_failed += 1

    def test_maxpool(self):
        """Test 18: MAXPOOL - Max Pooling"""
        print_test_header("MAXPOOL - Max Pooling", 18, self.total_tests)

        try:
            # 2x2 pooling test data
            test_data = np.array([1, 3, 2, 4], dtype=np.int8)  # Max = 4
            self.tpu.write_ub(0x00, test_data.tobytes())
            print_info(f"Input data (2x2): {test_data}")
            print_info(f"Expected max: 4")

            self.tpu.enable_stream_mode()

            instr = Instruction(Opcode.MAXPOOL, 0x00, 0x10, 0x00, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent MAXPOOL instruction")

            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=3.0):
                print_success("MAXPOOL test passed")
                self.tests_passed += 1
            else:
                print_failure("MAXPOOL test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"MAXPOOL test failed: {e}")
            self.tests_failed += 1

    def test_avgpool(self):
        """Test 19: AVGPOOL - Average Pooling"""
        print_test_header("AVGPOOL - Average Pooling", 19, self.total_tests)

        try:
            # 2x2 pooling test data
            test_data = np.array([2, 4, 6, 8], dtype=np.int8)  # Avg = 5
            self.tpu.write_ub(0x00, test_data.tobytes())
            print_info(f"Input data (2x2): {test_data}")
            print_info(f"Expected average: 5")

            self.tpu.enable_stream_mode()

            instr = Instruction(Opcode.AVGPOOL, 0x00, 0x10, 0x00, 0)
            self.tpu.stream_instruction(instr)
            print_info("Sent AVGPOOL instruction")

            self.tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)

            if self.tpu.wait_done(timeout=3.0):
                print_success("AVGPOOL test passed")
                self.tests_passed += 1
            else:
                print_failure("AVGPOOL test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"AVGPOOL test failed: {e}")
            self.tests_failed += 1

    def test_halt(self):
        """Test 20: HALT - Program Termination"""
        print_test_header("HALT - Program Termination", 20, self.total_tests)

        try:
            self.tpu.enable_stream_mode()

            # Send HALT instruction
            halt_instr = Instruction(Opcode.HALT, 0, 0, 0, 0)
            self.tpu.stream_instruction(halt_instr)
            print_info("Sent HALT instruction")

            # Wait for completion
            if self.tpu.wait_done(timeout=2.0):
                print_success("HALT test passed - TPU halted correctly")
                self.tests_passed += 1
            else:
                print_failure("HALT test failed - timeout")
                self.tests_failed += 1

        except Exception as e:
            print_failure(f"HALT test failed: {e}")
            self.tests_failed += 1

    def print_summary(self):
        """Print test summary"""
        print(f"\n{Colors.HEADER}{'='*80}{Colors.ENDC}")
        print(f"{Colors.BOLD}Test Summary{Colors.ENDC}")
        print(f"{Colors.HEADER}{'='*80}{Colors.ENDC}\n")

        print(f"Total Tests:   {self.total_tests}")
        print(f"{Colors.OKGREEN}Tests Passed:  {self.tests_passed}{Colors.ENDC}")
        print(f"{Colors.FAIL}Tests Failed:  {self.tests_failed}{Colors.ENDC}")

        pass_rate = (self.tests_passed / self.total_tests) * 100
        print(f"\nPass Rate:     {pass_rate:.1f}%")

        # Print TPU statistics
        print(f"\n{Colors.BOLD}TPU Statistics:{Colors.ENDC}")
        stats = self.tpu.get_statistics()
        print(f"Bytes Sent:       {stats['bytes_sent']}")
        print(f"Bytes Received:   {stats['bytes_received']}")
        print(f"Instructions:     {stats['instructions_sent']}")
        print(f"Errors:           {stats['errors']}")

        if self.tests_passed == self.total_tests:
            print(f"\n{Colors.OKGREEN}{Colors.BOLD}✓ ALL TESTS PASSED!{Colors.ENDC}")
        else:
            print(f"\n{Colors.FAIL}{Colors.BOLD}✗ SOME TESTS FAILED{Colors.ENDC}")


def main():
    """Main entry point"""
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <serial_port>")
        print(f"Example: {sys.argv[0]} /dev/tty.usbserial-210292B2C3A3")
        sys.exit(1)

    port = sys.argv[1]

    try:
        tester = TPUInstructionTester(port)
        tester.run_all_tests()
    except KeyboardInterrupt:
        print(f"\n{Colors.WARNING}Test interrupted by user{Colors.ENDC}")
        sys.exit(1)
    except Exception as e:
        print(f"\n{Colors.FAIL}Fatal error: {e}{Colors.ENDC}")
        sys.exit(1)


if __name__ == "__main__":
    main()
