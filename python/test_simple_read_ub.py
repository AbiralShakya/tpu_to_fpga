#!/usr/bin/env python3

import serial
import time

def test_read_ub(port):
    """Test READ_UB after WRITE_UB"""
    try:
        ser = serial.Serial(port, 115200, timeout=5)
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        print("Testing WRITE_UB then READ_UB...")

        # Write test data (just 4 bytes to avoid overwhelming UART)
        test_data = bytes([0xAA, 0xBB, 0xCC, 0xDD])
        print(f"Writing {len(test_data)} bytes: {test_data.hex()}...")

        # WRITE_UB command: Send byte by byte with delays
        cmd_bytes = [0x01, 0x00, 0x00, 0x00, 0x04] + list(test_data)
        for i, byte in enumerate(cmd_bytes):
            ser.write(bytes([byte]))
            ser.flush()
            time.sleep(0.01)  # Small delay between bytes

        time.sleep(0.2)  # Wait for write to complete

        # Check if any response (should be none for write)
        waiting = ser.in_waiting
        if waiting > 0:
            resp = ser.read(waiting)
            print(f"Unexpected response after write: {resp.hex()}")

        # Check basic communication before READ_UB
        print("Checking basic communication...")
        ser.write(bytes([0x06]))  # READ_STATUS
        time.sleep(0.1)
        if ser.in_waiting > 0:
            status = ser.read(1)
            print(f"Status: 0x{status[0]:02X}")
        else:
            print("No status response - FPGA not responding!")

        # Now read it back
        print("Reading back data...")
        # READ_UB command: Send byte by byte
        read_cmd_bytes = [0x04, 0x00, 0x00, 0x00, 0x04]
        for i, byte in enumerate(read_cmd_bytes):
            ser.write(bytes([byte]))
            ser.flush()
            time.sleep(0.01)  # Small delay between bytes

        # Wait for response
        start_time = time.time()
        response = b''
        while len(response) < 4 and (time.time() - start_time) < 3:
            if ser.in_waiting > 0:
                response += ser.read(ser.in_waiting)
            time.sleep(0.01)

        print(f"Received {len(response)} bytes")
        if len(response) == 4:
            print(f"Data: {response.hex()}")
            if response == test_data:
                print("✓ Data matches!")
                return True
            else:
                print(f"✗ Data mismatch: got {response.hex()}, expected {test_data.hex()}")
                return False
        else:
            print(f"✗ Wrong length: got {len(response)}, expected 4")

            # Check state after failed READ_UB
            print("Checking state after failed READ_UB...")
            ser.write(bytes([0x14]))  # READ_DEBUG
            time.sleep(0.1)
            if ser.in_waiting >= 10:
                debug_data = ser.read(10)
                state = debug_data[0]
                print(f"State after READ_UB: 0x{state:02X}")
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
        print("Usage: python test_simple_read_ub.py <serial_port>")
        sys.exit(1)

    port = sys.argv[1]

    success = test_read_ub(port)
    if success:
        print("READ_UB test PASSED")
    else:
        print("READ_UB test FAILED")
