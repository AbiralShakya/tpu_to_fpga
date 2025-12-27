# Quick Testing Guide for Basys3 FPGA

This guide helps you quickly test your FPGA bitstream and UART communication.

## Prerequisites

1. **Basys3 FPGA** connected via USB
2. **Bitstream file** (`simple_test.bit` or full TPU bitstream)
3. **Python 3** with `pyserial` and `numpy`:
   ```bash
   pip3 install pyserial numpy
   ```
4. **openFPGALoader** (optional, for programming):
   ```bash
   brew install openfpgaloader
   ```

## Quick Start

### Step 1: Program the FPGA

```bash
# If you have openFPGALoader installed
openFPGALoader -b basys3 simple_test.bit

# Or if bitstream is in build directory
openFPGALoader -b basys3 build/simple_test.bit
```

**Note:** The bitstream file location:
- Root: `simple_test.bit`
- Build: `build/simple_test.bit`
- TPU: `tpu_to_fpga/build/tpu_basys3.bit`

### Step 2: Find UART Device

On Mac, the Basys3 creates two USB serial devices:
- First: JTAG (for programming)
- Second: UART (for communication) ← **Use this one!**

```bash
# List all USB serial devices
ls /dev/tty.usbserial*

# Usually the last one is UART
UART_PORT=$(ls /dev/tty.usbserial* | tail -1)
echo "Using: $UART_PORT"
```

### Step 3: Run Automated Test

```bash
# Run comprehensive test script
./test_fpga_uart.sh
```

This script will:
1. ✓ Check for bitstream file
2. ✓ Program FPGA (if openFPGALoader available)
3. ✓ Find UART device
4. ✓ Test UART communication
5. ✓ Test TPU driver (if available)

### Step 4: Manual Testing

#### Test 1: Basic UART Communication

```bash
# Find your UART device
UART_PORT=$(ls /dev/tty.usbserial* | tail -1)

# Run simple UART test
python3 test_uart_simple.py $UART_PORT
```

This tests:
- UART RX/TX
- Status register read
- Instruction write
- Execute command

#### Test 2: TPU Instructions

```bash
# Test TPU instructions (requires full TPU bitstream)
python3 test_tpu_instructions.py $UART_PORT
```

This tests:
- Basic instructions (NOP, CFG_REG, SYNC)
- Memory operations (read/write UB, weights)
- Compute operations (MATMUL)

#### Test 3: Use TPU Driver Directly

```python
from tpu_to_fpga.tpu_driver import TPU_Basys3
import numpy as np

# Connect
tpu = TPU_Basys3(port='/dev/tty.usbserial-XXXX', baud=115200)

# Read status
status = tpu.read_status()
print(f"Status: {status}")

# Write data
data = np.array([1, 2, 3, 4], dtype=np.uint8)
tpu.write_ub(0, data)

# Write instruction
tpu.write_instruction(0, tpu.OP_NOP, 0, 0, 0, 0)
tpu.start_execution()

# Wait for completion
tpu.wait_done(timeout=5)

tpu.close()
```

## UART Protocol

The TPU uses a simple UART protocol at **115200 baud**:

### Commands

| Command | Code | Description |
|---------|------|-------------|
| `CMD_WRITE_UB` | 0x01 | Write to Unified Buffer |
| `CMD_WRITE_WT` | 0x02 | Write to Weight Memory |
| `CMD_WRITE_INSTR` | 0x03 | Write instruction |
| `CMD_READ_UB` | 0x04 | Read from Unified Buffer |
| `CMD_EXECUTE` | 0x05 | Start execution |
| `CMD_READ_STATUS` | 0x06 | Read status register |

### Example: Send Status Query

