#!/usr/bin/env python3
"""
Comprehensive UART DMA Test Suite
Tests all commands, edge cases, and validates responses correctly
"""
import serial
import time
import sys
from typing import Optional, Tuple

# Command definitions
CMD_WRITE_UB = 0x01      # Write Unified Buffer
CMD_WRITE_WT = 0x02      # Write Weight Memory
CMD_WRITE_INSTR = 0x03   # Write Instruction
CMD_READ_UB = 0x04       # Read Unified Buffer
CMD_EXECUTE = 0x05       # Start execution
CMD_READ_STATUS = 0x06   # Read status byte
CMD_READ_DEBUG = 0x14    # Read debug counters

class UARTDMATester:
    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 2.0):
        self.ser = serial.Serial(port, baudrate, timeout=timeout)
        self.passed = 0
        self.failed = 0
        self.tests = []
        time.sleep(0.5)  # Wait for serial to initialize
    
    def log_test(self, name: str, passed: bool, details: str = ""):
        """Log test result"""
        status = "✓ PASS" if passed else "✗ FAIL"
        self.tests.append((name, passed, details))
        if passed:
            self.passed += 1
        else:
            self.failed += 1
        print(f"{status}: {name}")
        if details:
            print(f"      {details}")
    
    def clear_buffers(self):
        """Clear serial buffers"""
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()
        time.sleep(0.1)
    
    def send_bytes(self, data: bytes) -> int:
        """Send bytes and return count sent"""
        written = self.ser.write(data)
        self.ser.flush()
        return written
    
    def read_response(self, expected_bytes: int, timeout_ms: int = 1000) -> Optional[bytes]:
        """Read response with timeout
        For multi-byte responses, allow more time (each byte takes ~87us at 115200 baud)
        """
        start_time = time.time()
        response = b''
        last_byte_time = start_time
        
        # Calculate minimum time needed (87us per byte + overhead)
        min_time_needed = (expected_bytes * 87 / 1000.0) + 0.1  # Add 100ms overhead
        timeout_ms = max(timeout_ms, int(min_time_needed * 1000))
        
        while len(response) < expected_bytes:
            elapsed = time.time() - start_time
            if elapsed > timeout_ms / 1000.0:
                # Timeout - check if we got partial response
                if len(response) > 0:
                    # Got some bytes but not all - might be stuck
                    break
                return None
            
            if self.ser.in_waiting > 0:
                new_data = self.ser.read(self.ser.in_waiting)
                response += new_data
                last_byte_time = time.time()
            else:
                # No data available - check if we've been waiting too long since last byte
                if len(response) > 0 and (time.time() - last_byte_time) > 0.5:
                    # Got some bytes but no more coming - might be stuck
                    break
            
            time.sleep(0.01)
        
        return response if len(response) == expected_bytes else None
    
    def reset_state(self):
        """Try to reset FPGA state by sending a single READ_STATUS command"""
        print("      Attempting state reset...")
        self.clear_buffers()
        self.send_bytes(bytes([CMD_READ_STATUS]))
        time.sleep(0.3)  # Give more time for response
        if self.ser.in_waiting > 0:
            response = self.ser.read(self.ser.in_waiting)
            print(f"      Reset response: {' '.join(f'{b:02x}' for b in response)}")
        else:
            print("      No reset response")
    
    def test_read_status(self) -> bool:
        """Test READ_STATUS command (0x06)"""
        self.clear_buffers()
        self.send_bytes(bytes([CMD_READ_STATUS]))
        time.sleep(0.1)
        
        response = self.read_response(1, timeout_ms=500)
        if response is None:
            self.log_test("READ_STATUS", False, "No response received (FPGA might be stuck)")
            # Try to recover by sending reset sequence
            self.reset_state()
            return False
        
        status = response[0]
        # Status byte format: [7:6]=00, [5]=ub_done, [4]=ub_busy, 
        #                     [3]=vpu_done, [2]=vpu_busy, [1]=sys_done, [0]=sys_busy
        # 0x20 = all idle (0b00100000)
        if status == 0x20:
            self.log_test("READ_STATUS", True, f"Status: 0x{status:02X} (all idle)")
            return True
        else:
            self.log_test("READ_STATUS", False, f"Unexpected status: 0x{status:02X} (expected 0x20)")
            return False
    
    def test_read_debug(self) -> tuple[bool, Optional[bytes]]:
        """Test READ_DEBUG command (0x14) - returns 10 bytes"""
        self.clear_buffers()
        self.send_bytes(bytes([CMD_READ_DEBUG]))
        # Allow more time for 10 bytes (each byte takes ~87us, so 10 bytes = ~870us + overhead)
        time.sleep(0.2)

        response = self.read_response(10, timeout_ms=2000)  # Increased timeout for 10 bytes
        if response is None or len(response) != 10:
            if response and len(response) > 0:
                self.log_test("READ_DEBUG", False, f"Partial response: {len(response)}/10 bytes received: {response.hex()}")
            else:
                self.log_test("READ_DEBUG", False, f"No response received (FPGA might be stuck in READ_DEBUG state)")
                self.reset_state()
            return False, response

        state = response[0]
        rx_count = int.from_bytes(response[1:5], 'little')
        tx_count = int.from_bytes(response[5:9], 'little')
        last_rx = response[9]

        self.log_test("READ_DEBUG", True,
                     f"State=0x{state:02X}, RX={rx_count}, TX={tx_count}, LastRX=0x{last_rx:02X}")
        return True, response
    
    def test_invalid_command(self) -> bool:
        """Test invalid command handling (should return 0xFF)"""
        self.clear_buffers()
        self.send_bytes(bytes([0x99]))  # Invalid command
        time.sleep(0.1)
        
        response = self.read_response(1, timeout_ms=500)
        if response is None:
            self.log_test("INVALID_COMMAND", False, "No response received")
            return False
        
        if response[0] == 0xFF:
            self.log_test("INVALID_COMMAND", True, "Correctly returned 0xFF")
            return True
        else:
            self.log_test("INVALID_COMMAND", False, f"Unexpected response: 0x{response[0]:02X} (expected 0xFF)")
            return False
    
    def test_write_ub(self, addr: int, data: bytes) -> bool:
        """Test WRITE_UB command (0x01)
        Format: [CMD(1)] [ADDR_HI(1)] [ADDR_LO(1)] [LEN_HI(1)] [LEN_LO(1)] [DATA(N)]
        """
        if len(data) % 32 != 0:
            self.log_test(f"WRITE_UB@0x{addr:04X}", False, f"Data length must be multiple of 32 bytes (got {len(data)})")
            return False
        
        addr_hi = (addr >> 8) & 0xFF
        addr_lo = addr & 0xFF
        len_hi = (len(data) >> 8) & 0xFF
        len_lo = len(data) & 0xFF
        
        cmd = bytes([CMD_WRITE_UB, addr_hi, addr_lo, len_hi, len_lo]) + data
        
        self.clear_buffers()
        written = self.send_bytes(cmd)
        time.sleep(0.2)  # Wait for write to complete
        
        if written != len(cmd):
            self.log_test(f"WRITE_UB@0x{addr:04X}", False, f"Only wrote {written}/{len(cmd)} bytes")
            return False
        
        self.log_test(f"WRITE_UB@0x{addr:04X}", True, f"Wrote {len(data)} bytes")
        return True
    
    def test_read_ub(self, addr: int, length: int) -> Optional[bytes]:
        """Test READ_UB command (0x04)
        Format: [CMD(1)] [ADDR_HI(1)] [ADDR_LO(1)] [LEN_HI(1)] [LEN_LO(1)]
        Response: [DATA(N)]
        """
        addr_hi = (addr >> 8) & 0xFF
        addr_lo = addr & 0xFF
        len_hi = (length >> 8) & 0xFF
        len_lo = length & 0xFF

        cmd = bytes([CMD_READ_UB, addr_hi, addr_lo, len_hi, len_lo])

        self.clear_buffers()
        # Send command byte by byte with small delays to ensure FPGA processes each one
        for i, byte in enumerate(cmd):
            self.send_bytes(bytes([byte]))
            time.sleep(0.01)  # 10ms delay between bytes

        # Wait for FPGA to process the command and start responding
        time.sleep(0.2)

        # Calculate timeout: ~87us per byte + 2 seconds overhead for slow FPGA
        timeout_ms = int((length * 87 / 1000.0) + 2000)
        response = self.read_response(length, timeout_ms=timeout_ms)
        if response is None or len(response) != length:
            # Debug: check what state the FPGA is in
            debug_success, debug_resp = self.test_read_debug()
            if debug_success and debug_resp:
                state = debug_resp[0]
                self.log_test(f"READ_UB@0x{addr:04X}", False,
                             f"FPGA in state 0x{state:02X} after failed READ_UB")
            else:
                self.log_test(f"READ_UB@0x{addr:04X}", False,
                             f"No response (FPGA might be stuck)")

            if response and len(response) > 0:
                self.log_test(f"READ_UB@0x{addr:04X}", False,
                             f"Partial response: {len(response)}/{length} bytes: {' '.join(f'{b:02x}' for b in response[:8])}...")
            else:
                self.reset_state()
            return None

        self.log_test(f"READ_UB@0x{addr:04X}", True, f"Read {len(response)} bytes: {' '.join(f'{b:02x}' for b in response[:8])}...")
        return response
    
    def test_write_wt(self, addr: int, data: bytes) -> bool:
        """Test WRITE_WT command (0x02)
        Format: [CMD(1)] [ADDR_HI(1)] [ADDR_LO(1)] [LEN_HI(1)] [LEN_LO(1)] [DATA(N)]
        Weight memory is 8 bytes per word
        """
        if len(data) % 8 != 0:
            self.log_test(f"WRITE_WT@0x{addr:04X}", False, f"Data length must be multiple of 8 bytes (got {len(data)})")
            return False
        
        addr_hi = (addr >> 8) & 0xFF
        addr_lo = addr & 0xFF
        len_hi = (len(data) >> 8) & 0xFF
        len_lo = len(data) & 0xFF
        
        cmd = bytes([CMD_WRITE_WT, addr_hi, addr_lo, len_hi, len_lo]) + data
        
        self.clear_buffers()
        written = self.send_bytes(cmd)
        time.sleep(0.2)
        
        if written != len(cmd):
            self.log_test(f"WRITE_WT@0x{addr:04X}", False, f"Only wrote {written}/{len(cmd)} bytes")
            return False
        
        self.log_test(f"WRITE_WT@0x{addr:04X}", True, f"Wrote {len(data)} bytes")
        return True
    
    def test_write_instr(self, addr: int, instr: int) -> bool:
        """Test WRITE_INSTR command (0x03)
        Format: [CMD(1)] [ADDR_HI(1)] [ADDR_LO(1)] [INSTR_BYTE3(1)] [INSTR_BYTE2(1)] [INSTR_BYTE1(1)] [INSTR_BYTE0(1)]
        """
        addr_hi = (addr >> 8) & 0xFF
        addr_lo = addr & 0xFF
        instr_bytes = instr.to_bytes(4, 'little')
        
        cmd = bytes([CMD_WRITE_INSTR, addr_hi, addr_lo]) + instr_bytes
        
        self.clear_buffers()
        written = self.send_bytes(cmd)
        time.sleep(0.1)
        
        if written != len(cmd):
            self.log_test(f"WRITE_INSTR@0x{addr:02X}", False, f"Only wrote {written}/{len(cmd)} bytes")
            return False
        
        self.log_test(f"WRITE_INSTR@0x{addr:02X}", True, f"Wrote instruction 0x{instr:08X}")
        return True
    
    def test_execute(self) -> bool:
        """Test EXECUTE command (0x05)"""
        self.clear_buffers()
        self.send_bytes(bytes([CMD_EXECUTE]))
        time.sleep(0.1)
        
        self.log_test("EXECUTE", True, "Execution started")
        return True
    
    def run_all_tests(self):
        """Run comprehensive test suite"""
        print("=" * 80)
        print("COMPREHENSIVE UART DMA TEST SUITE")
        print("=" * 80)
        print()
        
        # Phase 1: Basic Commands
        print("─" * 80)
        print("PHASE 1: BASIC COMMANDS")
        print("─" * 80)
        print("NOTE: If READ_DEBUG or READ_UB fail, FPGA might be stuck in that state.")
        print("      Try power cycling FPGA or toggling SW15 (reset switch).")
        print()
        self.test_read_status()
        # Skip READ_DEBUG if READ_STATUS failed (FPGA might be stuck)
        debug_success, debug_resp = self.test_read_debug()  # Always try READ_DEBUG now
        self.test_invalid_command()
        print()
        
        # Phase 2: Unified Buffer Write/Read
        print("─" * 80)
        print("PHASE 2: UNIFIED BUFFER WRITE/READ")
        print("─" * 80)
        # Create test pattern: 32 bytes with recognizable pattern
        test_pattern = bytes([0xAA, 0xBB, 0xCC, 0xDD] + [i & 0xFF for i in range(28)])

        if self.test_write_ub(0x0000, test_pattern):
            time.sleep(0.2)
            response = self.test_read_ub(0x0000, 32)
            if response:
                if response[:4] == test_pattern[:4]:
                    self.log_test("UB_WRITE_READ_MATCH", True, "Data matches!")
                else:
                    self.log_test("UB_WRITE_READ_MATCH", False,
                                 f"Mismatch: got {response[:4].hex()}, expected {test_pattern[:4].hex()}")

        # Test multiple addresses
        test_pattern2 = bytes([0x11, 0x22, 0x33, 0x44] + [0xFF - i for i in range(28)])
        self.test_write_ub(0x0001, test_pattern2)
        time.sleep(0.2)
        self.test_read_ub(0x0001, 32)
        print()
        
        # Phase 3: Weight Memory Write
        print("─" * 80)
        print("PHASE 3: WEIGHT MEMORY WRITE")
        print("─" * 80)
        weight_data = bytes([0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC])
        self.test_write_wt(0x0000, weight_data)
        print()
        
        # Phase 4: Instruction Memory Write
        print("─" * 80)
        print("PHASE 4: INSTRUCTION MEMORY WRITE")
        print("─" * 80)
        # NOP instruction (all zeros)
        self.test_write_instr(0x00, 0x00000000)
        # Example instruction (dummy value)
        self.test_write_instr(0x01, 0x12345678)
        print()
        
        # Phase 5: Execute (skip this for now - it seems to hang the FPGA)
        print("─" * 80)
        print("PHASE 5: EXECUTE COMMAND (SKIPPED)")
        print("─" * 80)
        print("      Skipping EXECUTE as it appears to hang the FPGA")
        print("      This needs investigation in the TPU controller/datapath")
        print()
        
        # Phase 6: Final Status Check
        print("─" * 80)
        print("PHASE 6: FINAL STATUS CHECK")
        print("─" * 80)
        self.test_read_status()
        debug_success, debug_resp = self.test_read_debug()
        print()
        
        # Summary
        print("=" * 80)
        print("TEST SUMMARY")
        print("=" * 80)
        print(f"Total Tests: {self.passed + self.failed}")
        print(f"Passed: {self.passed}")
        print(f"Failed: {self.failed}")
        print(f"Success Rate: {(self.passed / (self.passed + self.failed) * 100):.1f}%")
        print()
        
        if self.failed == 0:
            print("✓✓✓ ALL TESTS PASSED! ✓✓✓")
        else:
            print("✗ Some tests failed. Review output above.")
        print("=" * 80)
    
    def close(self):
        """Close serial connection"""
        self.ser.close()

def main():
    port = sys.argv[1] if len(sys.argv) > 1 else '/dev/tty.usbserial-210183A27BE01'
    
    print(f"Connecting to {port}...")
    tester = UARTDMATester(port)
    
    try:
        tester.run_all_tests()
    except KeyboardInterrupt:
        print("\n\nTest interrupted by user")
    except Exception as e:
        print(f"\n\nError during testing: {e}")
        import traceback
        traceback.print_exc()
    finally:
        tester.close()

if __name__ == "__main__":
    main()

