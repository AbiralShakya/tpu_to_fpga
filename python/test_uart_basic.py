#!/usr/bin/env python3
"""
Basic UART connectivity test - verifies serial port works
"""

import serial
import sys
import time

def test_uart_basic(port):
    """Test basic UART connectivity"""
    print("="*80)
    print("Basic UART Connectivity Test")
    print("="*80)
    print(f"\nTesting port: {port}")
    
    try:
        ser = serial.Serial(port, 115200, timeout=1.0)
        print(f"✓ Serial port opened")
        print(f"  Config: {ser.baudrate} baud, {ser.bytesize}N{ser.parity}{ser.stopbits}")
        
        # Flush buffers
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        time.sleep(0.1)
        
        # Test 1: Send a byte and see if anything comes back
        print("\nTest 1: Send READ_STATUS (0x06) and wait for response...")
        ser.write(b'\x06')
        time.sleep(0.1)
        
        in_waiting = ser.in_waiting
        print(f"  Bytes in buffer: {in_waiting}")
        
        if in_waiting > 0:
            response = ser.read(in_waiting)
            print(f"  ✓ Response received: {response.hex()}")
        else:
            print(f"  ✗ No response (FPGA not responding)")
        
        # Test 2: Send multiple commands
        print("\nTest 2: Send multiple commands...")
        commands = [b'\x06', b'\x06', b'\x06']  # READ_STATUS
        for i, cmd in enumerate(commands):
            ser.write(cmd)
            time.sleep(0.1)
            in_waiting = ser.in_waiting
            print(f"  Command {i+1}: sent {cmd.hex()}, buffer has {in_waiting} bytes")
        
        # Read any accumulated responses
        if ser.in_waiting > 0:
            response = ser.read(ser.in_waiting)
            print(f"  ✓ Total response: {response.hex()}")
        else:
            print(f"  ✗ No responses accumulated")
        
        ser.close()
        print("\n" + "="*80)
        print("Diagnosis:")
        if in_waiting == 0:
            print("  ✗ FPGA is NOT responding to UART commands")
            print("  Possible causes:")
            print("    1. Wrong bitstream loaded (doesn't have UART DMA)")
            print("    2. UART DMA not properly connected in bitstream")
            print("    3. UART pins incorrectly assigned")
            print("    4. FPGA clock/reset not working")
            print("\n  Solution: Load tpu_top_wrapper_rev2.bit (with streaming mode)")
        else:
            print("  ✓ FPGA IS responding!")
            print("  UART communication is working")
        
    except Exception as e:
        print(f"✗ Error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python3 test_uart_basic.py /dev/tty.usbserial-XXXX")
        sys.exit(1)
    
    test_uart_basic(sys.argv[1])

