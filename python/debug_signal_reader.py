#!/usr/bin/env python3
"""
TPU Debug Signal Reader

Reads and displays TPU internal debug signals to help diagnose RTL issues.
Uses UART to read debug signals exposed from the TPU datapath.

Debug signals available:
- Weight FIFO outputs: col0_wt, col1_wt, col2_wt (8 bits each)
- Activation latches: row0_act, row1_act, row2_act (8 bits each)
- Control signals: systolic_active, en_weight_pass (1 bit each)
- Accumulator outputs: acc0_latched, acc1_latched, acc2_latched (32 bits each)
"""

import serial
import time
import sys
import struct
from typing import Dict, Optional

class Cmd:
    """TPU Command Codes"""
    WRITE_UB = 0x01
    WRITE_WT = 0x02
    WRITE_INSTR = 0x03
    READ_UB = 0x04
    EXECUTE = 0x05
    READ_STATUS = 0x06
    READ_DEBUG = 0x14  # Read UART debug counters
    READ_TPU_DEBUG = 0x15  # Read TPU datapath debug signals (if implemented)

class TPUDebugReader:
    """Read TPU debug signals via UART"""

    def __init__(self, port: str):
        """Initialize serial connection"""
        try:
            self.ser = serial.Serial(port, 115200, timeout=2.0)
            time.sleep(0.1)
        except Exception as e:
            raise RuntimeError(f"Failed to connect to TPU: {e}")

    def __del__(self):
        """Cleanup serial connection"""
        if hasattr(self, 'ser') and self.ser.is_open:
            self.ser.close()

    def send_cmd(self, cmd: int, addr_hi: int = 0, addr_lo: int = 0, 
                 len_hi: int = 0, len_lo: int = 0) -> None:
        """Send command to TPU"""
        self.ser.write(bytes([cmd, addr_hi, addr_lo, len_hi, len_lo]))

    def read_status(self) -> Optional[int]:
        """Read TPU status byte"""
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.READ_STATUS)
        time.sleep(0.1)
        status = self.ser.read(1)
        return status[0] if status else None

    def read_uart_debug(self) -> Dict[str, int]:
        """Read UART debug counters (if READ_DEBUG command works)"""
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.READ_DEBUG)
        time.sleep(0.2)
        # READ_DEBUG sends multiple bytes - read them
        data = self.ser.read(32)  # Read up to 32 bytes
        if not data or len(data) < 16:
            return {}
        
        # Parse debug counters (format depends on UART implementation)
        return {
            'rx_count': struct.unpack('>I', data[0:4])[0] if len(data) >= 4 else 0,
            'tx_count': struct.unpack('>I', data[4:8])[0] if len(data) >= 8 else 0,
            'state_changes': struct.unpack('>I', data[8:12])[0] if len(data) >= 12 else 0,
            'framing_errors': struct.unpack('>I', data[12:16])[0] if len(data) >= 16 else 0,
        }

    def read_ub(self, addr: int, count: int = 32) -> bytes:
        """Read from Unified Buffer"""
        self.ser.reset_input_buffer()
        self.send_cmd(Cmd.READ_UB, 0, addr, 0, count)
        time.sleep(0.1)
        return self.ser.read(count)

    def monitor_during_execution(self, duration: float = 2.0) -> None:
        """Monitor debug signals during TPU execution"""
        print("=" * 70)
        print("TPU DEBUG SIGNAL MONITORING")
        print("=" * 70)
        print(f"Monitoring for {duration} seconds...")
        print()

        start_time = time.time()
        sample_count = 0

        while time.time() - start_time < duration:
            # Read status
            status = self.read_status()
            if status is not None:
                sys_busy = (status >> 0) & 0x01
                sys_done = (status >> 1) & 0x01
                vpu_busy = (status >> 2) & 0x01
                vpu_done = (status >> 3) & 0x01
                ub_busy = (status >> 4) & 0x01
                ub_done = (status >> 5) & 0x01
                wt_busy = (status >> 6) & 0x01
                dma_busy = (status >> 7) & 0x01

                print(f"Sample {sample_count}:")
                print(f"  Status: 0x{status:02X}")
                print(f"  sys_busy={sys_busy}, sys_done={sys_done}")
                print(f"  vpu_busy={vpu_busy}, vpu_done={vpu_done}")
                print(f"  ub_busy={ub_busy}, ub_done={ub_done}")
                print(f"  wt_busy={wt_busy}, dma_busy={dma_busy}")
                print()

            sample_count += 1
            time.sleep(0.1)

        print(f"Collected {sample_count} samples")
        print("=" * 70)

    def read_accumulator_results(self, base_addr: int = 0) -> Dict[str, int]:
        """Read accumulator results from UB (after ST_UB)"""
        print("Reading accumulator results from UB...")
        data = self.read_ub(base_addr, 32)
        
        if len(data) < 3:
            return {'acc0': 0, 'acc1': 0, 'acc2': 0}
        
        return {
            'acc0': data[0],
            'acc1': data[1],
            'acc2': data[2]
        }

    def print_debug_info(self) -> None:
        """Print current debug information"""
        print("=" * 70)
        print("TPU DEBUG INFORMATION")
        print("=" * 70)
        
        # Read status
        status = self.read_status()
        if status is not None:
            print(f"Status byte: 0x{status:02X} ({status:08b})")
            sys_busy = (status >> 0) & 0x01
            sys_done = (status >> 1) & 0x01
            vpu_busy = (status >> 2) & 0x01
            vpu_done = (status >> 3) & 0x01
            ub_busy = (status >> 4) & 0x01
            ub_done = (status >> 5) & 0x01
            wt_busy = (status >> 6) & 0x01
            dma_busy = (status >> 7) & 0x01
            
            print("\nComponent Status:")
            print(f"  Systolic Array: busy={sys_busy}, done={sys_done}")
            print(f"  VPU:            busy={vpu_busy}, done={vpu_done}")
            print(f"  Unified Buffer: busy={ub_busy}, done={ub_done}")
            print(f"  Weight FIFO:    busy={wt_busy}")
            print(f"  DMA:            busy={dma_busy}")
        else:
            print("Failed to read status")
        
        # Try to read UART debug counters
        print("\nUART Debug Counters:")
        try:
            uart_debug = self.read_uart_debug()
            if uart_debug:
                for key, value in uart_debug.items():
                    print(f"  {key}: {value}")
            else:
                print("  (READ_DEBUG command not working or not implemented)")
        except Exception as e:
            print(f"  Error reading UART debug: {e}")
        
        print("=" * 70)
        print("\nNote: TPU datapath debug signals (weights, activations, accumulators)")
        print("      are currently only available via LEDs on the FPGA.")
        print("      To read them via UART, add a READ_TPU_DEBUG command to the UART DMA.")
        print("=" * 70)

def main():
    """Main entry point"""
    if len(sys.argv) < 2:
        print("Usage: python debug_signal_reader.py <serial_port> [command]")
        print("\nCommands:")
        print("  status    - Read and display current status")
        print("  monitor   - Monitor status during execution (2 seconds)")
        print("  acc       - Read accumulator results from UB[0]")
        print("\nExample:")
        print("  python debug_signal_reader.py /dev/tty.usbserial-XXX status")
        sys.exit(1)

    port = sys.argv[1]
    command = sys.argv[2] if len(sys.argv) > 2 else "status"

    try:
        reader = TPUDebugReader(port)
        
        if command == "status":
            reader.print_debug_info()
        elif command == "monitor":
            reader.monitor_during_execution(2.0)
        elif command == "acc":
            results = reader.read_accumulator_results(0)
            print("Accumulator Results (from UB[0:2]):")
            print(f"  acc0: {results['acc0']} (0x{results['acc0']:02X})")
            print(f"  acc1: {results['acc1']} (0x{results['acc1']:02X})")
            print(f"  acc2: {results['acc2']} (0x{results['acc2']:02X})")
        else:
            print(f"Unknown command: {command}")
            sys.exit(1)

    except KeyboardInterrupt:
        print("\nInterrupted by user")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
