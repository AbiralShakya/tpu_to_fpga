#!/usr/bin/env python3
"""
Bank Selection Verification Test

This test verifies that UART writes and reads use the same bank,
and that ST_UB writes to a bank that UART can read.
"""

import sys
import time
import serial

class Cmd:
    WRITE_UB = 0x01
    READ_UB = 0x04
    WRITE_INSTR = 0x03
    EXECUTE = 0x05

ACK_UB = 0xAA

class BankTest:
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

    def write_instruction(self, addr, instr):
        self.ser.reset_input_buffer()
        self.ser.write(bytes([Cmd.WRITE_INSTR, 0, addr]))
        self.ser.write(instr.to_bytes(4, 'big'))
        time.sleep(0.05)

    def encode_instruction(self, opcode, arg1=0, arg2=0, arg3=0, flags=0):
        return ((opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags)

    def execute(self, timeout=2.0):
        self.ser.reset_input_buffer()
        self.ser.write(bytes([Cmd.EXECUTE]))
        time.sleep(0.05)
        start = time.time()
        while time.time() - start < timeout:
            self.send_cmd(0x06, 0, 0, 0, 0)  # READ_STATUS
            time.sleep(0.02)
            status = self.ser.read(1)
            if status:
                status_byte = status[0]
                if not (status_byte & 0x15):  # All idle
                    return True
            time.sleep(0.01)
        return False

def test_uart_write_read_same_bank(tpu):
    """Test that UART can write and immediately read back from the same address"""
    print("="*60)
    print("TEST: UART Write/Read Same Bank")
    print("="*60)
    
    test_addr = 70
    marker = bytes([0xAA]*32)
    
    print(f"\n1. Writing marker 0xAA to UB[{test_addr}]...")
    if not tpu.write_ub(test_addr, marker):
        print("  ✗ Write failed")
        return False
    print("  ✓ Write ACK received")
    
    print(f"\n2. Reading back from UB[{test_addr}]...")
    result = tpu.read_ub(test_addr, 32)
    print(f"  Read: {result[:8].hex()}")
    
    if result[:8] == marker[:8]:
        print("\n  ✓ PASS: UART can read what it wrote (same bank)")
        return True
    else:
        print(f"\n  ✗ FAIL: UART wrote {marker[:8].hex()} but read {result[:8].hex()}")
        print("  → Bank mismatch: Write and read are using different banks!")
        return False

def test_stub_writes_to_uart_readable_bank(tpu):
    """Test that ST_UB writes to bank 0 (same as UART reads)"""
    print("\n" + "="*60)
    print("TEST: ST_UB Writes to UART-Readable Bank")
    print("="*60)
    
    # First, populate accumulators with known values
    print("\n1. Populating accumulators...")
    # Load identity weights
    from diagnostic_isolated import TPUTest
    tpu_full = TPUTest(tpu.ser.port)
    
    weights = [
        bytes([0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),
        bytes([0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),
        bytes([0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00])
    ]
    for i, w in enumerate(weights):
        tpu_full.write_weight(i, w)
    
    inputs = bytes([0x0A, 0x14, 0x1E]) + bytes(29)
    tpu_full.write_ub(0, inputs)
    
    # MATMUL program
    program = [
        tpu.encode_instruction(0x03, 0, 1, 0, 0),  # RD_WEIGHT
        tpu.encode_instruction(0x03, 1, 1, 0, 0),
        tpu.encode_instruction(0x03, 2, 1, 0, 0),
        tpu.encode_instruction(0x04, 0, 1, 0, 0),  # LD_UB
        tpu.encode_instruction(0x10, 0, 0, 3, 2), # MATMUL
        tpu.encode_instruction(0x3F)                # HALT
    ]
    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)
    tpu.execute()
    print("  ✓ MATMUL completed")
    
    # Write marker to test address
    test_addr = 80
    marker = bytes([0xBB]*32)
    print(f"\n2. Writing marker 0xBB to UB[{test_addr}]...")
    tpu.write_ub(test_addr, marker)
    before = tpu.read_ub(test_addr)
    print(f"  Before ST_UB: {before[:8].hex()}")
    
    # ST_UB to same address
    print(f"\n3. Executing ST_UB to UB[{test_addr}]...")
    ST_UB = tpu.encode_instruction(0x05, test_addr, 1, 0, 0)
    HALT = tpu.encode_instruction(0x3F)
    tpu.write_instruction(0, ST_UB)
    tpu.write_instruction(1, HALT)
    tpu.execute()
    
    # Read back
    after = tpu.read_ub(test_addr)
    print(f"  After ST_UB: {after[:8].hex()}")
    
    acc0 = after[0] if after[0] < 128 else after[0] - 256
    acc1 = after[1] if after[1] < 128 else after[1] - 256
    acc2 = after[2] if after[2] < 128 else after[2] - 256
    print(f"  Interpreted: [{acc0}, {acc1}, {acc2}]")
    
    if after[:8] == marker[:8]:
        print("\n  ✗ FAIL: ST_UB did NOT modify UB (bank mismatch or not writing)")
        return False
    elif [acc0, acc1, acc2] == [10, 20, 30]:
        print("\n  ✓ PASS: ST_UB wrote to bank 0 (UART-readable)")
        return True
    else:
        print(f"\n  ⚠ PARTIAL: ST_UB modified UB but values wrong")
        print(f"  → Bank selection may be correct, but accumulator values are wrong")
        return False

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 test_bank_verification.py <UART_PORT>")
        sys.exit(1)
    
    port = sys.argv[1]
    tpu = BankTest(port)
    
    print("="*60)
    print("BANK SELECTION VERIFICATION TESTS")
    print("="*60)
    print("\nThese tests verify bank selection is consistent between")
    print("UART operations and ISA instructions (ST_UB).\n")
    
    test1_pass = test_uart_write_read_same_bank(tpu)
    test2_pass = test_stub_writes_to_uart_readable_bank(tpu)
    
    print("\n" + "="*60)
    print("SUMMARY")
    print("="*60)
    print(f"Test 1 (UART write/read same bank): {'PASS' if test1_pass else 'FAIL'}")
    print(f"Test 2 (ST_UB writes to UART-readable bank): {'PASS' if test2_pass else 'FAIL'}")
    
    if test1_pass and test2_pass:
        print("\n✓ All bank selection tests PASSED!")
    else:
        print("\n✗ Some bank selection tests FAILED")
        if not test1_pass:
            print("  → UART cannot read what it wrote - fundamental bank issue")
        if not test2_pass:
            print("  → ST_UB writes to different bank than UART reads")
    
    tpu.ser.close()

if __name__ == "__main__":
    main()
