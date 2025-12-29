#!/usr/bin/env python3
"""
Detailed debugging of the systolic array timing and data flow.
Tests specific timing scenarios to identify where the bug is.
"""

import serial
import sys
import time
import struct

def connect_tpu(port):
    ser = serial.Serial(port, 115200, timeout=1.0)
    time.sleep(0.1)
    ser.reset_input_buffer()
    return ser

def send_command(ser, cmd, data=b''):
    packet = bytes([cmd]) + data
    ser.write(packet)
    time.sleep(0.05)

def read_response(ser, expected_len):
    data = ser.read(expected_len)
    return data

def write_ub(ser, addr, data_bytes):
    """Write 8 bytes to UB at addr"""
    # Command 0x01: Write UB
    send_command(ser, 0x01, bytes([addr]) + data_bytes[:8].ljust(8, b'\x00'))
    time.sleep(0.02)

def read_ub(ser, addr):
    """Read 8 bytes from UB at addr"""
    # Command 0x02: Read UB
    send_command(ser, 0x02, bytes([addr]))
    time.sleep(0.02)
    return read_response(ser, 8)

def write_weight(ser, addr, data_bytes):
    """Write weight row (8 bytes) to weight memory at addr"""
    # Command 0x03: Write Weight
    send_command(ser, 0x03, bytes([addr]) + data_bytes[:8].ljust(8, b'\x00'))
    time.sleep(0.02)

def write_instruction(ser, instr_bytes):
    """Write 4-byte instruction to instruction memory"""
    # Command 0x05: Write Instruction
    send_command(ser, 0x05, instr_bytes[:4])
    time.sleep(0.02)

def execute(ser):
    """Execute loaded program"""
    # Command 0x06: Execute
    send_command(ser, 0x06)
    time.sleep(0.1)  # Wait for execution

def reset_tpu(ser):
    """Reset TPU"""
    # Command 0x09: Reset
    send_command(ser, 0x09)
    time.sleep(0.1)

def make_instruction(opcode, addr, size, acc_addr, flags=0):
    """Create a 32-bit instruction"""
    # Format: [opcode:5][addr:8][size:8][acc_addr:8][flags:3]
    instr = (opcode << 27) | (addr << 19) | (size << 11) | (acc_addr << 3) | flags
    return struct.pack('>I', instr)

# Opcodes
OP_NOP = 0
OP_RD_WEIGHT = 1
OP_MATMUL = 2
OP_LD_UB = 3
OP_ST_UB = 4
OP_HALT = 7

def test_weight_capture_timing(ser):
    """Test which weight values actually get captured"""
    print("\n" + "="*60)
    print("TEST: Weight Capture Timing Analysis")
    print("="*60)

    reset_tpu(ser)

    # Load distinct weights so we can identify which one was captured
    # Weight row 0: [0xAA, 0xBB, 0xCC, ...]
    # Weight row 1: [0x11, 0x22, 0x33, ...]
    # Weight row 2: [0x44, 0x55, 0x66, ...]
    print("\nLoading distinct weight patterns...")
    write_weight(ser, 0, bytes([0xAA, 0xBB, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00]))
    write_weight(ser, 1, bytes([0x11, 0x22, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00]))
    write_weight(ser, 2, bytes([0x44, 0x55, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00]))

    # Load input activations: [1, 1, 1, 0, 0, 0, 0, 0]
    # Using all 1s to see the raw weight values in output
    print("Loading activations: [1, 1, 1]")
    write_ub(ser, 0, bytes([0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00]))

    # Read back to verify
    ub_data = read_ub(ser, 0)
    print(f"  UB[0] = {ub_data.hex()}")

    # Program: RD_WEIGHT x3, LD_UB, MATMUL, ST_UB, HALT
    print("\nLoading program...")
    instructions = [
        make_instruction(OP_RD_WEIGHT, 0, 1, 0),  # Load weight row 0
        make_instruction(OP_RD_WEIGHT, 1, 1, 0),  # Load weight row 1
        make_instruction(OP_RD_WEIGHT, 2, 1, 0),  # Load weight row 2
        make_instruction(OP_LD_UB, 0, 1, 0),      # Load activations
        make_instruction(OP_MATMUL, 0, 0, 3, 2),  # MATMUL, 3 rows, flags=2 (signed)
        make_instruction(OP_ST_UB, 1, 1, 0),      # Store result to UB[1]
        make_instruction(OP_HALT, 0, 0, 0),
    ]
    for instr in instructions:
        write_instruction(ser, instr)

    print("Executing...")
    execute(ser)

    # Read result
    result = read_ub(ser, 1)
    print(f"\nResult at UB[1]: {result.hex()}")

    # Parse the accumulator outputs
    if len(result) >= 3:
        acc0 = result[0]
        acc1 = result[1]
        acc2 = result[2]
        print(f"\nAccumulator outputs:")
        print(f"  acc0 = 0x{acc0:02X} ({acc0})")
        print(f"  acc1 = 0x{acc1:02X} ({acc1})")
        print(f"  acc2 = 0x{acc2:02X} ({acc2})")

        # Analysis: With activations [1,1,1] and identity PE weights:
        # acc0 = pe00*1 + pe10*1 + pe20*1 = sum of column 0 weights
        # acc1 = pe01*1 + pe11*1 + pe21*1 = sum of column 1 weights
        # acc2 = pe02*1 + pe12*1 + pe22*1 = sum of column 2 weights

        print("\nAnalysis:")
        print("  If weights loaded correctly (identity):")
        print("    Expected: [0xAA+0x11+0x44, 0xBB+0x22+0x55, 0xCC+0x33+0x66]")
        print(f"    Expected: [0x{0xAA+0x11+0x44:02X}, 0x{0xBB+0x22+0x55:02X}, 0x{0xCC+0x33+0x66:02X}]")

        # For identity matrix (if only diagonal PEs captured):
        print("  If identity matrix captured:")
        print(f"    Column 0 sum = 0xAA (if pe00=0xAA, pe10=0, pe20=0)")
        print(f"    Column 1 sum = 0x22 (if pe01=0, pe11=0x22, pe21=0)")
        print(f"    Column 2 sum = 0x66 (if pe02=0, pe12=0, pe22=0x66)")

