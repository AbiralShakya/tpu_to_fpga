#!/usr/bin/env python3
"""
Isolated Diagnostic Tests for TPU MATMUL Pipeline

This script methodically tests each stage of the MATMUL data path
to isolate exactly where the failure occurs.

Data flow being tested:
  1. Weights → Weight Memory → Weight FIFO → Systolic Array PEs
  2. Activations → UB → Systolic Array
  3. Systolic Array → Accumulators → acc_data_out
  4. acc_data_out → ST_UB → UB
  5. UB → UART readback

Run each test independently to isolate the failure point.
"""

import sys
import time
import serial
import numpy as np

# =============================================================================
# UART PROTOCOL CONSTANTS
# =============================================================================
class Cmd:
    WRITE_UB = 0x01
    WRITE_WT = 0x02
    WRITE_INSTR = 0x03
    READ_UB = 0x04
    EXECUTE = 0x05
    READ_STATUS = 0x06

ACK_UB = 0xAA
ACK_WT = 0xBB

# =============================================================================
# LOW-LEVEL UART FUNCTIONS
# =============================================================================
class TPUDebug:
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

    def write_weight(self, addr, data):
        """Write 8 bytes to weight memory"""
        data = data.ljust(8, b'\x00')[:8]
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.WRITE_WT, 0, addr, 0, 8)
        self.ser.write(data)
        time.sleep(0.05)
        ack = self.ser.read(1)
        return ack == bytes([ACK_WT])

    def write_instruction(self, addr, instr):
        """Write 32-bit instruction"""
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.WRITE_INSTR, 0, addr, 0, 4)
        self.ser.write(instr.to_bytes(4, 'big'))
        time.sleep(0.05)
        return True

    def execute(self, timeout=2.0):
        """Execute program and wait for completion"""
        self.ser.reset_input_buffer()
        # EXECUTE command only needs command byte - extra bytes cause buffer pollution
        self.ser.write(bytes([Cmd.EXECUTE]))
        time.sleep(0.05)
        start = time.time()
        while time.time() - start < timeout:
            self.ser.reset_input_buffer()  # Clear any pending bytes
            # READ_STATUS only needs command byte
            self.ser.write(bytes([Cmd.READ_STATUS]))
            time.sleep(0.005)  # Give FPGA time to respond
            status = self.ser.read(1)
            if status and (status[0] & 0x01) == 0:  # Not busy
                return True
            time.sleep(0.01)
        return False

    def encode_instruction(self, opcode, arg1=0, arg2=0, arg3=0, flags=0):
        """Encode instruction: [31:26]=opcode, [25:18]=arg1, [17:10]=arg2, [9:2]=arg3, [1:0]=flags"""
        return ((opcode & 0x3F) << 26) | ((arg1 & 0xFF) << 18) | ((arg2 & 0xFF) << 10) | ((arg3 & 0xFF) << 2) | (flags & 0x03)

# =============================================================================
# DIAGNOSTIC TESTS
# =============================================================================

def test1_ub_survives_execution(tpu):
    """
    TEST 1: Does UB data survive program execution?

    If this fails: The execute command or HALT is corrupting UB
    """
    print("\n" + "="*60)
    print("TEST 1: UB Data Survives Execution")
    print("="*60)

    # Write known pattern to UB[0]
    pattern = bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88] + [0]*24)
    tpu.write_ub(0, pattern)

    # Verify it's there
    readback1 = tpu.read_ub(0)
    print(f"  Before execution: {readback1[:8].hex()}")

    # Execute minimal program (just HALT)
    HALT = tpu.encode_instruction(0x3F)  # HALT opcode
    tpu.write_instruction(0, HALT)

    success = tpu.execute()
    print(f"  Execution: {'OK' if success else 'TIMEOUT'}")

    # Read back
    readback2 = tpu.read_ub(0)
    print(f"  After execution:  {readback2[:8].hex()}")

    if readback1[:8] == readback2[:8]:
        print("  ✓ PASS: UB data unchanged by execution")
        return True
    else:
        print("  ✗ FAIL: UB data corrupted by execution!")
        return False

