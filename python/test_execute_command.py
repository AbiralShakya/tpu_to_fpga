#!/usr/bin/env python3

import serial
import time

def test_execute_command(port):
    """Test EXECUTE command (0x05)"""
    try:
        ser = serial.Serial(port, 115200, timeout=2)
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        print("Testing EXECUTE command (0x05)...")

        # First check status before execute
        print("Checking status before EXECUTE...")
        ser.write(bytes([0x06]))  # READ_STATUS
        time.sleep(0.1)
        if ser.in_waiting > 0:
            status_before = ser.read(1)
            print(f"Status before: 0x{status_before[0]:02X}")
        else:
            print("No status response")
            return False

        # Send EXECUTE command
        print("Sending EXECUTE (0x05)...")
        ser.write(bytes([0x05]))  # EXECUTE
        ser.flush()

        # Wait for execution to start/complete
        time.sleep(1.0)

        # Check status after execute
        print("Checking status after EXECUTE...")
        ser.write(bytes([0x06]))  # READ_STATUS
        time.sleep(0.1)
        if ser.in_waiting > 0:
            status_after = ser.read(1)
            print(f"Status after: 0x{status_after[0]:02X}")
            print("✓ EXECUTE command responded")
            return True
        else:
            print("✗ No response after EXECUTE")
            return False

    except Exception as e:
        print(f"Error: {e}")
        return False
    finally:
        if 'ser' in locals():
            ser.close()

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: python test_execute_command.py <serial_port>")
        sys.exit(1)

    port = sys.argv[1]

    success = test_execute_command(port)
    if success:
        print("EXECUTE command test PASSED")
    else:
        print("EXECUTE command test FAILED")
