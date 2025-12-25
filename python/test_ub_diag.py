# #!/usr/bin/env python3
# """
# Unified Buffer Write/Read Diagnostic
# Tests different addresses to find working range
# """
# import serial, time, sys

# port = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
# s = serial.Serial(port, 115200, timeout=1)

# print("UB Diagnostic: Testing different addresses")
# print("=" * 60)

# # Try writing and reading at different addresses
# test_pattern = bytes([0xAA, 0x55, 0xFF, 0x00] * 8)  # 32 bytes

# for addr in [0x0000, 0x0001, 0x0010, 0x0100]:
#     print(f"\nTesting address 0x{addr:04x}:")

#     # Write
#     cmd = bytes([0x01, (addr>>8)&0xFF, addr&0xFF, 0x00, 0x20]) + test_pattern
#     s.write(cmd)
#     time.sleep(0.3)
#     print(f"  Wrote pattern to 0x{addr:04x}")

#     # Read back
#     cmd = bytes([0x04, (addr>>8)&0xFF, addr&0xFF, 0x00, 0x20])
#     s.reset_input_buffer()
#     s.write(cmd)
#     time.sleep(0.5)
#     readback = s.read(32)

#     if readback:
#         non_zero = sum(1 for b in readback if b != 0)
#         print(f"  Read back: {readback[:8].hex()}...")
#         print(f"  Non-zero bytes: {non_zero}/32")
#         if non_zero > 0:
#             print(f"  ✓✓✓ FOUND DATA AT ADDRESS 0x{addr:04x}!")
#     else:
#         print(f"  ✗ No data")

# s.close()
# print("\n" + "=" * 60)



# """
# Test if we can at least READ data (even if all zeros)
# This confirms the read path works
# """
# import serial, time, sys

# port = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
# s = serial.Serial(port, 115200, timeout=1)

# print("Testing READ path (ignore write for now)")
# print("=" * 60)

# # Just try reading from several addresses
# for addr in range(0, 0x10, 0x04):
#     cmd = bytes([0x04, 0x00, addr, 0x00, 0x04])  # Read 4 bytes
#     s.reset_input_buffer()
#     s.write(cmd)
#     time.sleep(0.3)
#     data = s.read(4)
#     print(f"Addr 0x{addr:02x}: {data.hex() if data else 'NO DATA'} ({len(data) if data else 0} bytes)")

# s.close()


import serial, time

s = serial.Serial('/dev/tty.usbserial-210183A27BE01', 115200, timeout=1)

# Send garbage
print("Sending garbage...")
s.write(bytes([0xFF, 0xFF, 0xFF]))
time.sleep(0.5)

# Try status again
print("Trying READ_STATUS after garbage...")
s.reset_input_buffer()
s.write(bytes([0x06]))
time.sleep(0.3)
resp = s.read(1)
print(f"Response: {resp.hex() if resp else 'NONE'}")
s.close()