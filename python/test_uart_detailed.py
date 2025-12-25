#!/usr/bin/env python3
"""
Detailed UART diagnostic for TPU FPGA
Tests various aspects of UART communication
"""

import serial
import time
import sys

if len(sys.argv) < 2:
    print("Usage: python3 test_uart_detailed.py /dev/tty.usbserial-XXXXX")
    sys.exit(1)

port = sys.argv[1]

print("=" * 80)
print("DETAILED UART DIAGNOSTIC")
print("=" * 80)
print()

try:
    ser = serial.Serial(port, 115200, timeout=0.5)
    print(f"✓ Port opened: {port}")
    print(f"  Baudrate: 115200, Parity: None, Stop bits: 1")
    print()
except Exception as e:
    print(f"✗ Failed to open port: {e}")
    sys.exit(1)

# Clear any existing data
ser.reset_input_buffer()
ser.reset_output_buffer()
time.sleep(0.1)

# Test 1: Send single byte and watch for ANY response
print("Test 1: Send 0x06 (READ_STATUS) and wait...")
ser.write(bytes([0x06]))
time.sleep(0.5)
response = ser.read(100)
if response:
    print(f"  ✓ Got {len(response)} bytes: {' '.join(f'{b:02x}' for b in response)}")
else:
    print(f"  ✗ No response")
print()

# Test 2: Send multiple different commands
print("Test 2: Try all basic commands...")
commands = {
    0x01: "WRITE_UB",
    0x02: "WRITE_WEIGHT",
    0x03: "WRITE_INSTR",
    0x04: "READ_UB",
    0x05: "EXECUTE",
    0x06: "READ_STATUS",
}

for cmd, name in commands.items():
    ser.reset_input_buffer()
    ser.write(bytes([cmd]))
    time.sleep(0.2)
    response = ser.read(100)
    status = "✓" if response else "✗"
    print(f"  {status} 0x{cmd:02x} ({name:14s}): {len(response)} bytes")
print()

# Test 3: Check if FPGA is echoing
print("Test 3: Send pattern and check for echo...")
ser.reset_input_buffer()
pattern = bytes([0xAA, 0x55, 0xFF, 0x00])
ser.write(pattern)
time.sleep(0.3)
response = ser.read(100)
if response:
    print(f"  Sent: {' '.join(f'{b:02x}' for b in pattern)}")
    print(f"  Got:  {' '.join(f'{b:02x}' for b in response)}")
    if response == pattern:
        print("  → Exact echo (possible loopback)")
else:
    print(f"  ✗ No echo")
print()

# Test 4: Send complete WRITE_UB command
print("Test 4: Send complete WRITE_UB command (0x01 + 32 bytes)...")
ser.reset_input_buffer()
cmd = bytes([0x01])  # WRITE_UB
data = bytes([i & 0xFF for i in range(32)])  # 32 bytes of data
ser.write(cmd + data)
time.sleep(0.3)
response = ser.read(100)
if response:
    print(f"  ✓ Got {len(response)} bytes: {' '.join(f'{b:02x}' for b in response[:8])}...")
else:
    print(f"  ✗ No response")
print()

# Test 5: Just listen for spontaneous data
print("Test 5: Listen for spontaneous data (3 seconds)...")
ser.reset_input_buffer()
start = time.time()
all_data = b''
while time.time() - start < 3:
    if ser.in_waiting:
        all_data += ser.read(ser.in_waiting)
    time.sleep(0.1)

if all_data:
    print(f"  ✓ Received {len(all_data)} bytes spontaneously:")
    print(f"    {' '.join(f'{b:02x}' for b in all_data[:16])}...")
else:
    print(f"  ✗ No spontaneous data")
print()

# Test 6: Check baud rate detection
print("Test 6: Try 0x55 (UART autobaud pattern)...")
ser.reset_input_buffer()
ser.write(bytes([0x55] * 10))
time.sleep(0.3)
response = ser.read(100)
if response:
    print(f"  ✓ Got {len(response)} bytes: {' '.join(f'{b:02x}' for b in response)}")
else:
    print(f"  ✗ No response")
print()

ser.close()

print("=" * 80)
print("SUMMARY")
print("=" * 80)
print("If ALL tests show '✗ No response':")
print("  → UART RX not working (hardware/pin/clock issue)")
print()
print("If you see ANY data:")
print("  → UART is partially working, check protocol implementation")
print()
print("Current LED pattern should help:")
print("  LED[15:8] = byte count received")
print("  LED[7:0]  = UART state machine state")
print("=" * 80)
