#!/usr/bin/env python3
"""
Investigate TEST 7 Zero Accumulator Values

This script reproduces TEST 7 from diagnostic_isolated.py with more instrumentation
to understand why acc_data_out is always zero when it should be [10, 20, 30].

We'll trace through each instruction step-by-step and read acc_data_out after each one.
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
        # CRITICAL: Only 3 header bytes
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


def main(port):
    tpu = TPU(port)

    print("\n" + "="*70)
    print("INVESTIGATING TEST 7 ZERO ACCUMULATORS")
    print("="*70)
    print()
    print("Reproducing TEST 7 from diagnostic_isolated.py with step-by-step tracing")
    print()

    # Marker address for reading acc_data_out
    ACC_READOUT_ADDR = 50

    # Step 1: Load identity matrix weights
    print("Step 1: Loading identity matrix weights...")
    tpu.write_weight(0, bytes([1, 0, 0, 0, 0, 0, 0, 0]))
    tpu.write_weight(1, bytes([0, 1, 0, 0, 0, 0, 0, 0]))
    tpu.write_weight(2, bytes([0, 0, 1, 0, 0, 0, 0, 0]))
    print("  ✓ Identity weights loaded")

    # Step 2: Load input activations [10, 20, 30]
    print("\nStep 2: Loading input activations [10, 20, 30]...")
    tpu.write_ub(0, bytes([10, 20, 30] + [0]*29))
    verify = tpu.read_ub(0)
    print(f"  ✓ Inputs written: {verify[:3].hex()}")

    # Step 3: Test acc_data_out after RD_WEIGHT only
    print("\nStep 3: Testing acc_data_out after RD_WEIGHT x3 only...")
    program = [
        tpu.encode_instruction(0x03, arg1=0),  # RD_WEIGHT 0
        tpu.encode_instruction(0x03, arg1=1),  # RD_WEIGHT 1
        tpu.encode_instruction(0x03, arg1=2),  # RD_WEIGHT 2
        tpu.encode_instruction(0x05, arg1=ACC_READOUT_ADDR, arg2=1),  # ST_UB to read acc
        tpu.encode_instruction(0x3F),  # HALT
    ]
    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)

    tpu.execute()
    result1 = tpu.read_ub(ACC_READOUT_ADDR)
    print(f"  acc_data_out after RD_WEIGHT x3: {result1[:8].hex()} → {list(result1[:3])}")

    # Step 4: Test acc_data_out after RD_WEIGHT + LD_UB
    print("\nStep 4: Testing acc_data_out after RD_WEIGHT x3 + LD_UB...")
    program = [
        tpu.encode_instruction(0x03, arg1=0),
        tpu.encode_instruction(0x03, arg1=1),
        tpu.encode_instruction(0x03, arg1=2),
        tpu.encode_instruction(0x04, arg1=0, arg2=3),  # LD_UB addr=0, count=3
        tpu.encode_instruction(0x05, arg1=ACC_READOUT_ADDR+1, arg2=1),
        tpu.encode_instruction(0x3F),
    ]
    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)

    tpu.execute()
    result2 = tpu.read_ub(ACC_READOUT_ADDR+1)
    print(f"  acc_data_out after LD_UB: {result2[:8].hex()} → {list(result2[:3])}")

    # Step 5: Full program with MATMUL
    print("\nStep 5: Testing acc_data_out after full MATMUL...")
    program = [
        tpu.encode_instruction(0x03, arg1=0),
        tpu.encode_instruction(0x03, arg1=1),
        tpu.encode_instruction(0x03, arg1=2),
        tpu.encode_instruction(0x04, arg1=0, arg2=3),
        tpu.encode_instruction(0x10, arg1=0, arg2=0, arg3=3, flags=2),  # MATMUL signed
        tpu.encode_instruction(0x05, arg1=ACC_READOUT_ADDR+2, arg2=1),
        tpu.encode_instruction(0x3F),
    ]
    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)

    print("  Executing MATMUL program...", end=" ")
    success = tpu.execute()
    print("✓" if success else "✗ TIMEOUT")

    result3 = tpu.read_ub(ACC_READOUT_ADDR+2)
    print(f"  acc_data_out after MATMUL: {result3[:8].hex()} → {list(result3[:3])}")

    # Analysis
    print("\n" + "="*70)
    print("ANALYSIS")
    print("="*70)

    print(f"\n1. After RD_WEIGHT x3: {list(result1[:3])}")
    print("   Expected: [0, 0, 0] (no computation yet)")

    print(f"\n2. After LD_UB: {list(result2[:3])}")
    print("   Expected: [0, 0, 0] (no computation yet)")

    print(f"\n3. After MATMUL: {list(result3[:3])}")
    print("   Expected: [10, 20, 30] (identity matrix * [10,20,30])")

    if list(result3[:3]) == [10, 20, 30]:
        print("\n✓✓✓ SUCCESS: MATMUL produces correct result!")
    elif list(result3[:3]) == [0, 0, 0]:
        print("\n✗✗✗ PROBLEM: Accumulators are still zero after MATMUL!")
        print("\nPossible causes:")
        print("  1. MATMUL instruction not executing")
        print("  2. Systolic array not computing")
        print("  3. Accumulator clearing happening incorrectly")
        print("  4. systolic_active signal not triggering latching")
    else:
        print(f"\n✗ UNEXPECTED: Got {list(result3[:3])}")
        print("  Result is non-zero but incorrect")

    # Additional test: Try MATMUL without clearing accumulators first
    print("\n" + "="*70)
    print("ADDITIONAL TEST: MATMUL_ACC (accumulate mode)")
    print("="*70)

    # First run a MATMUL to populate accumulators
    print("\nRun 1: Initial MATMUL...")
    program = [
        tpu.encode_instruction(0x03, arg1=0),
        tpu.encode_instruction(0x03, arg1=1),
        tpu.encode_instruction(0x03, arg1=2),
        tpu.encode_instruction(0x04, arg1=0, arg2=3),
        tpu.encode_instruction(0x10, arg1=0, arg2=0, arg3=3, flags=2),
        tpu.encode_instruction(0x05, arg1=60, arg2=1),
        tpu.encode_instruction(0x3F),
    ]
    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)
    tpu.execute()

    result_run1 = tpu.read_ub(60)
    print(f"  Result after run 1: {list(result_run1[:3])}")

    # Second run: MATMUL_ACC (should add to existing)
    print("\nRun 2: MATMUL_ACC (accumulate)...")
    program = [
        tpu.encode_instruction(0x03, arg1=0),
        tpu.encode_instruction(0x03, arg1=1),
        tpu.encode_instruction(0x03, arg1=2),
        tpu.encode_instruction(0x04, arg1=0, arg2=3),
        tpu.encode_instruction(0x12, arg1=0, arg2=0, arg3=3, flags=2),  # MATMUL_ACC
        tpu.encode_instruction(0x05, arg1=61, arg2=1),
        tpu.encode_instruction(0x3F),
    ]
    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)
    tpu.execute()

    result_run2 = tpu.read_ub(61)
    print(f"  Result after run 2: {list(result_run2[:3])}")
    print(f"  Expected (2x): {[20, 40, 60]}")

    if list(result_run2[:3]) == [20, 40, 60]:
        print("\n  ✓ MATMUL_ACC is working (accumulation successful)")
    else:
        print(f"\n  ✗ MATMUL_ACC issue: got {list(result_run2[:3])}, expected [20, 40, 60]")

    print("\n" + "="*70)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <serial_port>")
        print(f"Example: {sys.argv[0]} /dev/tty.usbserial-210183A27BE01")
        sys.exit(1)

    main(sys.argv[1])