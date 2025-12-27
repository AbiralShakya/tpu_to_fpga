#!/usr/bin/env python3
"""
Bank Selection Diagnostic Test
Confirms the hypothesis that bank selection mismatch causes ISA integration failures.

Test Strategy:
1. Write known pattern to UB via UART
2. Read back via UART (should work - same bank)
3. Execute minimal ISA program that reads then writes
4. Read back via UART (will fail if banks mismatched)
5. Compare results to identify which bank has data
"""

import serial
import time
import struct
import sys

class BankDiagnostic:
    # UART Commands
    CMD_WRITE_UB    = 0x01
    CMD_WRITE_WT    = 0x02
    CMD_WRITE_INSTR = 0x03
    CMD_READ_UB     = 0x04
    CMD_EXECUTE     = 0x05
    CMD_READ_STATUS = 0x06

    # ISA Opcodes
    OP_NOP      = 0x00
    OP_LD_UB    = 0x04
    OP_ST_UB    = 0x05
    OP_SYNC     = 0x30
    OP_HALT     = 0x3F

    def __init__(self, port, baud=115200, timeout=2):
        self.ser = serial.Serial(port, baud, timeout=timeout)
        time.sleep(0.1)
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()
        print(f"Connected to {port}")

    def close(self):
        self.ser.close()

    def write_ub(self, addr, data):
        """Write 32 bytes to UB"""
        if len(data) < 32:
            data = data + bytes(32 - len(data))
        elif len(data) > 32:
            data = data[:32]

        # Command: 0x01, addr_hi, addr_lo, len_hi, len_lo
        cmd = bytes([self.CMD_WRITE_UB, 0x00, addr, 0x00, 0x20])  # 32 bytes
        self.ser.write(cmd)
        self.ser.write(data)

        # Wait for ACK
        time.sleep(0.05)
        ack = self.ser.read(1)
        return ack == b'\xAA'

    def read_ub(self, addr, length=32):
        """Read from UB"""
        cmd = bytes([self.CMD_READ_UB, 0x00, addr, 0x00, length])
        self.ser.write(cmd)
        time.sleep(0.05)
        data = self.ser.read(length)
        return data

    def write_instruction(self, addr, opcode, arg1, arg2, arg3, flags=0):
        """Write single instruction"""
        instr = (opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags
        cmd = bytes([self.CMD_WRITE_INSTR, 0x00, addr])
        self.ser.write(cmd)
        self.ser.write(struct.pack('>I', instr))
        time.sleep(0.01)

    def execute(self):
        """Start execution"""
        self.ser.write(bytes([self.CMD_EXECUTE]))
        time.sleep(0.1)

    def read_status(self):
        """Read status byte"""
        self.ser.write(bytes([self.CMD_READ_STATUS]))
        time.sleep(0.02)
        status = self.ser.read(1)
        if status:
            s = status[0]
            return {
                'sys_busy': bool(s & 0x01),
                'sys_done': bool(s & 0x02),
                'vpu_busy': bool(s & 0x04),
                'vpu_done': bool(s & 0x08),
                'ub_busy':  bool(s & 0x10),
                'ub_done':  bool(s & 0x20),
                'raw': hex(s)
            }
        return None

    def run_diagnostic(self):
        """Run the bank selection diagnostic"""
        print("\n" + "="*70)
        print("BANK SELECTION DIAGNOSTIC TEST")
        print("="*70)

        # Test pattern: recognizable bytes
        test_pattern = bytes([0xDE, 0xAD, 0xBE, 0xEF] + [i for i in range(28)])
        print(f"\nTest pattern (first 8 bytes): {test_pattern[:8].hex()}")

        # ============================================================
        # TEST 1: UART Write then UART Read (baseline)
        # ============================================================
        print("\n[TEST 1] UART Write -> UART Read (same bank expected)")

        if self.write_ub(0x00, test_pattern):
            print("  Write: OK (ACK received)")
        else:
            print("  Write: FAILED (no ACK)")
            return False

        readback = self.read_ub(0x00, 32)
        if readback == test_pattern:
            print(f"  Read:  OK - Pattern matches!")
        else:
            print(f"  Read:  MISMATCH!")
            print(f"    Expected: {test_pattern[:8].hex()}...")
            print(f"    Got:      {readback[:8].hex()}...")
            if readback == bytes(32):
                print("    -> All zeros! Data went to wrong bank or not written.")

        # ============================================================
        # TEST 2: Check status before ISA execution
        # ============================================================
        print("\n[TEST 2] Status check before ISA execution")
        status = self.read_status()
        print(f"  Status: {status}")

        # ============================================================
        # TEST 3: Minimal ISA program - just NOP and HALT
        # ============================================================
        print("\n[TEST 3] Minimal ISA: NOP -> HALT")
        self.write_instruction(0, self.OP_NOP, 0, 0, 0, 0)
        self.write_instruction(1, self.OP_HALT, 0, 0, 0, 0)

        self.execute()
        time.sleep(0.1)

        status = self.read_status()
        print(f"  Status after execution: {status}")

        # Read back data - should still be there
        readback = self.read_ub(0x00, 32)
        if readback == test_pattern:
            print(f"  Data intact after NOP/HALT: OK")
        else:
            print(f"  Data CHANGED after NOP/HALT!")
            print(f"    Got: {readback[:8].hex()}...")

        # ============================================================
        # TEST 4: ISA with LD_UB (read operation)
        # ============================================================
        print("\n[TEST 4] ISA with LD_UB (read from UB)")

        # Program: LD_UB from addr 0, then HALT
        # LD_UB: opcode=0x04, arg1=ub_addr, arg2=count, arg3=0
        self.write_instruction(0, self.OP_LD_UB, 0x00, 0x01, 0x00, 0)  # Read 1 block from addr 0
        self.write_instruction(1, self.OP_HALT, 0, 0, 0, 0)

        self.execute()
        time.sleep(0.1)

        status = self.read_status()
        print(f"  Status after LD_UB: {status}")

        # Read back - should still be there (LD_UB doesn't modify data)
        readback = self.read_ub(0x00, 32)
        if readback == test_pattern:
            print(f"  Data intact after LD_UB: OK")
            print("  -> LD_UB reads from CORRECT bank (bank 0)")
        else:
            print(f"  Data CHANGED after LD_UB!")
            print(f"    Got: {readback[:8].hex()}...")

        # ============================================================
        # TEST 5: ISA with SYNC (toggles bank selection!)
        # ============================================================
        print("\n[TEST 5] ISA with SYNC (bank toggle instruction)")

        # Re-write pattern first
        self.write_ub(0x00, test_pattern)

        # Program: SYNC -> HALT
        # SYNC toggles ub_buf_sel_reg in the controller!
        self.write_instruction(0, self.OP_SYNC, 0x0F, 0x00, 0x10, 0)  # Wait for all, timeout
        self.write_instruction(1, self.OP_HALT, 0, 0, 0, 0)

        self.execute()
        time.sleep(0.2)  # SYNC might take longer

        status = self.read_status()
        print(f"  Status after SYNC: {status}")

        # Read back - will this work after bank toggle?
        readback = self.read_ub(0x00, 32)
        if readback == test_pattern:
            print(f"  Data intact after SYNC: OK")
            print("  -> Bank toggle did NOT affect UART reads (expected)")
        else:
            print(f"  Data read after SYNC:")
            print(f"    Got: {readback[:8].hex()}...")
            if readback == bytes(32):
                print("    -> All zeros! SYNC may have toggled controller's bank view")

        # ============================================================
        # TEST 6: Write via UART, execute LD_UB+SYNC+LD_UB, read via UART
        # ============================================================
        print("\n[TEST 6] Full sequence: UART Write -> LD_UB -> SYNC -> LD_UB -> UART Read")

        # Re-write pattern
        self.write_ub(0x00, test_pattern)
        print("  Wrote pattern to addr 0")

        # Program: LD_UB -> SYNC -> LD_UB -> HALT
        self.write_instruction(0, self.OP_LD_UB, 0x00, 0x01, 0x00, 0)  # Read addr 0
        self.write_instruction(1, self.OP_SYNC, 0x0F, 0x00, 0x10, 0)  # Sync + toggle
        self.write_instruction(2, self.OP_LD_UB, 0x00, 0x01, 0x00, 0)  # Read addr 0 again
        self.write_instruction(3, self.OP_HALT, 0, 0, 0, 0)

        self.execute()
        time.sleep(0.3)

        status = self.read_status()
        print(f"  Final status: {status}")

        readback = self.read_ub(0x00, 32)
        if readback == test_pattern:
            print(f"  Final read: Pattern matches!")
        else:
            print(f"  Final read: {readback[:8].hex()}...")
            if readback == bytes(32):
                print("    -> All zeros after ISA sequence!")
                print("    CONFIRMS: Bank mismatch between UART and ISA controller")

        # ============================================================
        # TEST 7: Try reading from both addresses (bank test)
        # ============================================================
        print("\n[TEST 7] Read from addr 0 and addr 128 (different bank regions)")

        # Write distinct patterns
        pattern_a = bytes([0xAA] * 32)
        pattern_b = bytes([0xBB] * 32)

        self.write_ub(0x00, pattern_a)
        print("  Wrote 0xAA pattern to addr 0")

        self.write_ub(0x40, pattern_b)  # addr 64
        print("  Wrote 0xBB pattern to addr 64")

        read_0 = self.read_ub(0x00, 32)
        read_64 = self.read_ub(0x40, 32)

        print(f"  Read addr 0:  {read_0[:4].hex()}... (expected AAAAAAAA)")
        print(f"  Read addr 64: {read_64[:4].hex()}... (expected BBBBBBBB)")

        # ============================================================
        # SUMMARY
        # ============================================================
        print("\n" + "="*70)
        print("DIAGNOSTIC SUMMARY")
        print("="*70)
        print("""
Key observations to check:
1. If TEST 1 fails: UART path is broken
2. If TEST 3 fails: NOP/HALT corrupts data (shouldn't happen)
3. If TEST 4 fails: LD_UB reads from wrong bank
4. If TEST 5 fails: SYNC affects UART's bank view
5. If TEST 6 fails: Bank toggles during ISA execution break subsequent reads
6. If TEST 7 fails: Address space fragmented by bank selection

Root cause likely: Controller's ub_buf_sel_reg starts at 0, but UART
writes with ub_buf_sel=1 (which makes wr_bank_sel=0). After SYNC,
controller toggles to ub_buf_sel_reg=1, causing reads from wrong bank.
        """)

        return True


def main():
    if len(sys.argv) < 2:
        print("Usage: python test_bank_diagnostic.py <UART_PORT>")
        print("Example: python test_bank_diagnostic.py /dev/tty.usbserial-xxxx")
        sys.exit(1)

    port = sys.argv[1]

    try:
        diag = BankDiagnostic(port)
        diag.run_diagnostic()
        diag.close()
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nInterrupted")
        sys.exit(1)


if __name__ == '__main__':
    main()
