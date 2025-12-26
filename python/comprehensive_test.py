#!/usr/bin/env python3
"""
Comprehensive TPU UART Test
Shows exactly what commands work and what don't
"""
import serial, time, sys

port = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
s = serial.Serial(port, 115200, timeout=1)

def send_cmd(name, cmd_bytes, expect_response=0):
    """Send command and optionally read response"""
    print(f"\n{name}:")
    print(f"  Sending: {cmd_bytes[:16].hex()}{'...' if len(cmd_bytes) > 16 else ''}")
    s.reset_input_buffer()
    s.write(cmd_bytes)
    time.sleep(0.3)

    if expect_response > 0:
        resp = s.read(expect_response)
        print(f"  Response: {resp.hex() if resp else 'NONE'} ({len(resp) if resp else 0} bytes)")
        return resp
    else:
        print(f"  (no response expected)")
        return None

print("=" * 70)
print("COMPREHENSIVE TPU UART TEST")
print("=" * 70)

# Test 1: Simple status (baseline)
print("\n" + "─" * 70)
print("PHASE 1: BASELINE (commands that should always work)")
print("─" * 70)
send_cmd("1.1 READ_STATUS", bytes([0x06]), 1)
send_cmd("1.2 READ_STATUS again", bytes([0x06]), 1)

# Test 2: Write commands (MUST USE 32 BYTES - unified buffer is 256-bit wide!)
print("\n" + "─" * 70)
print("PHASE 2: WRITE COMMANDS (32-byte chunks required)")
print("─" * 70)
# Create 32-byte test pattern
test_data = bytes([0xAA, 0xBB, 0xCC, 0xDD] + [i & 0xFF for i in range(28)])
send_cmd("2.1 WRITE_UB (32 bytes)", bytes([0x01, 0x00, 0x00, 0x00, 0x20]) + test_data, 0)

# Write 32 bytes of weight data
weight_data = bytes([0x11, 0x22, 0x33, 0x44] + [0xFF - i for i in range(28)])
send_cmd("2.2 WRITE_WEIGHT (32 bytes)", bytes([0x02, 0x00, 0x00, 0x00, 0x20]) + weight_data, 0)

# Write instruction (4 bytes is fine for instruction memory)
send_cmd("2.3 WRITE_INSTR (NOP)", bytes([0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]), 0)

# Test 3: Check if writes affected status
print("\n" + "─" * 70)
print("PHASE 3: STATUS AFTER WRITES")
print("─" * 70)
send_cmd("3.1 READ_STATUS", bytes([0x06]), 1)

# Test 4: Execute
print("\n" + "─" * 70)
print("PHASE 4: EXECUTE")
print("─" * 70)
send_cmd("4.1 EXECUTE", bytes([0x05]), 0)
time.sleep(0.2)
send_cmd("4.2 READ_STATUS (after execute)", bytes([0x06]), 1)

# Test 5: Read back the 32-byte data we wrote
print("\n" + "─" * 70)
print("PHASE 5: READ BACK 32-BYTE DATA")
print("─" * 70)
resp = send_cmd("5.1 READ_UB (32 bytes from addr 0x0000)", bytes([0x04, 0x00, 0x00, 0x00, 0x20]), 32)
if resp and len(resp) == 32:
    print(f"  First 8 bytes: {resp[:8].hex()}")
    print(f"  Expected:      aabbccdd00010203")
    if resp[:4] == bytes([0xAA, 0xBB, 0xCC, 0xDD]):
        print("  ✓✓✓ DATA MATCH! Write/Read working!")
    else:
        print(f"  ✗ Mismatch! Got: {resp[:4].hex()}")
else:
    print(f"  ✗ Wrong length: {len(resp) if resp else 0} bytes")

# Test 6: Final status
print("\n" + "─" * 70)
print("PHASE 6: FINAL STATUS")
print("─" * 70)
send_cmd("6.1 READ_STATUS", bytes([0x06]), 1)

s.close()

print("\n" + "=" * 70)
print("ANALYSIS:")
print("=" * 70)
print("✓ If all READ_STATUS commands returned 0x20:")
print("  → UART communication is stable")
print()
print("✓ If Phase 5 shows 'DATA MATCH':")
print("  → Unified Buffer write/read path WORKS!")
print("  → Bank selection fix successful")
print()
print("✗ If READ_UB data is still zeros:")
print("  → Check synthesis logs for memory optimization")
print("  → Verify UART DMA is triggering write enables")
print()
print("IMPORTANT: Unified buffer is 256-bit (32 bytes) wide")
print("  - All writes must be in 32-byte chunks")
print("  - UART DMA accumulates bytes before writing to BRAM")
print("=" * 70)
