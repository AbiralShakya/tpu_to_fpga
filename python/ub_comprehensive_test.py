#!/usr/bin/env python3
"""
Comprehensive Unified Buffer Diagnostic Suite

This test suite systematically diagnoses UB write failures by testing:
1. UART write/read functionality (baseline)
2. Simple ST_UB without any computation
3. ST_UB timing with different instruction sequences
4. Bank selection during ISA execution
5. Accumulator latching and acc_data_out path
6. use_test_interface mux timing

Root cause analysis for ST_UB write failure.
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
        """Send 5-byte command header"""
        self.ser.write(bytes([cmd, addr_hi, addr_lo, len_hi, len_lo]))

    def write_ub(self, addr, data):
        """Write 32 bytes to Unified Buffer"""
        data = data.ljust(32, b'\x00')[:32]
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.WRITE_UB, 0, addr, 0, 32)
        self.ser.write(data)
        time.sleep(0.05)
        ack = self.ser.read(1)
        return ack == bytes([ACK_UB])

    def read_ub(self, addr, length=32):
        """Read from Unified Buffer"""
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.READ_UB, 0, addr, 0, length)
        time.sleep(0.05)
        return self.ser.read(length)

    def write_instruction(self, addr, instr):
        """Write 32-bit instruction"""
        self.ser.reset_input_buffer()
        # CRITICAL FIX: WRITE_INSTR expects only 3 header bytes (cmd, addr_hi, addr_lo)
        # NOT 5 bytes like other commands - no length bytes for fixed-size instruction
        self.ser.write(bytes([Cmd.WRITE_INSTR, 0, addr]))  # Only 3 bytes!
        self.ser.write(instr.to_bytes(4, 'big'))
        time.sleep(0.05)
        return True

    def execute(self, timeout=2.0):
        """Execute program and wait for completion"""
        self.ser.reset_input_buffer()
        self.ser.write(bytes([Cmd.EXECUTE]))
        time.sleep(0.05)
        start = time.time()
        while time.time() - start < timeout:
            self.ser.reset_input_buffer()
            self.ser.write(bytes([Cmd.READ_STATUS]))
            time.sleep(0.005)
            status = self.ser.read(1)
            if status and (status[0] & 0x01) == 0:  # Not busy
                return True
            time.sleep(0.01)
        return False

    def encode_instruction(self, opcode, arg1=0, arg2=0, arg3=0, flags=0):
        """Encode instruction"""
        return ((opcode & 0x3F) << 26) | ((arg1 & 0xFF) << 18) | ((arg2 & 0xFF) << 10) | ((arg3 & 0xFF) << 2) | (flags & 0x03)

# =============================================================================
# COMPREHENSIVE DIAGNOSTIC TESTS
# =============================================================================

def test_uart_baseline(tpu):
    """TEST 0: UART baseline - can we write and read?"""
    print("\n" + "="*70)
    print("TEST 0: UART Baseline (Write + Read)")
    print("="*70)

    # Write unique patterns to multiple addresses
    test_patterns = [
        (0, bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88] + [0]*24)),
        (5, bytes([0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE] + [0]*24)),
        (10, bytes([0xAA]*32)),
        (20, bytes([0x55]*32)),
    ]

    all_pass = True
    for addr, pattern in test_patterns:
        tpu.write_ub(addr, pattern)
        readback = tpu.read_ub(addr)
        match = readback[:8] == pattern[:8]
        print(f"  UB[{addr:3d}]: Write {pattern[:8].hex()} → Read {readback[:8].hex()} {'✓' if match else '✗'}")
        all_pass = all_pass and match

    print(f"\n  {'✓ PASS' if all_pass else '✗ FAIL'}: UART write/read")
    return all_pass

def test_minimal_st_ub(tpu):
    """TEST 1: Minimal ST_UB - just ST_UB then HALT"""
    print("\n" + "="*70)
    print("TEST 1: Minimal ST_UB (No Computation)")
    print("="*70)

    # Clear UB[5] to all zeros first
    tpu.write_ub(5, bytes([0]*32))
    before = tpu.read_ub(5)
    print(f"  UB[5] before: {before[:8].hex()}")

    # Program: ST_UB(5, 1) → HALT
    ST_UB = tpu.encode_instruction(0x05, arg1=5, arg2=1)
    HALT = tpu.encode_instruction(0x3F)
    tpu.write_instruction(0, ST_UB)
    tpu.write_instruction(1, HALT)

    success = tpu.execute()
    print(f"  Execution: {'OK' if success else 'TIMEOUT'}")

    after = tpu.read_ub(5)
    print(f"  UB[5] after:  {after[:8].hex()}")
    print(f"  Expected: acc_data_out (likely all zeros if no computation)")

    # Check if value changed
    changed = before[:8] != after[:8]
    print(f"\n  {'✓ PASS' if changed else '✗ FAIL'}: ST_UB {'modified' if changed else 'did NOT modify'} UB[5]")

    if changed:
        print(f"  → ST_UB wrote: {after[:8].hex()}")
        print(f"  → This is acc_data_out (accumulators after reset)")
    else:
        print(f"  → FAILURE: ST_UB write enable NOT reaching UB!")
        print(f"  → Check: use_test_interface mux, ctrl_ub_wr_en signal")

    return changed, after[:8].hex()

def test_st_ub_with_marker(tpu):
    """TEST 2: ST_UB over existing marker pattern"""
    print("\n" + "="*70)
    print("TEST 2: ST_UB Overwrites Existing Data")
    print("="*70)

    # Write marker pattern
    marker = bytes([0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED, 0xFA, 0xCE] + [0]*24)
    tpu.write_ub(7, marker)
    before = tpu.read_ub(7)
    print(f"  UB[7] before: {before[:8].hex()}")

    # Program: ST_UB(7, 1) → HALT
    ST_UB = tpu.encode_instruction(0x05, arg1=7, arg2=1)
    HALT = tpu.encode_instruction(0x3F)
    tpu.write_instruction(0, ST_UB)
    tpu.write_instruction(1, HALT)

    success = tpu.execute()
    after = tpu.read_ub(7)
    print(f"  UB[7] after:  {after[:8].hex()}")

    changed = before[:8] != after[:8]
    print(f"\n  {'✓ PASS' if changed else '✗ FAIL'}: ST_UB {'overwrote' if changed else 'did NOT overwrite'} marker")

    return changed

def test_multiple_st_ub(tpu):
    """TEST 3: Multiple ST_UB instructions in sequence"""
    print("\n" + "="*70)
    print("TEST 3: Multiple ST_UB Instructions")
    print("="*70)

    # Clear addresses
    for addr in [10, 11, 12]:
        tpu.write_ub(addr, bytes([0]*32))

    # Program: ST_UB(10) → ST_UB(11) → ST_UB(12) → HALT
    instructions = [
        tpu.encode_instruction(0x05, arg1=10, arg2=1),  # ST_UB(10)
        tpu.encode_instruction(0x05, arg1=11, arg2=1),  # ST_UB(11)
        tpu.encode_instruction(0x05, arg1=12, arg2=1),  # ST_UB(12)
        tpu.encode_instruction(0x3F),                   # HALT
    ]

    for i, instr in enumerate(instructions):
        tpu.write_instruction(i, instr)

    success = tpu.execute()
    print(f"  Execution: {'OK' if success else 'TIMEOUT'}")

    # Read all addresses
    results = []
    for addr in [10, 11, 12]:
        data = tpu.read_ub(addr)
        results.append(data[:8])
        print(f"  UB[{addr}]: {data[:8].hex()}")

    # Check if any changed from zeros
    any_changed = any(result != bytes([0]*8) for result in results)
    print(f"\n  {'✓ PASS' if any_changed else '✗ FAIL'}: At least one ST_UB wrote data")

    return any_changed

def test_st_ub_after_nop(tpu):
    """TEST 4: ST_UB after NOPs (test pipeline advancement)"""
    print("\n" + "="*70)
    print("TEST 4: ST_UB After NOP Instructions")
    print("="*70)

    # Clear UB[15]
    tpu.write_ub(15, bytes([0]*32))

    # Program: NOP → NOP → NOP → ST_UB(15) → HALT
    # NOP can be encoded as opcode 0x00 with all args 0
    instructions = [
        tpu.encode_instruction(0x00),  # NOP
        tpu.encode_instruction(0x00),  # NOP
        tpu.encode_instruction(0x00),  # NOP
        tpu.encode_instruction(0x05, arg1=15, arg2=1),  # ST_UB(15)
        tpu.encode_instruction(0x3F),  # HALT
    ]

    for i, instr in enumerate(instructions):
        tpu.write_instruction(i, instr)

    success = tpu.execute()
    after = tpu.read_ub(15)
    print(f"  UB[15] after NOPs+ST_UB: {after[:8].hex()}")

    changed = after[:8] != bytes([0]*8)
    print(f"\n  {'✓ PASS' if changed else '✗ FAIL'}: ST_UB executed after NOPs")

    return changed

def test_acc_data_out_path(tpu):
    """TEST 5: Verify acc_data_out is routed correctly"""
    print("\n" + "="*70)
    print("TEST 5: Accumulator Data Path (acc_data_out)")
    print("="*70)

    # Write different markers, execute ST_UB to each
    addresses = [20, 21, 22, 23]
    markers = [bytes([0xAA]*32), bytes([0xBB]*32), bytes([0xCC]*32), bytes([0xDD]*32)]

    for addr, marker in zip(addresses, markers):
        tpu.write_ub(addr, marker)

    # Execute ST_UB to each address
    results = []
    for i, addr in enumerate(addresses):
        ST_UB = tpu.encode_instruction(0x05, arg1=addr, arg2=1)
        HALT = tpu.encode_instruction(0x3F)
        tpu.write_instruction(0, ST_UB)
        tpu.write_instruction(1, HALT)
        tpu.execute()

        data = tpu.read_ub(addr)
        results.append(data[:8])
        print(f"  UB[{addr}]: Marker {marker[:4].hex()} → ST_UB → {data[:8].hex()}")

    # Check if all results are identical (they should all be acc_data_out)
    all_same = all(r == results[0] for r in results)
    print(f"\n  All ST_UB results identical: {all_same}")
    print(f"  → This is acc_data_out value: {results[0].hex()}")

    # Check if any marker was overwritten
    any_changed = any(results[i] != markers[i][:8] for i in range(len(addresses)))
    print(f"  Any marker changed: {any_changed}")

    print(f"\n  {'✓ PASS' if any_changed else '✗ FAIL'}: acc_data_out path")

    return any_changed

def test_bank_selection(tpu):
    """TEST 6: Verify bank selection (UART vs ISA)"""
    print("\n" + "="*70)
    print("TEST 6: Bank Selection (UART always bank 0)")
    print("="*70)

    # UART writes to UB[30] (bank 0 by default)
    marker = bytes([0xBE, 0xEF] + [0]*30)
    tpu.write_ub(30, marker)
    before = tpu.read_ub(30)
    print(f"  UART wrote to UB[30]: {before[:8].hex()}")

    # Execute ST_UB(30) - should write to same location
    ST_UB = tpu.encode_instruction(0x05, arg1=30, arg2=1)
    HALT = tpu.encode_instruction(0x3F)
    tpu.write_instruction(0, ST_UB)
    tpu.write_instruction(1, HALT)
    tpu.execute()

    after = tpu.read_ub(30)
    print(f"  After ST_UB(30):      {after[:8].hex()}")

    changed = before[:8] != after[:8]
    print(f"\n  {'✓ PASS' if changed else '✗ FAIL'}: ST_UB {'wrote to' if changed else 'did NOT write to'} same bank as UART")

    if not changed:
        print(f"  → CRITICAL: Bank mismatch OR ST_UB not writing at all!")
        print(f"  → exec_ub_buf_sel should be 0 (bank 0) after start_execution")

    return changed

# =============================================================================
# MAIN TEST RUNNER
# =============================================================================

def main(port):
    tpu = TPU(port)

    print("\n" + "="*70)
    print("COMPREHENSIVE UB DIAGNOSTIC SUITE")
    print("="*70)
    print(f"Connected to: {port}")

    results = {}

    # Run all tests
    results['uart_baseline'] = test_uart_baseline(tpu)
    results['minimal_st_ub'], acc_value = test_minimal_st_ub(tpu)
    results['st_ub_marker'] = test_st_ub_with_marker(tpu)
    results['multiple_st_ub'] = test_multiple_st_ub(tpu)
    results['st_ub_after_nop'] = test_st_ub_after_nop(tpu)
    results['acc_data_out_path'] = test_acc_data_out_path(tpu)
    results['bank_selection'] = test_bank_selection(tpu)

    # Summary
    print("\n" + "="*70)
    print("DIAGNOSTIC SUMMARY")
    print("="*70)

    for test_name, passed in results.items():
        status = "✓ PASS" if passed else "✗ FAIL"
        print(f"  {test_name:25s}: {status}")

    # Analysis
    print("\n" + "="*70)
    print("ROOT CAUSE ANALYSIS")
    print("="*70)

    if not results['uart_baseline']:
        print("  ✗ UART communication broken - fix this first!")
    elif not results['minimal_st_ub']:
        print("  ✗ ST_UB instruction NOT writing to UB")
        print("  → Likely causes:")
        print("    1. use_test_interface mux blocking ctrl_ub_wr_en")
        print("    2. ctrl_ub_wr_en signal never asserted")
        print("    3. Pipeline not advancing to exec stage")
        print(f"  → acc_data_out value: {acc_value}")
        print("  → Next step: Check use_test_interface timing with logic analyzer")
    elif not results['bank_selection']:
        print("  ✗ Bank selection mismatch between UART and ISA")
        print("  → exec_ub_buf_sel not matching UART bank (should both be bank 0)")
    else:
        print("  ✓ ST_UB is working! Issue may be specific to certain sequences.")

    print("="*70)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <serial_port>")
        print(f"Example: {sys.argv[0]} /dev/tty.usbserial-210183A27BE01")
        sys.exit(1)

    main(sys.argv[1])
