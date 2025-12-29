#!/usr/bin/env python3
"""
Comprehensive MATMUL Verification Test

Tests matrix multiplication with known weights and inputs to verify:
1. Correct computation (3x3 systolic array)
2. Accumulator latching during systolic_active
3. ST_UB reading from acc_data_out correctly

Expected behavior:
- Input activations: [a0, a1, a2] (loaded from UB)
- Weight matrix (3x3):
    w00 w01 w02
    w10 w11 w12
    w20 w21 w22
- Output: [o0, o1, o2] where:
    o0 = a0*w00 + a1*w10 + a2*w20
    o1 = a0*w01 + a1*w11 + a2*w21
    o2 = a0*w02 + a1*w12 + a2*w22
"""

import sys
import time
import serial

class Cmd:
    WRITE_UB = 0x01
    WRITE_WT = 0x02
    WRITE_INSTR = 0x03
    READ_UB = 0x04
    EXECUTE = 0x05
    READ_STATUS = 0x06

ACK_UB = 0xAA
ACK_WT = 0xBB

class TPU:
    def __init__(self, port):
        self.ser = serial.Serial(port, 115200, timeout=2)
        time.sleep(0.1)
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

    def send_cmd(self, cmd, addr_hi, addr_lo, len_hi, len_lo):
        self.ser.write(bytes([cmd, addr_hi, addr_lo, len_hi, len_lo]))

    def write_ub(self, addr, data):
        data = data.ljust(32, b'\x00')[:32]
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.WRITE_UB, 0, addr, 0, 32)
        self.ser.write(data)
        time.sleep(0.05)
        ack = self.ser.read(1)
        return ack == bytes([ACK_UB])

    def read_ub(self, addr, length=32):
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.READ_UB, 0, addr, 0, length)
        time.sleep(0.05)
        return self.ser.read(length)

    def write_weight(self, addr, data):
        data = data.ljust(8, b'\x00')[:8]
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.WRITE_WT, 0, addr, 0, 8)
        self.ser.write(data)
        time.sleep(0.05)
        ack = self.ser.read(1)
        return ack == bytes([ACK_WT])

    def write_instruction(self, addr, instr):
        self.ser.reset_input_buffer()
        # CRITICAL: Only 3 header bytes for WRITE_INSTR
        self.ser.write(bytes([Cmd.WRITE_INSTR, 0, addr]))
        self.ser.write(instr.to_bytes(4, 'big'))
        time.sleep(0.05)
        return True

    def execute(self, timeout=2.0):
        self.ser.reset_input_buffer()
        self.ser.write(bytes([Cmd.EXECUTE]))
        time.sleep(0.05)
        start = time.time()
        while time.time() - start < timeout:
            self.ser.reset_input_buffer()
            self.ser.write(bytes([Cmd.READ_STATUS]))
            time.sleep(0.005)
            status = self.ser.read(1)
            if status and (status[0] & 0x01) == 0:
                return True
            time.sleep(0.01)
        return False

    def encode_instruction(self, opcode, arg1=0, arg2=0, arg3=0, flags=0):
        return ((opcode & 0x3F) << 26) | ((arg1 & 0xFF) << 18) | ((arg2 & 0xFF) << 10) | ((arg3 & 0xFF) << 2) | (flags & 0x03)


def test_identity_matrix(tpu):
    """Test 1: Identity matrix multiplication (simplest case)"""
    print("\n" + "="*70)
    print("TEST 1: Identity Matrix Multiplication")
    print("="*70)
    print("Input: [10, 20, 30]")
    print("Weights: 3x3 identity matrix")
    print("Expected output: [10, 20, 30]")
    print()

    # Load identity matrix weights (1 on diagonal, 0 elsewhere)
    # Row 0: [1, 0, 0] (position 0,1,2 in weight memory)
    # Row 1: [0, 1, 0] (position 3,4,5)
    # Row 2: [0, 0, 1] (position 6,7,8)

    # Each weight memory address holds a row of 8 int8 values
    # For 3x3 matrix, we use positions 0-2 of each row
    tpu.write_weight(0, bytes([1, 0, 0, 0, 0, 0, 0, 0]))  # Row 0
    tpu.write_weight(1, bytes([0, 1, 0, 0, 0, 0, 0, 0]))  # Row 1
    tpu.write_weight(2, bytes([0, 0, 1, 0, 0, 0, 0, 0]))  # Row 2

    print("  ✓ Loaded identity weights")

    # Load input activations [10, 20, 30] to UB[0]
    tpu.write_ub(0, bytes([10, 20, 30] + [0]*29))
    print("  ✓ Loaded inputs: [10, 20, 30]")

    # Create program: RD_WEIGHT x3 → LD_UB → MATMUL → ST_UB → HALT
    program = [
        tpu.encode_instruction(0x03, arg1=0),  # RD_WEIGHT from addr 0
        tpu.encode_instruction(0x03, arg1=1),  # RD_WEIGHT from addr 1
        tpu.encode_instruction(0x03, arg1=2),  # RD_WEIGHT from addr 2
        tpu.encode_instruction(0x04, arg1=0, arg2=3),  # LD_UB from addr 0, load 3 bytes
        tpu.encode_instruction(0x10, arg1=0, arg2=0, arg3=3, flags=2),  # MATMUL (signed)
        tpu.encode_instruction(0x05, arg1=10, arg2=1),  # ST_UB to addr 10
        tpu.encode_instruction(0x3F),  # HALT
    ]

    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)
    print("  ✓ Loaded program (7 instructions)")

    # Execute
    print("  Executing...", end=" ")
    success = tpu.execute()
    print("✓" if success else "✗ TIMEOUT")

    # Read result from UB[10]
    result = tpu.read_ub(10)
    output = list(result[:3])
    print(f"\n  Result: {output}")
    print(f"  Expected: [10, 20, 30]")

    # Check
    if output == [10, 20, 30]:
        print("\n  ✓✓✓ PASS: Identity matrix multiplication correct!")
        return True
    else:
        print(f"\n  ✗✗✗ FAIL: Got {output}, expected [10, 20, 30]")
        return False