def test_simple_multiply(ser):
    """Test with known values to verify MAC operation"""
    print("\n" + "="*60)
    print("TEST: Simple Multiply-Accumulate")
    print("="*60)

    reset_tpu(ser)

    # Load weight = 2 for all PEs (column-wise)
    # If W = [[2,0,0], [0,2,0], [0,0,2]] (identity * 2)
    print("\nLoading weights: [[2,0,0], [0,2,0], [0,0,2]]")
    write_weight(ser, 0, bytes([0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]))
    write_weight(ser, 1, bytes([0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]))
    write_weight(ser, 2, bytes([0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00]))

    # Load input: [3, 5, 7]
    print("Loading activations: [3, 5, 7]")
    write_ub(ser, 0, bytes([0x03, 0x05, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00]))

    # Program
    instructions = [
        make_instruction(OP_RD_WEIGHT, 0, 1, 0),
        make_instruction(OP_RD_WEIGHT, 1, 1, 0),
        make_instruction(OP_RD_WEIGHT, 2, 1, 0),
        make_instruction(OP_LD_UB, 0, 1, 0),
        make_instruction(OP_MATMUL, 0, 0, 3, 0),  # unsigned
        make_instruction(OP_ST_UB, 1, 1, 0),
        make_instruction(OP_HALT, 0, 0, 0),
    ]
    for instr in instructions:
        write_instruction(ser, instr)

    print("Executing...")
    execute(ser)

    result = read_ub(ser, 1)
    print(f"\nResult: {result.hex()}")

    if len(result) >= 3:
        print(f"  acc0 = {result[0]} (expected: 3*2 = 6)")
        print(f"  acc1 = {result[1]} (expected: 5*2 = 10)")
        print(f"  acc2 = {result[2]} (expected: 7*2 = 14)")

