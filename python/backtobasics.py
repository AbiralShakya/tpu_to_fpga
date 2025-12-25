import serial, time
s = serial.Serial('/dev/tty.usbserial-210183A27BE01', 115200, timeout=2)
print("Clearing buffers...")
s.reset_input_buffer()
s.reset_output_buffer()
time.sleep(0.5)

print("Sending READ_STATUS (0x06)...")
s.write(bytes([0x06]))
s.flush()
time.sleep(1)  # Long wait

print(f"Bytes waiting: {s.in_waiting}")
response = s.read(10)
if response:
    print(f"✓ Got: {' '.join(f'{b:02x}' for b in response)}")
else:
    print("✗ No response - CHECK SW15 IS UP!")
s.close()