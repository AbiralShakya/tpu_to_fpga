#!/usr/bin/env python3
"""
Test READ_UB with 32 bytes (same as comprehensive test)
"""
import serial
import time
import sys

port = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
ser = serial.Serial(port, 115200, timeout=3)

print("=" * 70)
print("32-BYTE READ_UB TEST")
print("=" * 70)

# Step 1: Write 32 bytes to UB
print("\nStep 1: Writing 32 bytes to UB@0x0000...")
test_data = bytes([i for i in range(32)])  # 0x00, 0x01, ..., 0x1F
cmd = bytes([0x01, 0x00, 0x00, 0x00, 0x20]) + test_data  # WRITE_UB, addr=0x0000, len=32
for byte in cmd:
    ser.write(bytes([byte]))
    ser.flush()
    time.sleep(0.01)
time.sleep(0.2)
print(f"  Sent: {test_data.hex()}")

# Step 2: Check status
print("\nStep 2: Checking status...")
ser.reset_input_buffer()
ser.write(bytes([0x06]))  # READ_STATUS
ser.flush()
time.sleep(0.1)
status = ser.read(1)
if status:
    print(f"  Status: 0x{status[0]:02X}")
else:
    print("  No status response!")

# Step 3: Send READ_UB command (read 32 bytes)
print("\nStep 3: Sending READ_UB command (read 32 bytes from 0x0000)...")
ser.reset_input_buffer()
read_cmd = bytes([0x04, 0x00, 0x00, 0x00, 0x20])  # READ_UB, addr=0x0000, len=32
for byte in read_cmd:
    ser.write(bytes([byte]))
    ser.flush()
    time.sleep(0.01)
print(f"  Command sent: {read_cmd.hex()}")

# Step 4: Wait and read response
print("\nStep 4: Reading response...")
time.sleep(0.5)  # Wait for FPGA to process

# Check bytes waiting
waiting = ser.in_waiting
print(f"  Bytes waiting: {waiting}")

if waiting > 0:
    response = ser.read(min(waiting, 32))
    print(f"  Response ({len(response)} bytes): {response.hex()}")
    if len(response) == 32:
        if response == test_data:
            print("  ✓ SUCCESS: Data matches!")
        else:
            print(f"  ✗ FAIL: Data mismatch!")
            print(f"    Expected: {test_data.hex()}")
            print(f"    Got:      {response.hex()}")
            # Show first few bytes
            print(f"    First 8 bytes expected: {test_data[:8].hex()}")
            print(f"    First 8 bytes got:      {response[:8].hex()}")
    else:
        print(f"  ✗ FAIL: Wrong length (expected 32, got {len(response)})")
else:
    print("  ✗ FAIL: No response received")

# Step 5: Check debug state
print("\nStep 5: Checking debug state...")
ser.write(bytes([0x14]))  # READ_DEBUG
ser.flush()
time.sleep(0.2)
debug_resp = ser.read(10)
if debug_resp and len(debug_resp) == 10:
    state = debug_resp[0]
    rx_count = int.from_bytes(debug_resp[1:5], 'little')
    tx_count = int.from_bytes(debug_resp[5:9], 'little')
    last_rx = debug_resp[9]
    print(f"  State: 0x{state:02X}, RX: {rx_count}, TX: {tx_count}, LastRX: 0x{last_rx:02X}")
else:
    print("  Could not read debug state")

ser.close()
print("\n" + "=" * 70)