def test_simple_matmul(tpu):
    """Test 2: Simple known matrix multiplication"""
    print("\n" + "="*70)
    print("TEST 2: Simple Matrix Multiplication")
    print("="*70)
    print("Input: [1, 2, 3]")
    print("Weights:")
    print("  [1, 0, 0]")
    print("  [0, 2, 0]")
    print("  [0, 0, 3]")
    print("Expected output: [1, 4, 9]")
    print()

    # Load diagonal matrix with scaling: diag(1, 2, 3)
    tpu.write_weight(0, bytes([1, 0, 0, 0, 0, 0, 0, 0]))  # Row 0: [1, 0, 0]
    tpu.write_weight(1, bytes([0, 2, 0, 0, 0, 0, 0, 0]))  # Row 1: [0, 2, 0]
    tpu.write_weight(2, bytes([0, 0, 3, 0, 0, 0, 0, 0]))  # Row 2: [0, 0, 3]
    print("  ✓ Loaded diagonal weights: diag(1, 2, 3)")

    # Load inputs
    tpu.write_ub(0, bytes([1, 2, 3] + [0]*29))
    print("  ✓ Loaded inputs: [1, 2, 3]")

    # Program
    program = [
        tpu.encode_instruction(0x03, arg1=0),
        tpu.encode_instruction(0x03, arg1=1),
        tpu.encode_instruction(0x03, arg1=2),
        tpu.encode_instruction(0x04, arg1=0, arg2=3),
        tpu.encode_instruction(0x10, arg1=0, arg2=0, arg3=3, flags=2),
        tpu.encode_instruction(0x05, arg1=11, arg2=1),
        tpu.encode_instruction(0x3F),
    ]

    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)
    print("  ✓ Loaded program")

    print("  Executing...", end=" ")
    success = tpu.execute()
    print("✓" if success else "✗ TIMEOUT")

    result = tpu.read_ub(11)
    output = list(result[:3])
    print(f"\n  Result: {output}")
    print(f"  Expected: [1, 4, 9]")

    if output == [1, 4, 9]:
        print("\n  ✓✓✓ PASS: Diagonal scaling correct!")
        return True
    else:
        print(f"\n  ✗✗✗ FAIL: Got {output}, expected [1, 4, 9]")
        return False


