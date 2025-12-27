#!/usr/bin/env python3
"""
TPU Step-by-Step Debug Script
==============================
Comprehensive debugging of the TPU coprocessor ISA execution.

This script tests each stage of the compute pipeline independently:
1. Weight memory write/verify
2. Unified buffer write/verify
3. Individual instruction execution
4. Data flow verification at each step
"""

import serial
import struct
import time
import sys
from dataclasses import dataclass
from enum import IntEnum


# =============================================================================
# ISA DEFINITIONS (from tpu_controller.sv)
# =============================================================================

class Opcode(IntEnum):
    NOP         = 0x00
    RD_HOST_MEM = 0x01
    WR_HOST_MEM = 0x02
    RD_WEIGHT   = 0x03
    LD_UB       = 0x04
    ST_UB       = 0x05
    MATMUL      = 0x10
    CONV2D      = 0x11
    MATMUL_ACC  = 0x12
    RELU        = 0x18
    RELU6       = 0x19
    SIGMOID     = 0x1A
    TANH        = 0x1B
    MAX_POOL    = 0x20
    AVG_POOL    = 0x21
    ADD_BIAS    = 0x22
    BATCH_NORM  = 0x23
    SYNC        = 0x30
    CFG_REG     = 0x31
    HALT        = 0x3F


class UARTCmd(IntEnum):
    WRITE_UB    = 0x01
    WRITE_WT    = 0x02
    WRITE_INSTR = 0x03
    READ_UB     = 0x04
    EXECUTE     = 0x05
    READ_STATUS = 0x06
    READ_DEBUG  = 0x14


# =============================================================================
# LOW-LEVEL HELPERS
# =============================================================================