def test2_st_ub_writes_to_ub(tpu):
    """
    TEST 2: Does ST_UB actually write to UB?

    If this fails: ST_UB instruction isn't triggering UB write
    """
    print("\n" + "="*60)
    print("TEST 2: ST_UB Writes to UB")
    print("="*60)

    # Write known pattern to UB[5] (destination)
    marker = bytes([0xDE, 0xAD, 0xBE, 0xEF] + [0]*28)
    tpu.write_ub(5, marker)

    # Verify marker is there
    before = tpu.read_ub(5)
    print(f"  UB[5] before ST_UB: {before[:8].hex()}")

    # Execute: ST_UB(5, 1, 0) then HALT
    # ST_UB opcode = 0x05, writes acc_data_out to UB[arg1]
    ST_UB = tpu.encode_instruction(0x05, arg1=5, arg2=1, arg3=0, flags=0)
    HALT = tpu.encode_instruction(0x3F)

    tpu.write_instruction(0, ST_UB)
    tpu.write_instruction(1, HALT)

    success = tpu.execute()
    print(f"  Execution: {'OK' if success else 'TIMEOUT'}")

    # Read back
    after = tpu.read_ub(5)
    print(f"  UB[5] after ST_UB:  {after[:8].hex()}")

    if before[:8] != after[:8]:
        print(f"  ✓ PASS: ST_UB modified UB[5]")
        print(f"  → acc_data_out value: {after[:8].hex()}")
        return True, after
    else:
        print("  ✗ FAIL: ST_UB did NOT modify UB[5]!")
        print("  → Check: ub_wr_en signal, ub_wr_addr, use_test_interface mux")
        return False, None

def test3_acc_latches_after_systolic(tpu):
    """
    TEST 3: Do accumulators change after MATMUL?

    Compare acc_data_out before and after MATMUL (no weights loaded)
    """
    print("\n" + "="*60)
    print("TEST 3: Accumulators Change After MATMUL")
    print("="*60)

    # First, get baseline acc_data_out (ST_UB without MATMUL)
    tpu.write_ub(10, bytes([0xAA]*32))  # Marker

    ST_UB = tpu.encode_instruction(0x05, arg1=10, arg2=1)
    HALT = tpu.encode_instruction(0x3F)
    tpu.write_instruction(0, ST_UB)
    tpu.write_instruction(1, HALT)
    tpu.execute()

    baseline = tpu.read_ub(10)
    print(f"  Baseline acc_data_out (no MATMUL): {baseline[:8].hex()}")

    # Now execute with MATMUL (but no weights/inputs loaded)
    tpu.write_ub(11, bytes([0xBB]*32))  # Marker

    # Load some dummy input to UB[0]
    tpu.write_ub(0, bytes([1, 2, 3] + [0]*29))

    # Program: MATMUL(0, 0, 3) -> ST_UB(11) -> HALT
    MATMUL = tpu.encode_instruction(0x10, arg1=0, arg2=0, arg3=3, flags=2)  # signed
    ST_UB2 = tpu.encode_instruction(0x05, arg1=11, arg2=1)

    tpu.write_instruction(0, MATMUL)
    tpu.write_instruction(1, ST_UB2)
    tpu.write_instruction(2, HALT)
    tpu.execute()

    after_matmul = tpu.read_ub(11)
    print(f"  After MATMUL (no weights):        {after_matmul[:8].hex()}")

    if baseline[:8] != after_matmul[:8]:
        print("  ✓ PASS: MATMUL changes accumulator outputs")
        return True
    else:
        print("  ✗ FAIL: MATMUL did NOT change accumulators")
        print("  → Check: systolic_active signal, acc latching logic")
        return False

