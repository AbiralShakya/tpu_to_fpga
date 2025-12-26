#!/usr/bin/env python3
"""
Test Instruction Memory Write/Read
Instruction memory might work differently than UB
"""
import serial, time, sys

port = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
s = serial.Serial(port, 115200, timeout=1)

print("=" * 60)
print("INSTRUCTION MEMORY TEST")
print("=" * 60)

# Write a recognizable pattern to instruction memory
print("\n1. Writing test instructions...")
for addr in range(4):
    # Write instruction at each address
    # Format: [0x03][addr_hi][addr_lo][4-byte instruction]
    instr = bytes([
        0xAA + addr,  # Recognizable pattern
        0x11 + addr,
        0x22 + addr,
        0x33 + addr
    ])

    cmd = bytes([0x03, 0x00, addr]) + instr
    s.write(cmd)
    time.sleep(0.2)
    print(f"  Wrote to addr {addr}: {instr.hex()}")

# Now execute and check if controller saw any of these instructions
print("\n2. Executing (to see if instructions were stored)...")
s.write(bytes([0x05]))  # EXECUTE
time.sleep(0.5)

print("\n3. Checking status...")
s.reset_input_buffer()
s.write(bytes([0x06]))
time.sleep(0.2)
status = s.read(1)
if status:
    print(f"  Status: 0x{status[0]:02x}")

# Try reading back via status multiple times
print("\n4. Multiple status reads (look for any changes)...")
for i in range(5):
    s.reset_input_buffer()
    s.write(bytes([0x06]))
    time.sleep(0.1)
    status = s.read(1)
    if status:
        print(f"  Read {i}: 0x{status[0]:02x}")

s.close()
print("\n" + "=" * 60)
print("If all status reads return 0x20, the system is stable")
print("=" * 60)