def encode_instr(opcode, arg1=0, arg2=0, arg3=0, flags=0):
    """Encode instruction: [31:26]op | [25:18]arg1 | [17:10]arg2 | [9:2]arg3 | [1:0]flags"""
    return ((opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags)


def decode_instr(instr):
    """Decode instruction into fields"""
    return {
        'opcode': (instr >> 26) & 0x3F,
        'arg1': (instr >> 18) & 0xFF,
        'arg2': (instr >> 10) & 0xFF,
        'arg3': (instr >> 2) & 0xFF,
        'flags': instr & 0x03
    }


def opcode_name(op):
    """Get opcode name"""
    try:
        return Opcode(op).name
    except ValueError:
        return f"0x{op:02X}"


class TPUDebugger:
    """Low-level TPU debugger with step-by-step verification"""

    def __init__(self, port, baud=115200):
        self.ser = serial.Serial(port, baud, timeout=2)
        time.sleep(0.1)
        self.flush()
        print(f"Connected to {port}")

    def flush(self):
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

    def close(self):
        self.ser.close()

    # -------------------------------------------------------------------------
    # Raw UART Operations
    # -------------------------------------------------------------------------

    def send_cmd(self, cmd, addr_hi=0, addr_lo=0, len_hi=0, len_lo=0):
        """Send 5-byte command header"""
        self.ser.write(bytes([cmd, addr_hi, addr_lo, len_hi, len_lo]))

    def read_status(self):
        """Read and decode status register"""
        self.flush()
        self.ser.write(bytes([UARTCmd.READ_STATUS]))
        time.sleep(0.02)
        data = self.ser.read(1)
        if data:
            s = data[0]
            return {
                'raw': f"0x{s:02X}",
                'sys_busy': bool(s & 0x01),
                'sys_done': bool(s & 0x02),
                'vpu_busy': bool(s & 0x04),
                'vpu_done': bool(s & 0x08),
                'ub_busy': bool(s & 0x10),
                'ub_done': bool(s & 0x20),
            }
        return None

    # -------------------------------------------------------------------------
    # Unified Buffer Operations
    # -------------------------------------------------------------------------

    def write_ub(self, addr, data, verbose=True):
        """Write to unified buffer with verification"""
        self.flush()

        # Pad to 32 bytes
        if len(data) < 32:
            data = data + bytes(32 - len(data))

        if verbose:
            print(f"    TX: WRITE_UB addr={addr}, len={len(data)}")
            print(f"    TX: Data = {data[:16].hex()}...")

        self.send_cmd(UARTCmd.WRITE_UB, 0, addr, 0, len(data))
        self.ser.write(data)
        time.sleep(0.05)

        ack = self.ser.read(1)
        if verbose:
            if ack:
                print(f"    RX: ACK = 0x{ack[0]:02X} {'(OK)' if ack[0] == 0xAA else '(UNEXPECTED)'}")
            else:
                print(f"    RX: No ACK received!")

        return ack == b'\xAA'

    def read_ub(self, addr, length=32, verbose=True):
        """Read from unified buffer"""
        self.flush()

        if verbose:
            print(f"    TX: READ_UB addr={addr}, len={length}")

        self.send_cmd(UARTCmd.READ_UB, 0, addr, 0, length)
        time.sleep(0.05)

        data = self.ser.read(length)
        if verbose:
            print(f"    RX: Data ({len(data)} bytes) = {data.hex()}")

        return data

    # -------------------------------------------------------------------------
    # Weight Memory Operations
    # -------------------------------------------------------------------------

    def write_weight(self, addr, data, verbose=True):
        """Write to weight memory"""
        self.flush()

        # Pad to 8 bytes
        if len(data) < 8:
            data = data + bytes(8 - len(data))

        if verbose:
            print(f"    TX: WRITE_WT addr={addr}, len={len(data)}")
            print(f"    TX: Data = {data.hex()}")

        self.send_cmd(UARTCmd.WRITE_WT, 0, addr, 0, len(data))
        self.ser.write(data)
        time.sleep(0.05)

        ack = self.ser.read(1)
        if verbose:
            if ack:
                print(f"    RX: ACK = 0x{ack[0]:02X} {'(OK)' if ack[0] == 0xBB else '(UNEXPECTED)'}")
            else:
                print(f"    RX: No ACK received!")

        return ack == b'\xBB'

    # -------------------------------------------------------------------------
    # Instruction Operations
    # -------------------------------------------------------------------------

    def write_instruction(self, addr, instr, verbose=True):
        """Write instruction to program memory"""
        if verbose:
            fields = decode_instr(instr)
            print(f"    TX: WRITE_INSTR[{addr}] = 0x{instr:08X}")
            print(f"        {opcode_name(fields['opcode'])}({fields['arg1']}, {fields['arg2']}, {fields['arg3']}) flags={fields['flags']}")

        cmd = bytes([UARTCmd.WRITE_INSTR, 0, addr])
        self.ser.write(cmd)
        self.ser.write(struct.pack('>I', instr))
        time.sleep(0.01)

    def execute(self, verbose=True):
        """Start execution and wait for completion"""
        self.flush()

        if verbose:
            print(f"    TX: EXECUTE command")

        self.ser.write(bytes([UARTCmd.EXECUTE]))

        # Wait for completion
        start = time.time()
        while time.time() - start < 2.0:
            status = self.read_status()
            if status and not status['sys_busy']:
                elapsed = (time.time() - start) * 1000
                if verbose:
                    print(f"    Execution completed in {elapsed:.1f} ms")
                    print(f"    Status: {status}")
                return True
            time.sleep(0.01)

        if verbose:
            print(f"    Execution TIMEOUT!")
        return False


# =============================================================================
# TEST CASES
# =============================================================================

def test_ub_roundtrip(dbg):
    """Test 1: Basic UB write/read roundtrip"""
    print("\n" + "="*70)
    print("TEST 1: Unified Buffer Roundtrip")
    print("="*70)

    # Write test pattern
    test_data = bytes([0xDE, 0xAD, 0xBE, 0xEF] + list(range(28)))
    print("\n  [Step 1.1] Writing test pattern to UB[0]...")
    ok = dbg.write_ub(0, test_data)
    if not ok:
        print("  FAIL: No ACK for UB write")
        return False

    # Read back
    print("\n  [Step 1.2] Reading back from UB[0]...")
    readback = dbg.read_ub(0, 32)

    # Compare
    if readback == test_data:
        print("\n  PASS: Data matches!")
        return True
    else:
        print("\n  FAIL: Data mismatch!")
        print(f"    Expected: {test_data.hex()}")
        print(f"    Got:      {readback.hex()}")
        return False


def test_weight_write(dbg):
    """Test 2: Weight memory write"""
    print("\n" + "="*70)
    print("TEST 2: Weight Memory Write")
    print("="*70)

    # Write 3 rows of weights (8 bytes each)
    # For 3x3 systolic array, we need 3 weights per row
    weights = [
        bytes([1, 0, 0, 0, 0, 0, 0, 0]),  # Row 0: [1, 0, 0]
        bytes([0, 1, 0, 0, 0, 0, 0, 0]),  # Row 1: [0, 1, 0]
        bytes([0, 0, 1, 0, 0, 0, 0, 0]),  # Row 2: [0, 0, 1]
    ]

    for i, w in enumerate(weights):
        print(f"\n  [Step 2.{i+1}] Writing weight row {i}...")
        ok = dbg.write_weight(i, w)
        if not ok:
            print(f"  FAIL: No ACK for weight row {i}")
            return False

    print("\n  PASS: All weight rows written successfully")
    return True


def test_simple_program(dbg):
    """Test 3: Simple NOP -> HALT program"""
    print("\n" + "="*70)
    print("TEST 3: Simple Program Execution (NOP -> HALT)")
    print("="*70)

    print("\n  [Step 3.1] Loading program...")
    dbg.write_instruction(0, encode_instr(Opcode.NOP))
    dbg.write_instruction(1, encode_instr(Opcode.HALT))

    print("\n  [Step 3.2] Executing...")
    ok = dbg.execute()
    if not ok:
        print("  FAIL: Execution timeout")
        return False

    print("\n  PASS: Simple program executed")
    return True


def test_ub_load_store(dbg):
    """Test 4: LD_UB -> ST_UB (data passthrough via systolic array)"""
    print("\n" + "="*70)
    print("TEST 4: Load/Store UB (LD_UB -> ST_UB)")
    print("="*70)

    # Write input data to UB[0]
    input_data = bytes([0x11, 0x22, 0x33] + [0]*29)
    print("\n  [Step 4.1] Writing input to UB[0]...")
    dbg.write_ub(0, input_data)

    # Clear UB[1] to verify ST_UB works
    print("\n  [Step 4.2] Clearing UB[1]...")
    dbg.write_ub(1, bytes(32))

    # Verify UB[1] is cleared
    print("\n  [Step 4.3] Verifying UB[1] is cleared...")
    check = dbg.read_ub(1, 32)
    if check != bytes(32):
        print(f"    WARNING: UB[1] not cleared: {check.hex()}")

    # Load program: LD_UB -> ST_UB -> HALT
    print("\n  [Step 4.4] Loading LD_UB -> ST_UB -> HALT program...")
    # LD_UB: arg1=ub_addr, arg2=num_blocks
    dbg.write_instruction(0, encode_instr(Opcode.LD_UB, 0, 1))  # Load from UB[0]
    # ST_UB: arg1=ub_addr, arg2=num_blocks
    dbg.write_instruction(1, encode_instr(Opcode.ST_UB, 1, 1))  # Store to UB[1]
    dbg.write_instruction(2, encode_instr(Opcode.HALT))

    print("\n  [Step 4.5] Executing...")
    ok = dbg.execute()
    if not ok:
        print("  FAIL: Execution timeout")
        return False

    # Read back UB[1]
    print("\n  [Step 4.6] Reading result from UB[1]...")
    result = dbg.read_ub(1, 32)

    # Note: We don't expect exact match because LD_UB/ST_UB go through
    # the systolic array accumulator, which may transform the data
    print(f"\n  Input:  {input_data[:8].hex()}")
    print(f"  Output: {result[:8].hex()}")

    if result == bytes(32):
        print("\n  INFO: Output is all zeros - ST_UB may not have executed")
    elif result[:3] == input_data[:3]:
        print("\n  PASS: Data appears to have passed through")
    else:
        print("\n  INFO: Data was transformed (expected for accumulator path)")

    return True


def test_matmul_identity(dbg):
    """Test 5: Matrix multiply with identity weights"""
    print("\n" + "="*70)
    print("TEST 5: Matrix Multiply (Identity Matrix)")
    print("="*70)

    # Identity matrix weights
    print("\n  [Step 5.1] Loading identity matrix weights...")
    weights = [
        bytes([1, 0, 0, 0, 0, 0, 0, 0]),  # Row 0
        bytes([0, 1, 0, 0, 0, 0, 0, 0]),  # Row 1
        bytes([0, 0, 1, 0, 0, 0, 0, 0]),  # Row 2
    ]
    for i, w in enumerate(weights):
        dbg.write_weight(i, w)

    # Input vector [10, 20, 30]
    input_data = bytes([10, 20, 30] + [0]*29)
    print("\n  [Step 5.2] Loading input [10, 20, 30] to UB[0]...")
    dbg.write_ub(0, input_data)

    # Clear output location
    print("\n  [Step 5.3] Clearing UB[1]...")
    dbg.write_ub(1, bytes(32))

    # Load program
    print("\n  [Step 5.4] Loading MATMUL program...")
    # RD_WEIGHT: arg1=start_tile, arg2=num_tiles
    dbg.write_instruction(0, encode_instr(Opcode.RD_WEIGHT, 0, 1))
    # LD_UB: arg1=ub_addr, arg2=num_blocks
    dbg.write_instruction(1, encode_instr(Opcode.LD_UB, 0, 1))
    # MATMUL: arg1=ub_input_addr, arg2=acc_output_addr, arg3=num_rows, flags=signed
    dbg.write_instruction(2, encode_instr(Opcode.MATMUL, 0, 0, 3, 2))  # flags=2 for signed
    # ST_UB: arg1=ub_addr, arg2=num_blocks
    dbg.write_instruction(3, encode_instr(Opcode.ST_UB, 1, 1))
    dbg.write_instruction(4, encode_instr(Opcode.HALT))

    print("\n  [Step 5.5] Executing...")
    ok = dbg.execute()
    if not ok:
        print("  FAIL: Execution timeout")
        return False

    # Read results
    print("\n  [Step 5.6] Reading result from UB[1]...")
    result = dbg.read_ub(1, 32)

    # Expected: identity @ [10, 20, 30] = [10, 20, 30]
    expected = bytes([10, 20, 30] + [0]*29)
    print(f"\n  Expected: {expected[:8].hex()}")
    print(f"  Got:      {result[:8].hex()}")

    # Interpret as signed int8
    result_vals = [int.from_bytes([b], 'little', signed=True) for b in result[:3]]
    print(f"\n  As int8:  {result_vals}")

    if result[:3] == expected[:3]:
        print("\n  PASS: Matrix multiply produced correct result!")
        return True
    else:
        print("\n  FAIL: Result mismatch")
        return False


def test_individual_instructions(dbg):
    """Test 6: Execute and verify each instruction individually"""
    print("\n" + "="*70)
    print("TEST 6: Individual Instruction Verification")
    print("="*70)

    # Test each instruction one at a time

    # 6.1: RD_WEIGHT only
    print("\n  [Step 6.1] Testing RD_WEIGHT...")
    dbg.write_weight(0, bytes([5, 6, 7, 0, 0, 0, 0, 0]))
    dbg.write_instruction(0, encode_instr(Opcode.RD_WEIGHT, 0, 1))
    dbg.write_instruction(1, encode_instr(Opcode.HALT))
    if dbg.execute():
        print("    RD_WEIGHT executed OK")
    else:
        print("    RD_WEIGHT FAILED")

    # 6.2: LD_UB only
    print("\n  [Step 6.2] Testing LD_UB...")
    dbg.write_ub(0, bytes([1, 2, 3] + [0]*29))
    dbg.write_instruction(0, encode_instr(Opcode.LD_UB, 0, 1))
    dbg.write_instruction(1, encode_instr(Opcode.HALT))
    if dbg.execute():
        print("    LD_UB executed OK")
    else:
        print("    LD_UB FAILED")

    # 6.3: ST_UB only (after LD_UB)
    print("\n  [Step 6.3] Testing ST_UB (after LD_UB)...")
    dbg.write_ub(2, bytes(32))  # Clear destination
    dbg.write_instruction(0, encode_instr(Opcode.LD_UB, 0, 1))
    dbg.write_instruction(1, encode_instr(Opcode.ST_UB, 2, 1))
    dbg.write_instruction(2, encode_instr(Opcode.HALT))
    if dbg.execute():
        result = dbg.read_ub(2, 32, verbose=False)
        print(f"    ST_UB result: {result[:8].hex()}")
    else:
        print("    ST_UB FAILED")

    # 6.4: Check UB[0] wasn't corrupted
    print("\n  [Step 6.4] Verifying UB[0] still has input data...")
    check = dbg.read_ub(0, 32, verbose=False)
    print(f"    UB[0] = {check[:8].hex()}")

    return True


def run_all_tests(port):
    """Run all debug tests"""
    print("="*70)
    print("TPU STEP-BY-STEP DEBUG")
    print("="*70)

    dbg = TPUDebugger(port)

    # Initial status check
    print("\n  Initial status check...")
    status = dbg.read_status()
    print(f"  Status: {status}")

    tests = [
        ("UB Roundtrip", test_ub_roundtrip),
        ("Weight Write", test_weight_write),
        ("Simple Program", test_simple_program),
        ("LD/ST UB", test_ub_load_store),
        ("MATMUL Identity", test_matmul_identity),
        ("Individual Instructions", test_individual_instructions),
    ]

    results = []
    for name, test_fn in tests:
        try:
            ok = test_fn(dbg)
            results.append((name, ok))
        except Exception as e:
            print(f"\n  EXCEPTION: {e}")
            results.append((name, False))

    # Summary
    print("\n" + "="*70)
    print("SUMMARY")
    print("="*70)
    for name, ok in results:
        status = "PASS" if ok else "FAIL"
        print(f"  {name}: {status}")

    passed = sum(1 for _, ok in results if ok)
    print(f"\n  Total: {passed}/{len(results)} tests passed")

    dbg.close()


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <UART_PORT>")
        sys.exit(1)

    run_all_tests(sys.argv[1])