def test4_weight_loading(tpu):
    """
    TEST 4: Are weights being loaded into PEs?

    Load identity weights, run MATMUL, check if output differs from zero weights
    """
    print("\n" + "="*60)
    print("TEST 4: Weight Loading Affects Output")
    print("="*60)

    # Test A: MATMUL with ZERO weights
    print("\n  Test 4A: Zero weights...")
    for i in range(3):
        tpu.write_weight(i, bytes([0, 0, 0, 0, 0, 0, 0, 0]))

    tpu.write_ub(0, bytes([10, 20, 30] + [0]*29))  # Input
    tpu.write_ub(20, bytes([0xCC]*32))  # Marker

    # RD_WEIGHT x3, LD_UB, MATMUL, ST_UB, HALT
    RD_WT0 = tpu.encode_instruction(0x03, arg1=0, arg2=1)
    RD_WT1 = tpu.encode_instruction(0x03, arg1=1, arg2=1)
    RD_WT2 = tpu.encode_instruction(0x03, arg1=2, arg2=1)
    LD_UB = tpu.encode_instruction(0x04, arg1=0, arg2=1)
    MATMUL = tpu.encode_instruction(0x10, arg1=0, arg2=0, arg3=3, flags=2)
    ST_UB = tpu.encode_instruction(0x05, arg1=20, arg2=1)
    HALT = tpu.encode_instruction(0x3F)

    for i, instr in enumerate([RD_WT0, RD_WT1, RD_WT2, LD_UB, MATMUL, ST_UB, HALT]):
        tpu.write_instruction(i, instr)

    tpu.execute()
    zero_wt_result = tpu.read_ub(20)
    print(f"    Result with zero weights: {zero_wt_result[:8].hex()}")

    # Test B: MATMUL with IDENTITY weights
    print("\n  Test 4B: Identity weights...")
    tpu.write_weight(0, bytes([1, 0, 0, 0, 0, 0, 0, 0]))  # Row 0: [1,0,0]
    tpu.write_weight(1, bytes([0, 1, 0, 0, 0, 0, 0, 0]))  # Row 1: [0,1,0]
    tpu.write_weight(2, bytes([0, 0, 1, 0, 0, 0, 0, 0]))  # Row 2: [0,0,1]

    tpu.write_ub(0, bytes([10, 20, 30] + [0]*29))  # Input
    tpu.write_ub(21, bytes([0xDD]*32))  # Marker

    ST_UB2 = tpu.encode_instruction(0x05, arg1=21, arg2=1)
    for i, instr in enumerate([RD_WT0, RD_WT1, RD_WT2, LD_UB, MATMUL, ST_UB2, HALT]):
        tpu.write_instruction(i, instr)

    tpu.execute()
    identity_wt_result = tpu.read_ub(21)
    print(f"    Result with identity weights: {identity_wt_result[:8].hex()}")

    if zero_wt_result[:8] != identity_wt_result[:8]:
        print("\n  ✓ PASS: Different weights produce different outputs")
        print(f"    → Weights ARE reaching the systolic array")
        return True
    else:
        print("\n  ✗ FAIL: Weights don't affect output!")
        print("  → Check: RD_WEIGHT, wt_fifo_wr, wt_rd_en, weight FIFO pop timing")
        return False

