#!/usr/bin/env python3
"""
Comprehensive UART diagnostic test - tests multiple hypotheses
"""

import serial
import sys
import time
import json
import os

def test_all_ports():
    """Test all available serial ports"""
    import glob
    ports = glob.glob('/dev/tty.usbserial-*')
    print("="*80)
    print("Testing All Serial Ports")
    print("="*80)
    print(f"\nFound {len(ports)} port(s):")
    for port in ports:
        print(f"  - {port}")
    
    results = {}
    for port in ports:
        print(f"\n{'='*80}")
        print(f"Testing: {port}")
        print(f"{'='*80}")
        result = test_single_port(port)
        results[port] = result
    
    print(f"\n{'='*80}")
    print("Summary")
    print(f"{'='*80}")
    for port, result in results.items():
        status = "✓ RESPONDING" if result['responded'] else "✗ NO RESPONSE"
        print(f"{port}: {status}")
        if result['responded']:
            print(f"  Response: {result['response_hex']}")
    
    return results

def test_single_port(port, verbose=True):
    """Test a single serial port comprehensively"""
    result = {
        'port': port,
        'responded': False,
        'response_hex': '',
        'bytes_received': 0
    }
    
    try:
        ser = serial.Serial(port, 115200, timeout=2.0)
        if verbose:
            print(f"✓ Port opened: {port}")
            print(f"  Config: {ser.baudrate} baud, {ser.bytesize}N{ser.parity}{ser.stopbits}")
        
        # Flush everything
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        time.sleep(0.2)
        
        # Test 1: Send READ_STATUS (0x06) multiple times
        if verbose:
            print("\nTest 1: Send READ_STATUS (0x06) 5 times...")
        responses = []
        for i in range(5):
            ser.write(b'\x06')
            time.sleep(0.2)  # Give time for response
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                responses.append(data.hex())
                result['bytes_received'] += len(data)
                if verbose:
                    print(f"  Attempt {i+1}: Received {len(data)} bytes - {data.hex()}")
        
        # Test 2: Send various commands and check for ANY response
        if verbose:
            print("\nTest 2: Send various commands...")
        commands = [
            (b'\x06', "READ_STATUS"),
            (b'\x01\x00\x00\x00\x20', "WRITE_UB"),
            (b'\x04\x00\x00\x00\x20', "READ_UB"),
            (b'\x05', "EXECUTE"),
        ]
        
        for cmd, name in commands:
            ser.write(cmd)
            time.sleep(0.3)
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                if verbose:
                    print(f"  {name}: Received {len(data)} bytes - {data.hex()}")
                result['bytes_received'] += len(data)
                responses.append(data.hex())
        
        # Test 3: Send continuous pattern and check for ANY bytes
        if verbose:
            print("\nTest 3: Send continuous pattern (0xAA repeated)...")
        pattern = b'\xAA' * 10
        ser.write(pattern)
        time.sleep(0.5)
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting)
            if verbose:
                print(f"  Received {len(data)} bytes - {data.hex()}")
            result['bytes_received'] += len(data)
            responses.append(data.hex())
        
        # Test 4: Check if port is actually connected (try to read anything)
        if verbose:
            print("\nTest 4: Check for any incoming data (wait 1 second)...")
        time.sleep(1.0)
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting)
            if verbose:
                print(f"  Spontaneous data: {len(data)} bytes - {data.hex()}")
            result['bytes_received'] += len(data)
            responses.append(data.hex())
        
        # Summary
        if result['bytes_received'] > 0:
            result['responded'] = True
            result['response_hex'] = ' '.join(responses)
            if verbose:
                print(f"\n✓ Port {port} IS responding!")
                print(f"  Total bytes received: {result['bytes_received']}")
        else:
            if verbose:
                print(f"\n✗ Port {port} is NOT responding")
                print("  No data received from FPGA")
        
        ser.close()
        
    except Exception as e:
        if verbose:
            print(f"✗ Error testing {port}: {e}")
        result['error'] = str(e)
    
    return result

def log_result(result, log_path):
    """Log test result"""
    try:
        with open(log_path, 'a') as f:
            f.write(json.dumps({
                "sessionId": "debug-session",
                "runId": "run1",
                "hypothesisId": "H1",
                "location": "test_uart_diagnostic.py:test_single_port",
                "message": "UART diagnostic test",
                "data": result,
                "timestamp": int(time.time() * 1000)
            }) + "\n")
    except:
        pass

if __name__ == '__main__':
    log_path = os.path.join(os.path.dirname(os.path.dirname(__file__)), '.cursor', 'debug.log')
    
    if len(sys.argv) > 1:
        # Test specific port
        port = sys.argv[1]
        result = test_single_port(port)
        log_result(result, log_path)
    else:
        # Test all ports
        results = test_all_ports()
        for port, result in results.items():
            log_result(result, log_path)

