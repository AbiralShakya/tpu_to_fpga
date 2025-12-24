#!/usr/bin/env python3
"""
Simple UART Communication Test for Basys3 FPGA
Tests basic UART RX/TX functionality
"""

import serial
import time
import sys
import struct
import json
import os

def find_uart_device():
    """Find UART device on Mac"""
    import subprocess
    try:
        # Try to find USB serial devices
        result = subprocess.run(['ls', '/dev/tty.usbserial*'], 
                              capture_output=True, text=True, shell=True)
        if result.returncode == 0 and result.stdout.strip():
            devices = result.stdout.strip().split('\n')
            # Usually the second one is UART (first is JTAG)
            if len(devices) > 1:
                return devices[-1]  # Last one is usually UART
            return devices[0]
        
        # Try cu devices
        result = subprocess.run(['ls', '/dev/cu.usbserial*'], 
                              capture_output=True, text=True, shell=True)
        if result.returncode == 0 and result.stdout.strip():
            devices = result.stdout.strip().split('\n')
            return devices[-1]
    except:
        pass
    return None

def log_debug(location, message, data=None, hypothesis_id=None):
    """Write debug log entry"""
    log_path = "/Users/abiralshakya/Documents/tpu_to_fpga/.cursor/debug.log"
    entry = {
        "timestamp": int(time.time() * 1000),
        "location": location,
        "message": message,
        "data": data or {},
        "sessionId": "debug-session",
        "hypothesisId": hypothesis_id
    }
    try:
        with open(log_path, "a") as f:
            f.write(json.dumps(entry) + "\n")
    except:
        pass

