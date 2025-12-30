#!/usr/bin/env python3
import serial, time, sys

# Update the port to match your setup
PORT = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
s = serial.Serial(PORT, 115200, timeout=1.0)

def send_strict_cmd(cmd, addr=0, length=0, data=None):
    """Sends exact byte counts as expected by your RTL protocol."""
    # 0x01=WRITE_UB, 0x02=WRITE_WT, 0x04=READ_UB (5 bytes)
    if cmd in [0x01, 0x02, 0x04]:
        s.write(bytes([cmd, (addr >> 8) & 0xFF, addr & 0xFF, (length >> 8) & 0xFF, length & 0xFF]))
    # 0x03=WRITE_INSTR (3 bytes + 4 bytes of data)
    elif cmd == 0x03:
        s.write(bytes([cmd, (addr >> 8) & 0xFF, addr & 0xFF]))
    # 0x05=EXECUTE, 0x06=STATUS, 0x14=DEBUG (1 byte)
    else:
        s.write(bytes([cmd]))
    
    if data:
        s.write(data)
    
    # Immediately consume ACKs to keep buffers in sync
    if cmd == 0x01: # UB_ACK (0xAA)
        ack = s.read(1)
        if ack != b'\xAA': print(f"Warning: UB ACK error. Got {ack.hex()}")
    elif cmd == 0x02: # WT_ACK (0xBB)
        ack = s.read(1)
        if ack != b'\xBB': print(f"Warning: WT ACK error. Got {ack.hex()}")

def encode_instr(opcode, arg1=0, arg2=0, arg3=0, flags=0):
    return (((opcode & 0x3F) << 26) | ((arg1 & 0xFF) << 18) | \
            ((arg2 & 0xFF) << 10) | ((arg3 & 0xFF) << 2) | (flags & 0x03)).to_bytes(4, 'big')

# 1. Reset
s.reset_input_buffer()
s.write(bytes([0xFF]*5)) 
time.sleep(0.1)
s.reset_input_buffer()

print("--- 1. Loading Input Data [10, 20, 30] ---")
send_strict_cmd(0x01, addr=0, length=32, data=bytes([10, 20, 30] + [0]*29))

print("--- 2. Loading Identity Weights ---")
for i in range(3):
    row = [0,0,0]; row[i] = 1
    send_strict_cmd(0x02, addr=i, length=32, data=bytes(row + [0]*29))

print("--- 3. Flashing Program (RD_WT -> LD_UB -> MATMUL -> ST_UB -> HALT) ---")
program = [
    encode_instr(0x03, 0, 1), encode_instr(0x03, 1, 1), encode_instr(0x03, 2, 1), # Load 3 weight rows
    encode_instr(0x04, 0, 3),        # LD_UB (Pull inputs from UB addr 0)
    encode_instr(0x10, 0, 0, 3, 0),  # MATMUL (3 rows, result to acc addr 0)
    encode_instr(0x00), encode_instr(0x00), # NOP drains
    encode_instr(0x05, 10, 3),       # ST_UB (Store result to UB addr 10)
    encode_instr(0x3F)               # HALT
]
for i, instr in enumerate(program):
    send_strict_cmd(0x03, addr=i, data=instr)

print("--- 4. Execution & Status Polling ---")
send_strict_cmd(0x05) # EXECUTE

for i in range(20):
    send_strict_cmd(0x06) # STATUS
    st = s.read(1)
    if st and (st[0] & 0x80): # Check bit 7 (halt_req)
        print(f"  ✓ TPU Halted. Status: {st.hex()}")
        break
    time.sleep(0.1)

print("--- 5. Reading Result from UB Addr 10 ---")
s.reset_input_buffer()
send_strict_cmd(0x04, addr=10, length=3)
time.sleep(0.2) # Allow FPGA time to fetch
res = list(s.read(3))

print(f"Expected: [10, 20, 30]")
print(f"Actual:   {res}")

if res == [10, 20, 30]:
    print("\n✅ SUCCESS: MatMul Logic and Protocol are fully verified!")
else:
    print("\n❌ FAILED: Still getting zeros. Check if rx_ready is 1 in RTL.")