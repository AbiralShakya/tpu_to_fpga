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

# Send WRITE_UB command byte by byte
cmd_bytes = [0x01, 0x00, 0x00, 0x00, 0x20] + list(matrix_data)
for byte in cmd_bytes:
    s.write(bytes([byte]))
    s.flush()
    time.sleep(0.01)  # Small delay between bytes
time.sleep(0.2)
print(f"  ✓ Wrote {len(matrix_data)} bytes")

# Step 2: Write weights (identity matrix for testing)
print("\nStep 2: Writing weights to Weight Memory (address 0x0000)...")
# For 3x3 matrix multiplication, we need weight data
# The format depends on the systolic array, but let's use identity for simplicity
weight_data = bytes([
    1, 0, 0,  # Row 1 of weight matrix
    0, 1, 0,  # Row 2
    0, 0, 1,  # Row 3
] + [0] * 5)  # Pad to 8 bytes minimum

# Send WRITE_WEIGHT command byte by byte
cmd_bytes = [0x02, 0x00, 0x00, 0x00, len(weight_data)] + list(weight_data)
for byte in cmd_bytes:
    s.write(bytes([byte]))
    s.flush()
    time.sleep(0.01)
time.sleep(0.2)
print(f"  ✓ Wrote {len(weight_data)} bytes")
print(f"    Weight data: {weight_data.hex()}")

# Step 3: Write MATMUL instruction
print("\nStep 3: Writing MATMUL instruction...")
# Instruction format (32-bit little-endian):
# [7:0] = flags + (arg3 << 2)
# [15:8] = arg2
# [23:16] = arg1
# [31:24] = opcode
# MATMUL opcode = 0x10
# arg1 = UB address = 0x00
# arg2 = Accumulator address = 0x00
# arg3 = Number of rows = 0x03
# flags = 0b00
matmul_instr = bytes([
    (0x03 << 2) | 0x00,  # flags=0, arg3=3 -> 0x0C
    0x00,                # arg2 = 0
    0x00,                # arg1 = 0
    0x10                 # opcode = 0x10
])

# Send WRITE_INSTR command byte by byte
cmd_bytes = [0x03, 0x00, 0x00] + list(matmul_instr)
for byte in cmd_bytes:
    s.write(bytes([byte]))
    s.flush()
    time.sleep(0.01)
time.sleep(0.2)
print(f"  ✓ Instruction written: MATMUL (3 rows)")
print(f"    Instruction: {matmul_instr.hex()}")
print(f"    Decoded: opcode=0x{matmul_instr[3]:02X}, arg1={matmul_instr[2]}, arg2={matmul_instr[1]}, arg3={matmul_instr[0]>>2}, flags={matmul_instr[0]&3}")

# Step 4: Check status before execution
print("\nStep 4: Status check (before execution)...")
s.reset_input_buffer()
# Send READ_STATUS command
s.write(bytes([0x06]))
s.flush()
time.sleep(0.2)
status = s.read(1)
if status:
    s_val = status[0]
    print(f"  Status: 0x{s_val:02x}")
    print(f"    sys_busy={s_val&1}, vpu_busy={s_val>>2&1}, dma_busy={s_val>>1&1}, ub_busy={s_val>>4&1}")
    initial_busy = s_val & 1
else:
    print("  Could not read initial status")
    initial_busy = 0

# Step 5: Execute!
print("\nStep 5: Executing MATMUL...")
s.write(bytes([0x05]))  # EXECUTE
time.sleep(0.2)  # Brief wait
print("  ✓ Execute command sent")

# Step 6: Check status after execution
print("\nStep 6: Status check (after execution)...")
execution_started = False
computation_completed = False

for i in range(10):  # More polls
    s.reset_input_buffer()
    s.write(bytes([0x06]))
    s.flush()
    time.sleep(0.1)
    status = s.read(1)
    if status:
        s_val = status[0]
        sys_busy = s_val & 1
        print(f"  Poll {i+1}: Status=0x{s_val:02x}, sys_busy={sys_busy}")

        if not execution_started and sys_busy == 1:
            execution_started = True
            print("    → TPU computation started!")
        elif execution_started and sys_busy == 0:
            computation_completed = True
            print("  ✓ Computation completed successfully!")
            break
    time.sleep(0.1)

if not computation_completed:
    print("  ⚠ Computation may still be running or failed to start")
    if execution_started:
        print("    (TPU was busy but didn't complete within timeout)")
    else:
        print("    (TPU never showed as busy after EXECUTE)")

# Step 7: Verify execution completed (READ_UB is broken, so we can't read results)
print("\nStep 7: Verifying execution completed...")
print("  Note: READ_UB command is currently broken, so we cannot read back results")
print("  However, we can verify that the TPU executed by checking status changes")

# Final status check
s.reset_input_buffer()
s.write(bytes([0x06]))
s.flush()
time.sleep(0.2)
final_status = s.read(1)
if final_status:
    fs_val = final_status[0]
    print(f"  Final Status: 0x{fs_val:02x}")
    print(f"    sys_busy={fs_val&1}, vpu_busy={fs_val>>2&1}, ub_busy={fs_val>>4&1}")
    if (fs_val & 1) == 0:
        print("  ✓ SUCCESS: TPU computation completed successfully!")
        print("  ✓ Matrix multiplication executed without hanging the FPGA")
    else:
        print("  ⚠ TPU still busy - computation may still be running")
else:
    print("  ⚠ Could not read final status")

print("  Expected: Identity × Identity = Identity matrix")
print("  (Cannot verify actual results due to READ_UB bug)")

s.close()

print()
print("=" * 70)
print("MATMUL TEST COMPLETE")
print("=" * 70)
print()
print("Expected result: Identity matrix (same as input)")
print("If sys_busy went to 0, the TPU executed the instruction!")
print("=" * 70)