def test5_activation_loading(tpu):
    """
    TEST 5: Are activations being loaded from UB?

    Same weights, different inputs - output should differ
    """
    print("\n" + "="*60)
    print("TEST 5: Activation Loading Affects Output")
    print("="*60)

    # Load identity weights
    tpu.write_weight(0, bytes([1, 0, 0, 0, 0, 0, 0, 0]))
    tpu.write_weight(1, bytes([0, 1, 0, 0, 0, 0, 0, 0]))
    tpu.write_weight(2, bytes([0, 0, 1, 0, 0, 0, 0, 0]))

    # Test A: Input [1, 1, 1]
    print("\n  Test 5A: Input [1, 1, 1]...")
    tpu.write_ub(0, bytes([1, 1, 1] + [0]*29))
    tpu.write_ub(30, bytes([0xEE]*32))

    RD_WT0 = tpu.encode_instruction(0x03, arg1=0, arg2=1)
    RD_WT1 = tpu.encode_instruction(0x03, arg1=1, arg2=1)
    RD_WT2 = tpu.encode_instruction(0x03, arg1=2, arg2=1)
    LD_UB = tpu.encode_instruction(0x04, arg1=0, arg2=1)
    MATMUL = tpu.encode_instruction(0x10, arg1=0, arg2=0, arg3=3, flags=2)
    ST_UB = tpu.encode_instruction(0x05, arg1=30, arg2=1)
    HALT = tpu.encode_instruction(0x3F)

    for i, instr in enumerate([RD_WT0, RD_WT1, RD_WT2, LD_UB, MATMUL, ST_UB, HALT]):
        tpu.write_instruction(i, instr)

    tpu.execute()
    result_a = tpu.read_ub(30)
    print(f"    Result: {result_a[:8].hex()}")

    # Test B: Input [5, 10, 15]
    print("\n  Test 5B: Input [5, 10, 15]...")
    tpu.write_ub(0, bytes([5, 10, 15] + [0]*29))
    tpu.write_ub(31, bytes([0xFF]*32))

    ST_UB2 = tpu.encode_instruction(0x05, arg1=31, arg2=1)
    for i, instr in enumerate([RD_WT0, RD_WT1, RD_WT2, LD_UB, MATMUL, ST_UB2, HALT]):
        tpu.write_instruction(i, instr)

    tpu.execute()
    result_b = tpu.read_ub(31)
    print(f"    Result: {result_b[:8].hex()}")

    if result_a[:8] != result_b[:8]:
        print("\n  ✓ PASS: Different inputs produce different outputs")
        print(f"    → Activations ARE reaching the systolic array")
        return True
    else:
        print("\n  ✗ FAIL: Inputs don't affect output!")
        print("  → Check: LD_UB, ub_rd_en, ub_rd_data path to systolic array")
        return False

def test6_ub_bank_during_isa(tpu):
    """
    TEST 6: Bank selection during ISA execution

    Check if ST_UB writes to a bank that UART can read
    """
    print("\n" + "="*60)
    print("TEST 6: UB Bank Selection During ISA")
    print("="*60)

    # Write markers to BOTH possible destination addresses
    tpu.write_ub(40, bytes([0xAA]*32))  # If ST_UB uses bank 0
    tpu.write_ub(40, bytes([0xBB]*32))  # Overwrite (same addr)

    print(f"  Wrote 0xBB pattern to UB[40]")

    # Execute ST_UB to address 40
    ST_UB = tpu.encode_instruction(0x05, arg1=40, arg2=1)
    HALT = tpu.encode_instruction(0x3F)
    tpu.write_instruction(0, ST_UB)
    tpu.write_instruction(1, HALT)
    tpu.execute()

    # Read back
    result = tpu.read_ub(40)
    print(f"  UB[40] after ST_UB: {result[:8].hex()}")

    if result[:8] == bytes([0xBB]*8):
        print("  → ST_UB did NOT modify this address (bank mismatch?)")
        print("  ✗ FAIL: Bank selection issue - ST_UB writes to different bank than UART reads")
        return False
    else:
        print(f"  → ST_UB wrote: {result[:8].hex()}")
        print("  ✓ PASS: UART can read ST_UB output")
        return True

