# import serial, sys, time
# s = serial.Serial(sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01', 115200, timeout=1.0)
# s.reset_input_buffer()

# print("--- Requesting Internal Debug Counters (Cmd 0x14) ---")
# s.write(bytes([0x14])) # READ_DEBUG command
# res = s.read(10) # Returns 10 bytes

# if len(res) == 10:
#     print(f"  Internal State:  0x{res[0]:02x} (Should be 0x00 for IDLE)")
#     rx_count = int.from_bytes(res[1:5], 'little')
#     tx_count = int.from_bytes(res[5:9], 'little')
#     print(f"  Total Bytes RX:  {rx_count}")
#     print(f"  Total Bytes TX:  {tx_count}")
#     print(f"  Last Valid Byte: 0x{res[9]:02x}")
# else:
#     print("❌ No response to debug command.")
# s.close()


#!/usr/bin/env python3
import serial, time, sys

# Set your serial port (e.g., '/dev/tty.usbserial-210183A27BE01' or 'COM3')
PORT = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
s = serial.Serial(PORT, 115200, timeout=1.0)

def send_strict_cmd(cmd, addr=0, length=0, data=None):
    """Sends exact byte counts as expected by uart_dma_basys3.sv"""
    if cmd in [0x01, 0x02, 0x04]: # 5-byte header
        s.write(bytes([cmd, (addr >> 8) & 0xFF, addr & 0xFF, (length >> 8) & 0xFF, length & 0xFF]))
    elif cmd == 0x03: # 3-byte header
        s.write(bytes([cmd, (addr >> 8) & 0xFF, addr & 0xFF]))
    else: # 1-byte command (0x05, 0x06)
        s.write(bytes([cmd]))
    
    if data:
        s.write(data)
    
    # Consume ACKs immediately to keep buffer clean
    if cmd == 0x01: # UB_ACK (0xAA)
        s.read(1)
    elif cmd == 0x02: # WT_ACK (0xBB)
        s.read(1)

def encode_instr(opcode, arg1=0, arg2=0, arg3=0, flags=0):
    """Encodes a 32-bit TPU instruction"""
    return (((opcode & 0x3F) << 26) | 
            ((arg1 & 0xFF) << 18) | 
            ((arg2 & 0xFF) << 10) | 
            ((arg3 & 0xFF) << 2) | 
            (flags & 0x03)).to_bytes(4, 'big')

# --- 1. Reset and Clear ---
s.reset_input_buffer()
s.write(bytes([0xFF]*5)) 
time.sleep(0.1)
s.reset_input_buffer()

print("--- 1. Loading Input Data (UB Addr 0) ---")
input_vec = [10, 20, 30]
send_strict_cmd(0x01, addr=0, length=32, data=bytes(input_vec + [0]*29))

print("--- 2. Loading Identity Weights (WT Addr 0-2) ---")
for i in range(3):
    row = [0,0,0]; row[i] = 1 # Identity matrix row
    send_strict_cmd(0x02, addr=i, length=32, data=bytes(row + [0]*29))

print("--- 3. Programming MatMul + HALT ---")
program = [
    # RD_WEIGHT: load weights for rows 0, 1, and 2
    encode_instr(0x03, 0, 1), 
    encode_instr(0x03, 1, 1), 
    encode_instr(0x03, 2, 1),
    # LD_UB: Load activations from UB address 0
    encode_instr(0x04, 0, 3),        
    # MATMUL: 3 rows, base address 0, accumulator address 0
    encode_instr(0x10, 0, 0, 3, 0),  
    # Pipeline Drains (NOPs)
    encode_instr(0x00), encode_instr(0x00), 
    # ST_UB: Store results to UB Addr 10
    encode_instr(0x05, 10, 3),       
    # HALT: Terminate program
    encode_instr(0x3F)               
]

for i, instr in enumerate(program):
    send_strict_cmd(0x03, addr=i, data=instr)

print("--- 4. Executing - WATCH LED 11 NOW ---")
send_strict_cmd(0x05)

# Poll status for the Halt bit (0x80)
for i in range(20):
    send_strict_cmd(0x06)
    st = s.read(1)
    if st:
        val = st[0]
        if val & 0x80:
            print(f"  ✓ Halted detected! Status: 0x{val:02x}")
            break
    time.sleep(0.1)

print("--- 5. Reading Results from UB Addr 10 ---")
s.reset_input_buffer()
send_strict_cmd(0x04, addr=10, length=3)
res = list(s.read(3))
print(f"  Actual Results: {res}")

s.close()