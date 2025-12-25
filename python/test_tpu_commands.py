import serial, time, sys

port = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
s = serial.Serial(port, 115200, timeout=1)

print("TPU UART Test (Correct Protocol)")
print("=" * 60)

# Test 1: READ_STATUS
print("\n1. READ_STATUS")
s.reset_input_buffer()
s.write(bytes([0x06]))
time.sleep(0.2)
resp = s.read(1)
print(f"   Response: {resp.hex() if resp else 'NONE'}")

# Test 2: WRITE_UB (correct format!)
print("\n2. WRITE_UB - Write 32 bytes to address 0x0000")
test_data = bytes([i & 0xFF for i in range(32)])
cmd = bytes([
    0x01,        # Command: WRITE_UB
    0x00,        # addr_hi
    0x00,        # addr_lo
    0x00,        # length_hi (32 = 0x0020)
    0x20,        # length_lo
]) + test_data
s.write(cmd)
time.sleep(0.3)
print(f"   Sent: {len(cmd)} bytes total")

# Test 3: READ_STATUS
print("\n3. READ_STATUS (after write)")
s.reset_input_buffer()
s.write(bytes([0x06]))
time.sleep(0.2)
resp = s.read(1)
print(f"   Response: {resp.hex() if resp else 'NONE'}")

# Test 4: READ_UB (correct format!)
print("\n4. READ_UB - Read back 32 bytes from address 0x0000")
cmd = bytes([
    0x04,        # Command: READ_UB
    0x00,        # addr_hi
    0x00,        # addr_lo
    0x00,        # length_hi
    0x20,        # length_lo (32 bytes)
])
s.reset_input_buffer()
s.write(cmd)
time.sleep(0.5)
readback = s.read(32)
print(f"   Received: {len(readback)} bytes")
if readback:
    print(f"   Expected: {test_data[:8].hex()}...")
    print(f"   Got:      {readback[:8].hex()}...")
    if readback == test_data:
        print("   ✓✓✓ DATA MATCH!")

# Test 5: WRITE_INSTR (correct format!)
print("\n5. WRITE_INSTR - Write NOP to address 0x0000")
cmd = bytes([
    0x03,                    # Command: WRITE_INSTR
    0x00,                    # addr_hi
    0x00,                    # addr_lo
    0x00, 0x00, 0x00, 0x00   # NOP instruction
])
s.write(cmd)
time.sleep(0.2)
print(f"   Sent: NOP instruction")

# Test 6: EXECUTE
print("\n6. EXECUTE")
s.write(bytes([0x05]))
time.sleep(0.3)
print(f"   Execute command sent")

# Test 7: READ_STATUS (after execute)
print("\n7. READ_STATUS (after execute)")
s.reset_input_buffer()
s.write(bytes([0x06]))
time.sleep(0.2)
resp = s.read(1)
print(f"   Response: {resp.hex() if resp else 'NONE'}")

s.close()
print("\n" + "=" * 60)
print("Test complete!")
print("=" * 60)