```python
import serial
import time

ser = serial.Serial('/dev/tty.usbserial-XXXX', 115200, timeout=2)
time.sleep(2)  # Wait for FPGA init

# Send status query
ser.write(bytes([0x06]))
time.sleep(0.1)

# Read status byte
status = ser.read(1)
if len(status) == 1:
    print(f"Status: 0x{status[0]:02X}")
    # Bit 0: sys_busy
    # Bit 1: sys_done
    # Bit 2: vpu_busy
    # Bit 3: vpu_done
    # Bit 4: ub_busy
    # Bit 5: ub_done

ser.close()
```

## Troubleshooting

### Problem: No UART device found

```bash
# Check USB connection
system_profiler SPUSBDataType | grep -i "FTDI\|Basys"

# Check permissions
ls -l /dev/tty.usbserial*

# Fix permissions (if needed)
sudo chmod 666 /dev/tty.usbserial-XXXX
```

### Problem: Permission denied

```bash
# Add user to uucp group (Mac)
sudo dseditgroup -o edit -a $USER -t user uucp

# Or use sudo (not recommended)
sudo python3 test_uart_simple.py /dev/tty.usbserial-XXXX
```

### Problem: No response from FPGA

1. **Check power:** Is Basys3 powered on? (power LED lit)
2. **Check programming:** Is bitstream loaded? (re-program if unsure)
3. **Check device:** Are you using the UART device (second one), not JTAG?
4. **Check baud rate:** Must be 115200

### Problem: openFPGALoader not found

```bash
# Install on Mac
brew install openfpgaloader

# Or program manually with Vivado
vivado -mode tcl -source program_fpga.tcl
```

### Problem: Driver import fails

```bash
# Install Python dependencies
pip3 install pyserial numpy

# Check driver file exists
ls -la tpu_to_fpga/tpu_driver.py
```

## Understanding the Results

### Status Byte

The status byte (from `CMD_READ_STATUS`) has 6 bits:

```
Bit 5: ub_done  - Unified buffer operation complete
Bit 4: ub_busy  - Unified buffer is busy
Bit 3: vpu_done - VPU operation complete
Bit 2: vpu_busy - VPU is busy
Bit 1: sys_done - Systolic array operation complete
Bit 0: sys_busy - Systolic array is busy
```

### Visual Feedback on Basys3

**LEDs:**
- LED[15:12]: UART state
- LED[11:8]: FIFO status
- LED[7:4]: TPU status
- LED[3:0]: Pipeline status

**7-Segment Display:**
- Shows buffer occupancies (if full TPU design)
- Shows state values (if simple_test design)

## Next Steps

1. **Test with full TPU bitstream:**
   - Build TPU bitstream: `cd tpu_to_fpga && ./build_tpu_bitstream.sh`
   - Program: `openFPGALoader -b basys3 tpu_to_fpga/build/tpu_basys3.bit`
   - Test: `python3 test_tpu_instructions.py $UART_PORT`

2. **Run comprehensive test suite:**
   ```bash
   python3 test_all_instructions.py $UART_PORT
   ```

3. **Build custom applications:**
   - Use `TPU_Basys3` driver for custom inference
   - See `tpu_to_fpga/demo_tpu_complete.py` for examples

## Quick Reference

```bash
# Program FPGA
openFPGALoader -b basys3 simple_test.bit

# Find UART device
ls /dev/tty.usbserial* | tail -1

# Test UART
python3 test_uart_simple.py /dev/tty.usbserial-XXXX

# Test TPU
python3 test_tpu_instructions.py /dev/tty.usbserial-XXXX

# Run all tests
./test_fpga_uart.sh
```

## Files Created

- `test_fpga_uart.sh` - Comprehensive automated test script
- `test_uart_simple.py` - Basic UART communication test
- `test_tpu_instructions.py` - TPU instruction testing
- `QUICK_TEST_GUIDE.md` - This guide

## Support

For more details, see:
- `UART_SETUP_GUIDE.md` - Detailed UART setup
- `FPGA_PROGRAMMING_GUIDE.md` - FPGA programming guide
- `tpu_to_fpga/PROJECT_SUMMARY.md` - TPU architecture details

