import serial, time, sys

port = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
s = serial.Serial(port, 115200, timeout=1)

print("TPU 3x3 Matrix Multiplication Test")
print("=" * 50)

# Write a simple 3x3 identity-like matrix to UB
print("\n1. Writing matrix data to UB...")
s.write(bytes([0x01]))  # WRITE_UB
matrix = bytes([1,0,0, 0,1,0, 0,0,1] + [0]*23)  # 3x3 identity + padding
s.write(matrix)
time.sleep(0.2)
print("  ✓ Matrix written")

# Write MATMUL instruction (opcode 0x10)
print("\n2. Writing MATMUL instruction...")
s.write(bytes([0x03]))  # WRITE_INSTR
# Instruction: [opcode=0x10][arg1=0][arg2=0][arg3=3][flags=0b00]
instr = bytes([0x10, 0x00, 0x00, 0x0C])  # 0x10000C00 = MATMUL 3 rows
s.write(instr)
time.sleep(0.2)
print("  ✓ MATMUL instruction written")

# Execute
print("\n3. Executing MATMUL...")
s.write(bytes([0x05]))  # EXECUTE
time.sleep(0.5)  # Give time for computation
print("  ✓ Execution started")

# Check status
print("\n4. Checking status...")
s.reset_input_buffer()
s.write(bytes([0x06]))
time.sleep(10)
status = s.read(1)
if status:
    print(f"  Status: 0x{status[0]:02x}")
    if status[0] & 0x01:
        print("  ⚠ sys_busy=1 (still computing)")
    else:
        print("  ✓ Computation complete!")

s.close()
print("\n" + "=" * 50)
print("If status shows sys_busy=0, MATMUL executed!")
print("=" * 50)