def test_fifo_skew_timing(ser):
    """Test if FIFO column skewing works correctly"""
    print("\n" + "="*60)
    print("TEST: FIFO Column Skew Timing")
    print("="*60)

    reset_tpu(ser)

    # Load weights with clear column markers
    # Row 0: col0=0x10, col1=0x20, col2=0x30
    # Row 1: col0=0x11, col1=0x21, col2=0x31
    # Row 2: col0=0x12, col1=0x22, col2=0x32
    print("\nLoading weights with column markers...")
    write_weight(ser, 0, bytes([0x10, 0x20, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]))
    write_weight(ser, 1, bytes([0x11, 0x21, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00]))
    write_weight(ser, 2, bytes([0x12, 0x22, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00]))

    # Load input [1, 1, 1]
    write_ub(ser, 0, bytes([0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00]))

    instructions = [
        make_instruction(OP_RD_WEIGHT, 0, 1, 0),
        make_instruction(OP_RD_WEIGHT, 1, 1, 0),
        make_instruction(OP_RD_WEIGHT, 2, 1, 0),
        make_instruction(OP_LD_UB, 0, 1, 0),
        make_instruction(OP_MATMUL, 0, 0, 3, 0),
        make_instruction(OP_ST_UB, 1, 1, 0),
        make_instruction(OP_HALT, 0, 0, 0),
    ]
    for instr in instructions:
        write_instruction(ser, instr)

    print("Executing...")
    execute(ser)

    result = read_ub(ser, 1)
    print(f"\nResult: {result.hex()}")

    if len(result) >= 3:
        acc0, acc1, acc2 = result[0], result[1], result[2]
        print(f"\nColumn sums (with 1,1,1 input):")
        print(f"  acc0 = 0x{acc0:02X} = {acc0}")
        print(f"  acc1 = 0x{acc1:02X} = {acc1}")
        print(f"  acc2 = 0x{acc2:02X} = {acc2}")

        print("\nExpected if all weights captured correctly:")
        print(f"  acc0 = 0x10+0x11+0x12 = 0x{0x10+0x11+0x12:02X} = {0x10+0x11+0x12}")
        print(f"  acc1 = 0x20+0x21+0x22 = 0x{0x20+0x21+0x22:02X} = {0x20+0x21+0x22}")
        print(f"  acc2 = 0x30+0x31+0x32 = 0x{0x30+0x31+0x32:02X} = {0x30+0x31+0x32}")

def test_data_movement_only(ser):
    """Test LD_UB and ST_UB without MATMUL"""
    print("\n" + "="*60)
    print("TEST: Data Movement Only (LD_UB -> ST_UB)")
    print("="*60)

    reset_tpu(ser)

    # Write test pattern
    print("\nWriting 0x1122334455667788 to UB[0]...")
    write_ub(ser, 0, bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88]))

    # Verify write
    data = read_ub(ser, 0)
    print(f"  UB[0] = {data.hex()}")

    # Program: LD_UB(0), ST_UB(1), HALT
    print("\nProgram: LD_UB(0), ST_UB(1), HALT")
    instructions = [
        make_instruction(OP_LD_UB, 0, 1, 0),
        make_instruction(OP_ST_UB, 1, 1, 0),
        make_instruction(OP_HALT, 0, 0, 0),
    ]
    for instr in instructions:
        write_instruction(ser, instr)

    print("Executing...")
    execute(ser)

    # Check result
    result = read_ub(ser, 1)
    print(f"\nUB[1] after execution: {result.hex()}")

    # Note: ST_UB writes acc_data_out, not ub_rd_data
    print("\nNote: ST_UB writes accumulator output, not UB data")
    print("This test shows what's in the accumulator path")

def test_weight_only(ser):
    """Load weights and read them back (if possible)"""
    print("\n" + "="*60)
    print("TEST: Weight Loading Only")
    print("="*60)

    reset_tpu(ser)

    print("\nLoading weight 0xABCDEF12 to addr 0...")
    write_weight(ser, 0, bytes([0xAB, 0xCD, 0xEF, 0x12, 0x00, 0x00, 0x00, 0x00]))

    # Execute RD_WEIGHT only, then check accumulator output
    print("\nProgram: RD_WEIGHT(0), MATMUL (1 row), ST_UB(0)")
    instructions = [
        make_instruction(OP_RD_WEIGHT, 0, 1, 0),  # Load 1 weight row
        make_instruction(OP_LD_UB, 0, 1, 0),      # Need some activation
        make_instruction(OP_MATMUL, 0, 0, 1, 0),  # 1 row MATMUL
        make_instruction(OP_ST_UB, 0, 1, 0),
        make_instruction(OP_HALT, 0, 0, 0),
    ]

    # First write a known activation
    write_ub(ser, 0, bytes([0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00]))

    for instr in instructions:
        write_instruction(ser, instr)

    print("Executing...")
    execute(ser)

    result = read_ub(ser, 0)
    print(f"\nResult: {result.hex()}")
    print(f"  With input [1,1,1], output shows captured weights")

def main():
    if len(sys.argv) < 2:
        print("Usage: python detailed_systolic_debug.py <serial_port>")
        sys.exit(1)

    port = sys.argv[1]
    print(f"Connecting to TPU on {port}...")

    try:
        ser = connect_tpu(port)
        print("Connected!")

        test_data_movement_only(ser)
        test_weight_only(ser)
        test_simple_multiply(ser)
        test_weight_capture_timing(ser)
        test_fifo_skew_timing(ser)

        ser.close()
        print("\nDone!")

    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
