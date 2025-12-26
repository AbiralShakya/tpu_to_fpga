#!/usr/bin/env python3

import serial
import time

def test_single_command(port, command, expected_response_length=1):
    """Send a single command and check response"""
    try:
        ser = serial.Serial(port, 115200, timeout=2)
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        print(f"Sending command 0x{command:02X}...")

        # Send command
        ser.write(bytes([command]))
        ser.flush()

        # Wait a bit
        time.sleep(0.1)

        # Check if bytes are waiting
        waiting = ser.in_waiting
        print(f"Bytes waiting: {waiting}")

        if waiting > 0:
            response = ser.read(waiting)
            print(f"Response: {' '.join(f'{b:02X}' for b in response)}")
            return response
        else:
            print("No response received")
            return None

    except Exception as e:
        print(f"Error: {e}")
        return None
    finally:
        if 'ser' in locals():
            ser.close()

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: python test_simple_command.py <serial_port>")
        sys.exit(1)

    port = sys.argv[1]

    print("Testing READ_STATUS (0x06)...")
    response = test_single_command(port, 0x06, 1)
    if response and len(response) == 1:
        print("✓ Basic UART working")
    else:
        print("✗ Basic UART not working")
