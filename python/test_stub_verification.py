#!/usr/bin/env python3
"""
ST_UB Verification Test

This test verifies that ST_UB actually reads from accumulators and writes
that data to the Unified Buffer, not just writing zeros.

Test strategy:
1. Write known non-zero values directly to accumulator memory (if possible)
   OR run a MATMUL that should produce non-zero results
2. Use ST_UB to write accumulator data to UB
3. Read back from UB and verify the data matches accumulator values
"""

import sys
import time
import serial
import numpy as np

# UART Protocol Constants
class Cmd:
    WRITE_UB = 0x01
    WRITE_WT = 0x02
    WRITE_INSTR = 0x03
    READ_UB = 0x04
    EXECUTE = 0x05
    READ_STATUS = 0x06

ACK_UB = 0xAA
ACK_WT = 0xBB

class TPUTest:
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
        self.ser.write(bytes([Cmd.WRITE_INSTR, 0, addr]))
        self.ser.write(instr.to_bytes(4, 'big'))
        time.sleep(0.05)
        return True

    def encode_instruction(self, opcode, arg1=0, arg2=0, arg3=0, flags=0):
        return ((opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags)

    def execute(self, timeout=2.0):
        self.ser.reset_input_buffer()
        self.ser.write(bytes([Cmd.EXECUTE]))
        time.sleep(0.05)
        start = time.time()
        while time.time() - start < timeout:
            self.send_cmd(Cmd.READ_STATUS, 0, 0, 0, 0)
            time.sleep(0.02)
            status = self.ser.read(1)
            if status:
                status_byte = status[0]
                # Check if all units are idle (bits 0,2,4 are busy flags)
                if not (status_byte & 0x15):  # 0x15 = 0b00010101 (sys, vpu, ub busy)
                    return True
            time.sleep(0.01)
        return False

def test_stub_with_known_accumulator_values(tpu):
    """
    Test ST_UB by first running a MATMUL that should produce known results,
    then using ST_UB to write those results to UB.
    
    Strategy:
    1. Load identity matrix weights (should pass inputs through)
    2. Load known input values [10, 20, 30]
    3. Run MATMUL (should produce [10, 20, 30] in accumulators)
    4. Use ST_UB to write accumulators to UB
    5. Read back and verify we get [10, 20, 30]
    """
    print("="*60)
    print("ST_UB Verification Test")
    print("="*60)
    print("\nThis test verifies ST_UB actually reads from accumulators")
    print("by running a MATMUL that should produce non-zero results.\n")

    # Step 1: Load identity matrix weights
    print("Step 1: Loading identity matrix weights...")
    weights = [
        bytes([0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),  # [1, 0, 0]
        bytes([0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),  # [0, 1, 0]
        bytes([0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00])   # [0, 0, 1]
    ]
    for i, weight in enumerate(weights):
        if not tpu.write_weight(i, weight):
            print(f"  ✗ Failed to write weight {i}")
            return False
        print(f"  ✓ Weight row {i}: {weight[:3].hex()}")

    # Step 2: Load input activations
    print("\nStep 2: Loading input activations [10, 20, 30]...")
    inputs = bytes([0x0A, 0x14, 0x1E]) + bytes(29)  # [10, 20, 30] + padding
    if not tpu.write_ub(0, inputs):
        print("  ✗ Failed to write inputs")
        return False
    print(f"  ✓ Inputs written: {inputs[:3].hex()}")

    # Step 3: Create program: RD_WEIGHT x3, LD_UB, MATMUL, ST_UB, HALT
    print("\nStep 3: Creating MATMUL program...")
    program = [
        tpu.encode_instruction(0x03, 0, 1, 0, 0),  # RD_WEIGHT(0, 1, 0)
        tpu.encode_instruction(0x03, 1, 1, 0, 0),  # RD_WEIGHT(1, 1, 0)
        tpu.encode_instruction(0x03, 2, 1, 0, 0),  # RD_WEIGHT(2, 1, 0)
        tpu.encode_instruction(0x04, 0, 1, 0, 0),  # LD_UB(0, 1, 0)
        tpu.encode_instruction(0x10, 0, 0, 3, 2),  # MATMUL(0, 0, 3) flags=2 (signed)
        tpu.encode_instruction(0x05, 50, 1, 0, 0), # ST_UB(50, 1, 0) - write to UB[50]
        tpu.encode_instruction(0x3F)                # HALT
    ]
    
    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)
        opcode = (instr >> 26) & 0x3F
        print(f"  [{i}] Op:0x{opcode:02X}")

    # Step 4: Execute
    print("\nStep 4: Executing program...")
    if not tpu.execute():
        print("  ✗ Execution failed or timed out")
        return False
    print("  ✓ Execution completed")

    # Step 5: Read back from UB[50] (where ST_UB wrote)
    print("\nStep 5: Reading accumulator data from UB[50]...")
    result = tpu.read_ub(50, 32)
    print(f"  Raw bytes: {result.hex()}")
    print(f"  First 8 bytes: {result[:8].hex()}")

    # Extract lower 8 bits of each accumulator (bytes 0, 1, 2)
    acc0 = result[0] if result[0] < 128 else result[0] - 256
    acc1 = result[1] if result[1] < 128 else result[1] - 256
    acc2 = result[2] if result[2] < 128 else result[2] - 256
    
    print(f"  Interpreted as int8: [{acc0}, {acc1}, {acc2}]")
    print(f"  Expected: [10, 20, 30]")

    # Step 6: Verify
    expected = [10, 20, 30]
    actual = [acc0, acc1, acc2]
    
    print("\n" + "="*60)
    if actual == expected:
        print("✓ PASS: ST_UB correctly wrote accumulator data to UB!")
        print(f"  Accumulator values [{acc0}, {acc1}, {acc2}] match expected [{expected[0]}, {expected[1]}, {expected[2]}]")
        return True
    elif actual == [0, 0, 0]:
        print("✗ FAIL: ST_UB wrote zeros - accumulators may be zero or ST_UB not reading from accumulators")
        print("  This could mean:")
        print("    1. MATMUL didn't run (accumulators never got written)")
        print("    2. Accumulators were cleared before ST_UB")
        print("    3. ST_UB is not reading from accumulator latches")
        return False
    else:
        print(f"✗ FAIL: ST_UB wrote wrong values")
        print(f"  Expected: {expected}")
        print(f"  Got: {actual}")
        print("  This suggests MATMUL produced wrong results or ST_UB read wrong data")
        return False

def test_stub_bank_verification(tpu):
    """
    Test that ST_UB writes to the correct bank that UART can read.
    
    Strategy:
    1. Write different patterns to UB[60] in both possible banks
    2. Run ST_UB to write to UB[60]
    3. Read back and verify we get accumulator data, not the original patterns
    """
    print("\n" + "="*60)
    print("ST_UB Bank Selection Verification")
    print("="*60)
    
    # First, ensure we have non-zero accumulator data
    print("\nPreparing non-zero accumulator data...")
    # Run a simple MATMUL first to populate accumulators
    weights = [
        bytes([0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),  # [2, 0, 0]
        bytes([0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),  # [0, 2, 0]
        bytes([0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00])   # [0, 0, 2]
    ]
    for i, weight in enumerate(weights):
        tpu.write_weight(i, weight)
    
    inputs = bytes([0x05, 0x0A, 0x0F]) + bytes(29)  # [5, 10, 15]
    tpu.write_ub(0, inputs)
    
    # MATMUL program
    program = [
        tpu.encode_instruction(0x03, 0, 1, 0, 0),  # RD_WEIGHT(0, 1, 0)
        tpu.encode_instruction(0x03, 1, 1, 0, 0),  # RD_WEIGHT(1, 1, 0)
        tpu.encode_instruction(0x03, 2, 1, 0, 0),  # RD_WEIGHT(2, 1, 0)
        tpu.encode_instruction(0x04, 0, 1, 0, 0),  # LD_UB(0, 1, 0)
        tpu.encode_instruction(0x10, 0, 0, 3, 2),  # MATMUL(0, 0, 3)
        tpu.encode_instruction(0x3F)                # HALT (don't ST_UB yet)
    ]
    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)
    tpu.execute()
    
    # Now write markers to UB[60] - these should be overwritten by ST_UB
    print("\nWriting marker patterns to UB[60]...")
    marker1 = bytes([0xAA]*32)
    marker2 = bytes([0xBB]*32)
    tpu.write_ub(60, marker1)
    time.sleep(0.1)
    tpu.write_ub(60, marker2)  # Overwrite with different pattern
    print(f"  Marker written: {marker2[:8].hex()}")
    
    # Read back to verify marker is there
    before = tpu.read_ub(60)
    print(f"  UB[60] before ST_UB: {before[:8].hex()}")
    
    if before[:8] != marker2[:8]:
        print("  ⚠ WARNING: Marker write/read mismatch - bank selection may be wrong")
    
    # Now run ST_UB to address 60
    print("\nExecuting ST_UB to address 60...")
    ST_UB = tpu.encode_instruction(0x05, 60, 1, 0, 0)  # ST_UB(60, 1, 0)
    HALT = tpu.encode_instruction(0x3F)
    tpu.write_instruction(0, ST_UB)
    tpu.write_instruction(1, HALT)
    tpu.execute()
    
    # Read back
    after = tpu.read_ub(60)
    print(f"  UB[60] after ST_UB: {after[:8].hex()}")
    
    # Verify
    acc0 = after[0] if after[0] < 128 else after[0] - 256
    acc1 = after[1] if after[1] < 128 else after[1] - 256
    acc2 = after[2] if after[2] < 128 else after[2] - 256
    
    print(f"  Interpreted: [{acc0}, {acc1}, {acc2}]")
    print(f"  Expected from MATMUL: [10, 20, 30] (2*[5,10,15])")
    
    if after[:8] == marker2[:8]:
        print("\n  ✗ FAIL: ST_UB did NOT modify UB[60]")
        print("  → Bank mismatch: ST_UB wrote to different bank than UART reads")
        return False
    elif after[:3] == bytes([0, 0, 0]):
        print("\n  ✗ FAIL: ST_UB wrote zeros")
        print("  → Accumulators may be zero or ST_UB not reading from accumulators")
        return False
    else:
        print("\n  ✓ PASS: ST_UB modified UB[60] with accumulator data")
        print(f"  → Bank selection is correct (UART can read ST_UB output)")
        return True

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 test_stub_verification.py <UART_PORT>")
        sys.exit(1)
    
    port = sys.argv[1]
    tpu = TPUTest(port)
    
    print("="*60)
    print("ST_UB VERIFICATION TESTS")
    print("="*60)
    print("\nThese tests verify ST_UB actually reads from accumulators")
    print("and writes to the correct Unified Buffer bank.\n")
    
    test1_pass = test_stub_with_known_accumulator_values(tpu)
    test2_pass = test_stub_bank_verification(tpu)
    
    print("\n" + "="*60)
    print("SUMMARY")
    print("="*60)
    print(f"Test 1 (ST_UB reads accumulators): {'PASS' if test1_pass else 'FAIL'}")
    print(f"Test 2 (ST_UB bank selection): {'PASS' if test2_pass else 'FAIL'}")
    
    if test1_pass and test2_pass:
        print("\n✓ All ST_UB tests PASSED!")
    else:
        print("\n✗ Some ST_UB tests FAILED")
    
    tpu.ser.close()

if __name__ == "__main__":
    main()
