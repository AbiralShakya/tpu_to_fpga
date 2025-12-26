#!/usr/bin/env python3
"""
Debug logger for FPGA UART communication
Reads debug information from FPGA and logs to debug.log in NDJSON format
"""
import serial
import time
import json
import sys

LOG_PATH = "/Users/abiralshakya/Documents/tpu_to_fpga/.cursor/debug.log"
SESSION_ID = "debug-session"
RUN_ID = sys.argv[1] if len(sys.argv) > 1 else "run1"

def log_entry(location, message, data, hypothesis_id=None):
    """Write a log entry in NDJSON format"""
    entry = {
        "id": f"log_{int(time.time()*1000)}",
        "timestamp": int(time.time() * 1000),
        "location": location,
        "message": message,
        "data": data,
        "sessionId": SESSION_ID,
        "runId": RUN_ID
    }
    if hypothesis_id:
        entry["hypothesisId"] = hypothesis_id
    
    with open(LOG_PATH, "a") as f:
        f.write(json.dumps(entry) + "\n")

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 debug_logger.py <run_id> <serial_port>")
        print("Example: python3 debug_logger.py run1 /dev/tty.usbserial-210183A27BE01")
        sys.exit(1)
    
    port = sys.argv[2]
    
    log_entry("debug_logger.py:main", "Starting debug logger", {
        "port": port,
        "runId": RUN_ID
    })
    
    try:
        # #region agent log - Open serial port (HYPOTHESIS A, E)
        ser = serial.Serial(port, 115200, timeout=2)
        time.sleep(0.5)  # Wait for serial to initialize
        
        log_entry("debug_logger.py:main", "Serial port opened", {
            "port": port,
            "baudrate": 115200,
            "timeout": 2
        }, "A")
        # #endregion
        
        # #region agent log - Clear buffers (HYPOTHESIS A)
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        
        log_entry("debug_logger.py:main", "Buffers cleared", {
            "input_buffer_cleared": True,
            "output_buffer_cleared": True
        }, "A")
        # #endregion
        
        # #region agent log - Send READ_DEBUG command first (HYPOTHESIS A, B)
        log_entry("debug_logger.py:main", "Sending READ_DEBUG (0x14) to query FPGA state", {
            "command": "0x14",
            "command_hex": "0x14",
            "expected_response": "10 bytes: state, rx_count(4), tx_count(4), last_rx_byte"
        }, "A")
        
        bytes_sent = ser.write(bytes([0x14]))
        ser.flush()
        
        log_entry("debug_logger.py:main", "READ_DEBUG command sent", {
            "bytes_sent": bytes_sent,
            "command": "0x14"
        }, "A")
        
        # Wait for debug response
        time.sleep(0.1)
        debug_bytes = ser.in_waiting
        if debug_bytes > 0:
            debug_response = ser.read(debug_bytes)
            if len(debug_response) >= 10:
                state = debug_response[0]
                rx_count = int.from_bytes(debug_response[1:5], 'little')
                tx_count = int.from_bytes(debug_response[5:9], 'little')
                last_rx = debug_response[9]
                log_entry("debug_logger.py:main", "READ_DEBUG response received", {
                    "state": state,
                    "rx_count": rx_count,
                    "tx_count": tx_count,
                    "last_rx_byte": hex(last_rx),
                    "raw_response": [hex(b) for b in debug_response]
                }, "A")
            else:
                log_entry("debug_logger.py:main", "READ_DEBUG partial response", {
                    "bytes_received": len(debug_response),
                    "raw": [hex(b) for b in debug_response]
                }, "A")
        else:
            log_entry("debug_logger.py:main", "No READ_DEBUG response", {
                "bytes_waiting": 0
            }, "A")
        
        # Clear any remaining bytes
        ser.reset_input_buffer()
        time.sleep(0.1)
        # #endregion
        
        # #region agent log - Send READ_STATUS command (HYPOTHESIS A, B)
        log_entry("debug_logger.py:main", "Sending READ_STATUS (0x06)", {
            "command": "0x06",
            "command_hex": "0x06",
            "expected_response": "Status byte (0x20 if working)"
        }, "A")
        
        bytes_sent = ser.write(bytes([0x06]))
        ser.flush()
        
        log_entry("debug_logger.py:main", "Command sent", {
            "bytes_sent": bytes_sent,
            "command": "0x06"
        }, "A")
        # #endregion
        
        # #region agent log - Check for response (HYPOTHESIS D)
        # Check immediately after sending
        immediate_bytes = ser.in_waiting
        log_entry("debug_logger.py:main", "Immediate check after send", {
            "bytes_waiting": immediate_bytes,
            "time_after_send_ms": 0
        }, "D")
        
        # Wait and check multiple times
        for wait_ms in [10, 50, 100, 500, 1000]:
            time.sleep(wait_ms / 1000.0)
            bytes_waiting = ser.in_waiting
            log_entry("debug_logger.py:main", f"Check after {wait_ms}ms", {
                "bytes_waiting": bytes_waiting,
                "time_after_send_ms": wait_ms
            }, "D")
            
            if bytes_waiting > 0:
                break
        # #endregion
        
        # #region agent log - Read response if available (HYPOTHESIS D)
        bytes_waiting = ser.in_waiting
        if bytes_waiting > 0:
            response = ser.read(bytes_waiting)
            log_entry("debug_logger.py:main", "Response received", {
                "bytes": [hex(b) for b in response],
                "bytes_decimal": [b for b in response],
                "length": len(response),
                "first_byte": hex(response[0]) if len(response) > 0 else None
            }, "D")
        else:
            log_entry("debug_logger.py:main", "No response received", {
                "bytes_waiting": 0,
                "note": "FPGA did not transmit any data"
            }, "D")
        # #endregion
        
        # #region agent log - Reset state check (HYPOTHESIS C)
        log_entry("debug_logger.py:main", "Reset state check", {
            "note": "SW15 should be UP (not in reset) for FPGA to work",
            "assumed_not_in_reset": True,
            "warning": "If SW15 is DOWN, FPGA is in reset and will not respond"
        }, "C")
        # #endregion
        
        ser.close()
        log_entry("debug_logger.py:main", "Debug logger completed", {
            "session_complete": True
        })
        
    except serial.SerialException as e:
        log_entry("debug_logger.py:main", "Serial port error", {
            "error": str(e),
            "type": "SerialException",
            "hypothesis": "Port may not exist or be in use"
        }, "A")
    except Exception as e:
        log_entry("debug_logger.py:main", "Error in debug logger", {
            "error": str(e),
            "type": type(e).__name__
        })

if __name__ == "__main__":
    main()