def test_full_matmul(tpu):
    """Test 3: Full 3x3 matrix multiplication"""
    print("\n" + "="*70)
    print("TEST 3: Full 3x3 Matrix Multiplication")
    print("="*70)
    print("Input: [1, 2, 3]")
    print("Weights:")
    print("  [1, 2, 3]")
    print("  [4, 5, 6]")
    print("  [7, 8, 9]")
    print()
    print("Expected output:")
    print("  o0 = 1*1 + 2*4 + 3*7 = 1 + 8 + 21 = 30")
    print("  o1 = 1*2 + 2*5 + 3*8 = 2 + 10 + 24 = 36")
    print("  o2 = 1*3 + 2*6 + 3*9 = 3 + 12 + 27 = 42")
    print("  Expected: [30, 36, 42]")
    print()

    # Load full matrix
    tpu.write_weight(0, bytes([1, 2, 3, 0, 0, 0, 0, 0]))
    tpu.write_weight(1, bytes([4, 5, 6, 0, 0, 0, 0, 0]))
    tpu.write_weight(2, bytes([7, 8, 9, 0, 0, 0, 0, 0]))
    print("  ✓ Loaded weight matrix")

    # Load inputs
    tpu.write_ub(0, bytes([1, 2, 3] + [0]*29))
    print("  ✓ Loaded inputs: [1, 2, 3]")

    # Program
    program = [
        tpu.encode_instruction(0x03, arg1=0),
        tpu.encode_instruction(0x03, arg1=1),
        tpu.encode_instruction(0x03, arg1=2),
        tpu.encode_instruction(0x04, arg1=0, arg2=3),
        tpu.encode_instruction(0x10, arg1=0, arg2=0, arg3=3, flags=2),
        tpu.encode_instruction(0x05, arg1=12, arg2=1),
        tpu.encode_instruction(0x3F),
    ]

    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)
    print("  ✓ Loaded program")

    print("  Executing...", end=" ")
    success = tpu.execute()
    print("✓" if success else "✗ TIMEOUT")

    result = tpu.read_ub(12)
    output = list(result[:3])

    # Print as both unsigned and signed
    print(f"\n  Result (unsigned): {output}")
    output_signed = [x if x < 128 else x - 256 for x in output]
    print(f"  Result (signed):   {output_signed}")
    print(f"  Expected: [30, 36, 42]")

    if output == [30, 36, 42]:
        print("\n  ✓✓✓ PASS: Full matrix multiplication correct!")
        return True
    else:
        print(f"\n  ✗✗✗ FAIL: Got {output}, expected [30, 36, 42]")
        print("\n  Debugging info:")
        print(f"    Raw bytes: {result[:8].hex()}")
        return False


def test_zero_weights(tpu):
    """Test 4: Zero weights (should produce zero output)"""
    print("\n" + "="*70)
    print("TEST 4: Zero Weights")
    print("="*70)
    print("Input: [10, 20, 30]")
    print("Weights: all zeros")
    print("Expected output: [0, 0, 0]")
    print()

    # Load zero weights
    tpu.write_weight(0, bytes([0]*8))
    tpu.write_weight(1, bytes([0]*8))
    tpu.write_weight(2, bytes([0]*8))
    print("  ✓ Loaded zero weights")

    # Load inputs
    tpu.write_ub(0, bytes([10, 20, 30] + [0]*29))
    print("  ✓ Loaded inputs: [10, 20, 30]")

    # Program
    program = [
        tpu.encode_instruction(0x03, arg1=0),
        tpu.encode_instruction(0x03, arg1=1),
        tpu.encode_instruction(0x03, arg1=2),
        tpu.encode_instruction(0x04, arg1=0, arg2=3),
        tpu.encode_instruction(0x10, arg1=0, arg2=0, arg3=3, flags=2),
        tpu.encode_instruction(0x05, arg1=13, arg2=1),
        tpu.encode_instruction(0x3F),
    ]

    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)
    print("  ✓ Loaded program")

    print("  Executing...", end=" ")
    success = tpu.execute()
    print("✓" if success else "✗ TIMEOUT")

    result = tpu.read_ub(13)
    output = list(result[:3])
    print(f"\n  Result: {output}")
    print(f"  Expected: [0, 0, 0]")

    if output == [0, 0, 0]:
        print("\n  ✓✓✓ PASS: Zero weights produce zero output!")
        return True
    else:
        print(f"\n  ✗✗✗ FAIL: Got {output}, expected [0, 0, 0]")
        return False


def main(port):
    tpu = TPU(port)

    print("\n" + "="*70)
    print("COMPREHENSIVE MATMUL VERIFICATION TEST SUITE")
    print("="*70)
    print(f"Connected to: {port}")
    print()
    print("This suite tests matrix multiplication with known inputs and weights")
    print("to verify the TPU systolic array is computing correctly.")
    print()

    results = []

    # Run all tests
    results.append(("Identity Matrix", test_identity_matrix(tpu)))
    results.append(("Simple Diagonal", test_simple_matmul(tpu)))
    results.append(("Full 3x3 Matrix", test_full_matmul(tpu)))
    results.append(("Zero Weights", test_zero_weights(tpu)))

    # Summary
    print("\n" + "="*70)
    print("TEST SUMMARY")
    print("="*70)

    for name, passed in results:
        status = "✓ PASS" if passed else "✗ FAIL"
        print(f"  {name:25s}: {status}")

    all_pass = all(passed for _, passed in results)

    print("\n" + "="*70)
    if all_pass:
        print("✓✓✓ ALL TESTS PASSED! MATMUL is working correctly!")
    else:
        print("✗✗✗ SOME TESTS FAILED - MATMUL computation has issues")
    print("="*70)

    return all_pass


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <serial_port>")
        print(f"Example: {sys.argv[0]} /dev/tty.usbserial-210183A27BE01")
        sys.exit(1)

    main(sys.argv[1])
