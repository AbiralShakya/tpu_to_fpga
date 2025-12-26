#!/usr/bin/env python3
"""
TPU Matrix Multiplication Test
Tests a simple 3x3 identity matrix multiplication
"""
import serial, time, sys

port = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
s = serial.Serial(port, 115200, timeout=1)

print("=" * 70)
print("TPU 3x3 MATRIX MULTIPLICATION TEST")
print("=" * 70)

# Test: Multiply 3x3 identity matrix
# Expected result: Should get back identity matrix
print("\nTest Matrix: 3x3 Identity")
print("  [1 0 0]")
print("  [0 1 0]")
print("  [0 0 1]")
print()

# Step 1: Write matrix data to Unified Buffer
print("Step 1: Writing matrix to Unified Buffer (address 0x0000)...")
matrix_data = bytes([
    1, 0, 0,  # Row 1
    0, 1, 0,  # Row 2
    0, 0, 1,  # Row 3
] + [0] * 23)  # Pad to 32 bytes

cmd = bytes([
    0x01,        # WRITE_UB
    0x00, 0x00,  # address = 0x0000
    0x00, 0x20,  # length = 32 bytes
]) + matrix_data
s.write(cmd)
time.sleep(0.3)
print(f"  ✓ Wrote {len(matrix_data)} bytes")

# Step 2: Write weights (same identity matrix)
print("\nStep 2: Writing weights to Weight Memory (address 0x0000)...")
weight_data = bytes([
    1, 1,1,1,1,1,1,1,  # First 8 bytes
])
cmd = bytes([
    0x02,        # WRITE_WEIGHT
    0x00, 0x00,  # address = 0x0000
    0x00, 0x08,  # length = 8 bytes
]) + weight_data
s.write(cmd)
time.sleep(0.3)
print(f"  ✓ Wrote {len(weight_data)} bytes")

# Step 3: Write MATMUL instruction
print("\nStep 3: Writing MATMUL instruction...")
# Instruction format: [opcode][arg1][arg2][arg3][flags]
# MATMUL opcode = 0x10
# arg1 = UB address = 0x00
# arg2 = Accumulator address = 0x00
# arg3 = Number of rows = 0x03
# flags = 0b00 (unsigned, no transpose)
matmul_instr = bytes([
    0x40, 0x00, 0x00, 0x0C  # 0x10 << 26 | 0x03 << 2 = big-endian encoding
])
# Actually, let's use little-endian (LSB first)
matmul_instr = bytes([
    0x0C, 0x00, 0x00, 0x40  # flags=0, arg3=3, arg2=0, arg1=0, opcode=0x10
])

cmd = bytes([
    0x03,        # WRITE_INSTR
    0x00, 0x00,  # address = 0x0000
]) + matmul_instr
s.write(cmd)
time.sleep(0.3)
print(f"  ✓ Instruction written: MATMUL (3 rows)")
print(f"    Opcode: 0x10, Args: [0, 0, 3], Flags: 0b00")

# Step 4: Check status before execution
print("\nStep 4: Status check (before execution)...")
s.reset_input_buffer()
s.write(bytes([0x06]))
time.sleep(0.2)
status = s.read(1)
if status:
    s_val = status[0]
    print(f"  Status: 0x{s_val:02x}")
    print(f"    sys_busy={s_val&1}, vpu_busy={s_val>>2&1}, ub_busy={s_val>>4&1}")

# Step 5: Execute!
print("\nStep 5: Executing MATMUL...")
s.write(bytes([0x05]))  # EXECUTE
time.sleep(0.5)  # Give time for computation
print("  ✓ Execute command sent")

# Step 6: Check status after execution
print("\nStep 6: Status check (after execution)...")
for i in range(5):
    s.reset_input_buffer()
    s.write(bytes([0x06]))
    time.sleep(0.1)
    status = s.read(1)
    if status:
        s_val = status[0]
        print(f"  Poll {i+1}: Status=0x{s_val:02x}, sys_busy={s_val&1}")
        if (s_val & 1) == 0:
            print("  ✓ Computation complete!")
            break
    time.sleep(0.1)

# Step 7: Read back results from accumulator via Unified Buffer
print("\nStep 7: Reading results from Unified Buffer...")
cmd = bytes([
    0x04,        # READ_UB
    0x00, 0x00,  # address = 0x0000
    0x00, 0x20,  # length = 32 bytes
])
s.reset_input_buffer()
s.write(cmd)
time.sleep(0.5)
result = s.read(32)
if result and len(result) == 32:
    print(f"  ✓ Received {len(result)} bytes")
    print(f"  Result (first 16 bytes): {result[:16].hex()}")
    print(f"  Result matrix (3x3):")
    for i in range(3):
        row = result[i*3:(i+1)*3]
        print(f"    Row {i}: [{row[0]:3d} {row[1]:3d} {row[2]:3d}]")
else:
    print(f"  ⚠ Only received {len(result) if result else 0} bytes")

s.close()

print()
print("=" * 70)
print("MATMUL TEST COMPLETE")
print("=" * 70)
print()
print("Expected result: Identity matrix (same as input)")
print("If sys_busy went to 0, the TPU executed the instruction!")
print("=" * 70)