def test7_raw_accumulator_values(tpu):
    """
    TEST 7: What are the raw accumulator values?

    Execute ST_UB multiple times at different points to see accumulator state
    """
    print("\n" + "="*60)
    print("TEST 7: Raw Accumulator Values at Different Stages")
    print("="*60)

    # Load identity weights and input
    tpu.write_weight(0, bytes([1, 0, 0, 0, 0, 0, 0, 0]))
    tpu.write_weight(1, bytes([0, 1, 0, 0, 0, 0, 0, 0]))
    tpu.write_weight(2, bytes([0, 0, 1, 0, 0, 0, 0, 0]))
    tpu.write_ub(0, bytes([10, 20, 30] + [0]*29))

    # Clear destination addresses
    for addr in [50, 51, 52]:
        tpu.write_ub(addr, bytes([0]*32))

    # Program with ST_UB at multiple points
    RD_WT0 = tpu.encode_instruction(0x03, arg1=0, arg2=1)
    RD_WT1 = tpu.encode_instruction(0x03, arg1=1, arg2=1)
    RD_WT2 = tpu.encode_instruction(0x03, arg1=2, arg2=1)
    ST_UB_50 = tpu.encode_instruction(0x05, arg1=50, arg2=1)  # After weight load
    LD_UB = tpu.encode_instruction(0x04, arg1=0, arg2=1)
    ST_UB_51 = tpu.encode_instruction(0x05, arg1=51, arg2=1)  # After LD_UB
    MATMUL = tpu.encode_instruction(0x10, arg1=0, arg2=0, arg3=3, flags=2)
    ST_UB_52 = tpu.encode_instruction(0x05, arg1=52, arg2=1)  # After MATMUL
    HALT = tpu.encode_instruction(0x3F)

    program = [RD_WT0, RD_WT1, RD_WT2, ST_UB_50, LD_UB, ST_UB_51, MATMUL, ST_UB_52, HALT]
    for i, instr in enumerate(program):
        tpu.write_instruction(i, instr)

    tpu.execute()

    r50 = tpu.read_ub(50)
    r51 = tpu.read_ub(51)
    r52 = tpu.read_ub(52)

    print(f"  After RD_WEIGHT x3:  {r50[:8].hex()} → {list(r50[:3])}")
    print(f"  After LD_UB:         {r51[:8].hex()} → {list(r51[:3])}")
    print(f"  After MATMUL:        {r52[:8].hex()} → {list(r52[:3])}")

    # Interpret as signed int8
    import struct
    def to_signed(b):
        return [struct.unpack('b', bytes([x]))[0] for x in b[:3]]

    print(f"\n  As signed int8:")
    print(f"    After RD_WEIGHT: {to_signed(r50)}")
    print(f"    After LD_UB:     {to_signed(r51)}")
    print(f"    After MATMUL:    {to_signed(r52)}")
    print(f"    Expected:        [10, 20, 30]")

    return r52

# =============================================================================
# MAIN
# =============================================================================

def main():
    if len(sys.argv) < 2:
        print("Usage: python diagnostic_isolated.py <serial_port>")
        print("Example: python diagnostic_isolated.py /dev/tty.usbserial-210183A27BE01")
        sys.exit(1)

    port = sys.argv[1]
    print(f"\nConnecting to TPU on {port}...")
    tpu = TPUDebug(port)
    print("Connected!\n")

    results = {}

    # Run tests in order
    results['test1'] = test1_ub_survives_execution(tpu)
    results['test2'] = test2_st_ub_writes_to_ub(tpu)
    results['test3'] = test3_acc_latches_after_systolic(tpu)
    results['test4'] = test4_weight_loading(tpu)
    results['test5'] = test5_activation_loading(tpu)
    results['test6'] = test6_ub_bank_during_isa(tpu)
    results['test7'] = test7_raw_accumulator_values(tpu)

    # Summary
    print("\n" + "="*60)
    print("DIAGNOSTIC SUMMARY")
    print("="*60)

    test_names = {
        'test1': 'UB survives execution',
        'test2': 'ST_UB writes to UB',
        'test3': 'MATMUL changes accumulators',
        'test4': 'Weights affect output',
        'test5': 'Activations affect output',
        'test6': 'Bank selection correct',
        'test7': 'Raw accumulator trace'
    }

    for key, name in test_names.items():
        if key == 'test7':
            status = "DATA" if results[key] is not None else "FAIL"
        elif isinstance(results[key], tuple):
            status = "PASS" if results[key][0] else "FAIL"
        else:
            status = "PASS" if results[key] else "FAIL"
        print(f"  {key}: {status:4s} - {name}")

    print("\n" + "="*60)
    print("INTERPRETATION GUIDE")
    print("="*60)
    print("""
  If TEST 2 fails: ST_UB not writing → check ub_wr_en, use_test_interface mux
  If TEST 3 fails: MATMUL not running → check systolic_active, sys_start
  If TEST 4 fails: Weights not loaded → check wt_rd_en timing (our recent fix)
  If TEST 5 fails: Activations not loaded → check ub_rd_en, ub_rd_data path
  If TEST 6 fails: Bank mismatch → check ub_buf_sel during ISA execution

  TEST 7 shows accumulator state at each stage - compare to expected values
    """)

if __name__ == "__main__":
    main()
