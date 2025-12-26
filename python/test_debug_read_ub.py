#!/usr/bin/env python3

import serial
import time

def test_debug_read_ub(port):
    """Debug READ_UB by checking FPGA state"""
    try:
        ser = serial.Serial(port, 115200, timeout=2)
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        print("Testing WRITE_UB then checking state...")

        # Write test data
        test_data = bytes([0xAA, 0xBB, 0xCC, 0xDD])
        print(f"Writing {len(test_data)} bytes...")

        # WRITE_UB command
        cmd = bytes([0x01, 0x00, 0x00, 0x00, 0x04]) + test_data
        ser.write(cmd)
        ser.flush()
        time.sleep(0.2)  # Wait for write

        # Check state with READ_DEBUG
        print("Checking FPGA state after WRITE_UB...")
        ser.write(bytes([0x14]))  # READ_DEBUG
        time.sleep(0.1)
        if ser.in_waiting >= 10:
            debug_data = ser.read(10)
            state = debug_data[0]
            rx_count = int.from_bytes(debug_data[1:5], 'little')
            tx_count = int.from_bytes(debug_data[5:9], 'little')
            last_rx = debug_data[9]
            print(f"State: 0x{state:02X}, RX: {rx_count}, TX: {tx_count}, LastRX: 0x{last_rx:02X}")
        else:
            print(f"Only {ser.in_waiting} bytes available")

        # Clear buffers before READ_UB
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        time.sleep(0.1)

        # Now try READ_UB
        print("Sending READ_UB...")
        ser.write(bytes([0x04, 0x00, 0x00, 0x00, 0x04]))  # READ_UB 4 bytes
        ser.flush()

        # Wait for exactly 4 bytes
        response = b''
        start_time = time.time()
        while len(response) < 4 and (time.time() - start_time) < 2:
            if ser.in_waiting > 0:
                response += ser.read(ser.in_waiting)
            time.sleep(0.01)

        print(f"Received {len(response)} bytes after 2 seconds")
        if len(response) >= 4:
            print(f"First 4 bytes: {' '.join(f'{b:02X}' for b in response[:4])}")
        if len(response) > 4:
            print(f"Extra bytes: {' '.join(f'{b:02X}' for b in response[4:8])}")

        # Check state again
        print("Checking FPGA state after READ_UB...")
        ser.write(bytes([0x14]))  # READ_DEBUG
        time.sleep(0.1)
        if ser.in_waiting >= 10:
            debug_data = ser.read(10)
            state = debug_data[0]
            rx_count = int.from_bytes(debug_data[1:5], 'little')
            tx_count = int.from_bytes(debug_data[5:9], 'little')
            last_rx = debug_data[9]
            print(f"State: 0x{state:02X}, RX: {rx_count}, TX: {tx_count}, LastRX: 0x{last_rx:02X}")
        else:
            print(f"Debug failed, only {ser.in_waiting} bytes available")

    except Exception as e:
        print(f"Error: {e}")
    finally:
        if 'ser' in locals():
            ser.close()

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: python test_debug_read_ub.py <serial_port>")
        sys.exit(1)

    port = sys.argv[1]

    test_debug_read_ub(port)