def test_uart_basic(port, baud=115200):
    """Test basic UART communication"""
    print(f"\n{'='*60}")
    print(f"Basic UART Communication Test")
    print(f"{'='*60}")
    print(f"Port: {port}")
    print(f"Baud: {baud}")
    print()
    
    try:
        # #region agent log
        log_debug("test_uart_simple.py:46", "Opening serial port", {"port": port, "baud": baud}, "A")
        # #endregion
        ser = serial.Serial(port, baud, timeout=2)
        time.sleep(2)  # Wait for FPGA to initialize
        # #region agent log
        log_debug("test_uart_simple.py:49", "Serial port opened", {
            "port": ser.port,
            "baudrate": ser.baudrate,
            "bytesize": ser.bytesize,
            "parity": ser.parity,
            "stopbits": ser.stopbits,
            "timeout": ser.timeout,
            "in_waiting": ser.in_waiting
        }, "A")
        # #endregion
        print("✓ Serial port opened")
        
        # Test 0: Check for any pending data (hardware connection test)
        print("\n[Test 0] Checking hardware connection...")
        # #region agent log
        log_debug("test_uart_simple.py:51", "Hardware connection check", {
            "in_waiting": ser.in_waiting,
            "out_waiting": ser.out_waiting if hasattr(ser, 'out_waiting') else None
        }, "A")
        # #endregion
        # Try to read any pending bytes (should be empty if FPGA is idle)
        pending = ser.read(ser.in_waiting) if ser.in_waiting > 0 else b''
        # #region agent log
        log_debug("test_uart_simple.py:57", "Pending bytes check", {
            "pending_bytes": list(pending) if pending else None,
            "pending_count": len(pending)
        }, "A")
        # #endregion
        if pending:
            print(f"  ⚠ Found {len(pending)} pending bytes: {[hex(b) for b in pending]}")
        else:
            print("  ✓ No pending data (expected for idle FPGA)")
        
        # Test 1: Send status query
        print("\n[Test 1] Sending status query (CMD_READ_STATUS = 0x06)...")
        # #region agent log
        log_debug("test_uart_simple.py:52", "Before sending status query", {
            "bytes_to_send": [0x06],
            "in_waiting_before": ser.in_waiting
        }, "B")
        # #endregion
        bytes_written = ser.write(bytes([0x06]))
        # #region agent log
        log_debug("test_uart_simple.py:56", "After sending status query", {
            "bytes_written": bytes_written,
            "in_waiting_after_send": ser.in_waiting
        }, "B")
        # #endregion
        time.sleep(0.1)
        # #region agent log
        log_debug("test_uart_simple.py:60", "Before reading response", {
            "in_waiting": ser.in_waiting,
            "timeout": ser.timeout
        }, "B")
        # #endregion
        response = ser.read(1)
        # #region agent log
        log_debug("test_uart_simple.py:64", "After reading response", {
            "response_length": len(response),
            "response_bytes": list(response) if response else None,
            "in_waiting_after": ser.in_waiting
        }, "B")
        # #endregion
        if len(response) == 1:
            status = response[0]
            print(f"✓ Received status byte: 0x{status:02X}")
            print(f"  Bit 0 (sys_busy): {status & 0x01}")
            print(f"  Bit 1 (sys_done): {(status >> 1) & 0x01}")
            print(f"  Bit 2 (vpu_busy): {(status >> 2) & 0x01}")
            print(f"  Bit 3 (vpu_done): {(status >> 3) & 0x01}")
            print(f"  Bit 4 (ub_busy):  {(status >> 4) & 0x01}")
            print(f"  Bit 5 (ub_done):  {(status >> 5) & 0x01}")
        else:
            print("⚠ No response")
            print("  ⚠️  IMPORTANT: If you're using simple_test.bit, it does NOT implement UART!")
            print("     The design only tests physical I/O (switches, LEDs, 7-seg)")
            print("     For UART testing, you need:")
            print("     1. uart_test_simple.bit (loopback test) - build with: cd tpu_to_fpga && vivado -mode batch -source build_uart_test.tcl")
            print("     2. OR full TPU bitstream (full UART DMA) - build with: cd tpu_to_fpga && ./build_tpu_bitstream.sh")
        
        # Test 2: Send NOP instruction (if design supports it)
        print("\n[Test 2] Testing instruction write (CMD_WRITE_INSTR = 0x03)...")
        # Format: CMD(1) + ADDR_HI(1) + ADDR_LO(1) + INSTR(4)
        nop_instr = 0x00000000  # NOP instruction
        cmd = struct.pack('BBB', 0x03, 0x00, 0x00)  # CMD, addr_hi, addr_lo
        instr_bytes = struct.pack('>I', nop_instr)  # 32-bit instruction (big-endian)
        full_cmd = cmd + instr_bytes
        # #region agent log
        log_debug("test_uart_simple.py:75", "Before sending instruction", {
            "cmd_bytes": list(full_cmd),
            "cmd_hex": [hex(b) for b in full_cmd],
            "in_waiting_before": ser.in_waiting
        }, "C")
        # #endregion
        bytes_written = ser.write(full_cmd)
        # #region agent log
        log_debug("test_uart_simple.py:82", "After sending instruction", {
            "bytes_written": bytes_written,
            "expected_bytes": len(full_cmd),
            "in_waiting_after": ser.in_waiting
        }, "C")
        # #endregion
        time.sleep(0.1)
        # #region agent log
        log_debug("test_uart_simple.py:88", "After wait period", {
            "in_waiting": ser.in_waiting,
            "any_response": ser.in_waiting > 0
        }, "C")
        # #endregion
        print("✓ NOP instruction sent")
        
        # Test 3: Send execute command
        print("\n[Test 3] Sending execute command (CMD_EXECUTE = 0x05)...")
        # #region agent log
        log_debug("test_uart_simple.py:95", "Before execute command", {
            "cmd": 0x05,
            "in_waiting": ser.in_waiting
        }, "D")
        # #endregion
        ser.write(bytes([0x05]))
        time.sleep(0.1)
        # #region agent log
        log_debug("test_uart_simple.py:100", "After execute command", {
            "in_waiting": ser.in_waiting
        }, "D")
        # #endregion
        print("✓ Execute command sent")
        
        # Test 4: Read status again
        print("\n[Test 4] Reading status after execute...")
        # #region agent log
        log_debug("test_uart_simple.py:107", "Before second status query", {
            "in_waiting": ser.in_waiting
        }, "E")
        # #endregion
        ser.write(bytes([0x06]))
        time.sleep(0.1)
        # #region agent log
        log_debug("test_uart_simple.py:112", "Before reading second status", {
            "in_waiting": ser.in_waiting
        }, "E")
        # #endregion
        response = ser.read(1)
        # #region agent log
        log_debug("test_uart_simple.py:116", "After reading second status", {
            "response_length": len(response),
            "response_bytes": list(response) if response else None,
            "in_waiting_after": ser.in_waiting
        }, "E")
        # #endregion
        if len(response) == 1:
            status = response[0]
            print(f"✓ Status after execute: 0x{status:02X}")
        else:
            print("⚠ No status response")
        
        # #region agent log
        log_debug("test_uart_simple.py:125", "Test complete, closing port", {
            "final_in_waiting": ser.in_waiting
        }, "F")
        # #endregion
        ser.close()
        print("\n" + "="*60)
        print("✓ UART communication test complete!")
        print("="*60)
        return True
        
    except serial.SerialException as e:
        print(f"\n✗ Serial port error: {e}")
        print("  Check:")
        print("   1. Is FPGA connected and powered on?")
        print("   2. Is the correct port specified?")
        print("   3. Do you have permissions? (try: sudo chmod 666 {port})")
        return False
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Main function"""
    if len(sys.argv) > 1:
        port = sys.argv[1]
    else:
        # Try to auto-detect
        port = find_uart_device()
        if not port:
            print("✗ No UART device found!")
            print("\nUsage: python3 test_uart_simple.py [PORT]")
            print("\nExample:")
            print("  python3 test_uart_simple.py /dev/tty.usbserial-210292B2C3A4")
            print("\nFind your device with:")
            print("  ls /dev/tty.usbserial*")
            sys.exit(1)
        else:
            print(f"Auto-detected UART device: {port}")
    
    success = test_uart_basic(port)
    sys.exit(0 if success else 1)

if __name__ == '__main__':
    main()

