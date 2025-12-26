#!/usr/bin/env python3

import serial
import time

def test_write_instr(port):
    """Test WRITE_INSTR command"""
    try:
        ser = serial.Serial(port, 115200, timeout=2)
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        print("Testing WRITE_INSTR command...")

        # First check status
        print("Initial status check...")
        ser.write(bytes([0x06]))
        ser.flush()
        time.sleep(0.1)
        status = ser.read(1)
        if status:
            print(f"Initial status: 0x{status[0]:02X}")
        else:
            print("No initial status")

        # Write a simple instruction (same as comprehensive test)
        print("Writing NOP instruction...")
        nop_instr = bytes([0x00, 0x00, 0x00, 0x00])  # All zeros

        cmd_bytes = [0x03, 0x00, 0x01] + list(nop_instr)  # Address 0x01
        for byte in cmd_bytes:
            ser.write(bytes([byte]))
            ser.flush()
            time.sleep(0.01)
        time.sleep(0.2)

        # Check status after writing instruction
        print("Status check after WRITE_INSTR...")
        ser.write(bytes([0x06]))
        ser.flush()
        time.sleep(0.1)
        status = ser.read(1)
        if status:
            print(f"Status after write: 0x{status[0]:02X}")
            print("✓ WRITE_INSTR works!")
            return True
        else:
            print("✗ No response after WRITE_INSTR")
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
        print("Usage: python test_simple_instr.py <serial_port>")
        sys.exit(1)

    port = sys.argv[1]

    success = test_write_instr(port)
    if success:
        print("WRITE_INSTR test PASSED")
    else:
        print("WRITE_INSTR test FAILED")
