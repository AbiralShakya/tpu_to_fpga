# TPU Coprocessor UART Setup and Testing Guide
## Basys 3 FPGA + Mac Laptop

This guide walks you through setting up the TPU coprocessor streaming interface via UART, from hardware connection to running the full 20-instruction test suite.

---

## Table of Contents

1. [Hardware Requirements](#hardware-requirements)
2. [Software Requirements](#software-requirements)
3. [Hardware Setup](#hardware-setup)
4. [Software Installation](#software-installation)
5. [FPGA Programming](#fpga-programming)
6. [UART Connection Verification](#uart-connection-verification)
7. [Running Tests](#running-tests)
8. [Troubleshooting](#troubleshooting)
9. [Understanding the Results](#understanding-the-results)

---

## 1. Hardware Requirements

### Required Hardware
- **Basys 3 Artix-7 FPGA Board** (Digilent)
- **USB A to Micro-B cable** (for FPGA power and programming)
- **Mac laptop** (macOS 10.15+)
- **OPTIONAL:** USB-UART adapter if using external UART (Basys 3 has built-in USB-UART)

### Basys 3 Specifications
- FPGA: Xilinx Artix-7 XC7A35T-1CPG236C
- USB-UART Bridge: Built-in FTDI FT2232HQ
- Clock: 100 MHz oscillator
- UART Pins:
  - **TX:** Pin B18 (FPGA → PC)
  - **RX:** Pin A18 (PC → FPGA)

---

## 2. Software Requirements

### macOS Tools

1. **Python 3.8+**
   ```bash
   python3 --version  # Should be 3.8 or higher
   ```

2. **pyserial library**
   ```bash
   pip3 install pyserial
   ```

3. **numpy library**
   ```bash
   pip3 install numpy
   ```

4. **FPGA Toolchain** (choose one):
   - **Vivado** (Xilinx - recommended for Basys 3)
   - **OR** open-source: Yosys + nextpnr + Vivado for bitstream generation

5. **Serial Port Driver**
   - macOS usually includes FTDI drivers automatically
   - If not detected, install from: https://ftdichip.com/drivers/vcp-drivers/

---

## 3. Hardware Setup

### Step 1: Connect the Basys 3 Board

1. **Power OFF** the Basys 3
2. Connect **USB cable** from Mac to **Basys 3 PROG/UART port** (top micro-USB port)
3. Set **Power Select Jumper** to USB
4. **Power ON** the Basys 3 (flip power switch to ON)

### Step 2: Verify FPGA Detection

```bash
# Check if FPGA is detected
ls /dev/tty.usbserial*
```

You should see two devices:
- `/dev/tty.usbserial-XXXXXXX1` - Programming interface (JTAG)
- `/dev/tty.usbserial-XXXXXXX2` - UART interface ← **This is what we use!**

**Note the UART device name** - you'll need it later.

Example:
```
/dev/tty.usbserial-210292B2C3A3
/dev/tty.usbserial-210292B2C3A4  ← UART (second one)
```

---

## 4. Software Installation

### Step 1: Clone/Navigate to TPU Repository

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga
```

### Step 2: Verify Python Driver Files

Make sure these files exist:
```bash
ls -la tpu_coprocessor_driver.py
ls -la instruction_encoder.py
ls -la example_coprocessor_usage.py
ls -la test_all_instructions.py
```

### Step 3: Install Python Dependencies

```bash
pip3 install pyserial numpy
```

### Step 4: Test Python Driver Import

```bash
python3 -c "from tpu_coprocessor_driver import TPU_Coprocessor; print('✓ Driver import successful')"
```

---

## 5. FPGA Programming

### Method A: Using Vivado (Recommended)

#### Step 1: Synthesize the Bitstream

```bash
cd tpu_to_fpga  # Inner subfolder

# Option 1: Use Vivado GUI
vivado &
# Open project → Synthesize → Implement → Generate Bitstream

# Option 2: Use TCL script
vivado -mode batch -source create_vivado_project.tcl
```

#### Step 2: Program the FPGA

```bash
# Via Vivado Hardware Manager
# Tools → Open Hardware Manager → Open Target → Auto Connect
# Right-click on XC7A35T → Program Device → Select bitstream (.bit file)
```

**Bitstream location:** `tpu_to_fpga/vivado_project/tpu.runs/impl_1/tpu_top.bit`

### Method B: Using Open-Source Tools

```bash
cd tpu_to_fpga

# Synthesize with Yosys
yosys synth_basys3.ys

# Place and route (requires Vivado for Xilinx bitstream generation)
# This is complex - Vivado is recommended for Basys 3
```

### Step 3: Verify Programming

After programming, the Basys 3 LEDs should show:
- **LED[15:12]:** UART state (should be `0000` initially = IDLE)
- **LED[11:8]:** FIFO status (should be `0000` initially)
- **LED[7:4]:** TPU status (should be `0000` = not busy)
- **LED[3:0]:** Pipeline status

The **7-segment display** should show: `0000` (FIFO buffer occupancies)

---

## 6. UART Connection Verification

### Step 1: Find UART Device

```bash
ls /dev/tty.usbserial* | tail -1
```

Save this device name. Example: `/dev/tty.usbserial-210292B2C3A4`

### Step 2: Test UART Loopback (Optional)

```bash
# Send a simple command to test UART RX/TX
python3 << 'EOF'
import serial
import time

port = '/dev/tty.usbserial-210183A27BE01'  # ← Use your device name
ser = serial.Serial(port, 115200, timeout=2)
time.sleep(0.1)

# Send status query (command 0x06)
ser.write(bytes([0x06]))
time.sleep(0.1)

# Read status byte
status = ser.read(1)
if len(status) == 1:
    print(f"✓ UART working! Status byte: 0x{status[0]:02X}")
    print(f"  sys_busy={status[0] & 0x01}, sys_done={(status[0] >> 1) & 0x01}")
else:
    print("✗ No response from UART")

ser.close()
EOF
```

Expected output:
```
✓ UART working! Status byte: 0x00
  sys_busy=0, sys_done=0
```

### Step 3: Test Python Driver Connection

```bash
python3 << 'EOF'
from tpu_coprocessor_driver import TPU_Coprocessor
import time

port = '/dev/tty.usbserial-210292B2C3A4'  # ← Use your device name
tpu = TPU_Coprocessor(port)

# Read status
status = tpu.read_status()
print(f"✓ TPU connected!")
print(f"  Status: {status}")
print(f"  sys_busy={status.sys_busy}, vpu_busy={status.vpu_busy}")

tpu.close()
EOF
```

---

## 7. Running Tests

### Test 1: Simple NOP Test

This sends a few NOP instructions to verify streaming mode works.

```bash
python3 << 'EOF'
from tpu_coprocessor_driver import TPU_Coprocessor, Instruction, Opcode

port = '/dev/tty.usbserial-210292B2C3A4'  # ← Use your device name
tpu = TPU_Coprocessor(port)

print("Enabling streaming mode...")
tpu.enable_stream_mode()

print("Streaming 3 NOP instructions...")
for i in range(3):
    instr = Instruction(Opcode.NOP, 0, 0, 0, 0)
    tpu.stream_instruction(instr)
    print(f"  NOP {i+1}/3 sent")

print("Sending HALT...")
tpu.stream_instruction(Instruction(Opcode.HALT, 0, 0, 0, 0))

print("Waiting for completion...")
if tpu.wait_done(timeout=2.0):
    print("✓ Test PASSED - TPU executed and halted")
else:
    print("✗ Test FAILED - timeout")

stats = tpu.get_statistics()
print(f"\nStatistics:")
print(f"  Instructions sent: {stats['instructions_sent']}")
print(f"  Bytes sent: {stats['bytes_sent']}")
print(f"  Bytes received: {stats['bytes_received']}")

tpu.close()
EOF
```

Expected output:
```
Enabling streaming mode...
Streaming 3 NOP instructions...
  NOP 1/3 sent
  NOP 2/3 sent
  NOP 3/3 sent
Sending HALT...
Waiting for completion...
✓ Test PASSED - TPU executed and halted

Statistics:
  Instructions sent: 4
  Bytes sent: 17
  Bytes received: 5
```

### Test 2: Matrix Multiplication Test

```bash
python3 << 'EOF'
from tpu_coprocessor_driver import TPU_Coprocessor, Instruction, Opcode
import numpy as np

port = '/dev/tty.usbserial-210292B2C3A4'  # ← Use your device name
tpu = TPU_Coprocessor(port)

# Create 3x3 identity matrix test
A = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]], dtype=np.int8)
B = np.eye(3, dtype=np.int8)  # 3x3 identity matrix

print(f"Matrix A (3x3):\n{A}\n")
print(f"Matrix B (3x3 identity):\n{B}\n")
print(f"Expected result:\n{A @ B}\n")  # Should be same as A

# Load data
print("Loading matrices...")
tpu.write_ub(0x00, A.tobytes())
tpu.write_weights(0x00, B.tobytes())

# Stream instructions
print("Streaming MATMUL instructions...")
tpu.enable_stream_mode()

# Load weights
tpu.stream_instruction(Instruction(Opcode.RD_WEIGHT, 0x00, 0x01, 0x00, 0))

# Load activations
tpu.stream_instruction(Instruction(Opcode.LD_UB, 0x00, 0x01, 0x00, 0))

# Execute MATMUL (3x3, signed)
tpu.stream_instruction(Instruction(Opcode.MATMUL, 0x00, 0x00, 0x03, 0b10))

# Sync and halt
tpu.stream_instruction(Instruction(Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0))
tpu.stream_instruction(Instruction(Opcode.HALT, 0, 0, 0, 0))

print("Executing MATMUL on TPU...")
if tpu.wait_done(timeout=5.0):
    print("✓ MATMUL test PASSED")
else:
    print("✗ MATMUL test FAILED - timeout")

tpu.close()
EOF
```

### Test 3: Full 20-Instruction Test Suite

This is the comprehensive test that validates all instructions:

```bash
# Replace with your actual UART device
export TPU_PORT="/dev/tty.usbserial-210292B2C3A4"

python3 test_all_instructions.py $TPU_PORT
```

**Expected runtime:** ~60-90 seconds

You should see colorful output testing each instruction:

```
================================================================================
Test 1/20: NOP - No Operation
================================================================================

ℹ Streaming mode enabled
ℹ Sent NOP instruction 1/5
ℹ Sent NOP instruction 2/5
...
✓ NOP test passed - TPU executed and halted

================================================================================
Test 2/20: SYNC - Synchronize Operations
================================================================================
...

[... all 20 tests ...]

================================================================================
Test Summary
================================================================================

Total Tests:   20
Tests Passed:  20
Tests Failed:  0

Pass Rate:     100.0%

TPU Statistics:
Bytes Sent:       842
Bytes Received:   123
Instructions:     87
Errors:           0

✓ ALL TESTS PASSED!
```

---

## 8. Troubleshooting

### Problem: No `/dev/tty.usbserial*` devices

**Solution:**
```bash
# Check if USB device is detected
system_profiler SPUSBDataType | grep -A 10 "Basys3"

# Install FTDI VCP drivers if needed
# Download from: https://ftdichip.com/drivers/vcp-drivers/
```

### Problem: `Permission denied` on `/dev/tty.usbserial*`

**Solution:**
```bash
# Add yourself to dialout group (may require logout/login)
sudo dseditgroup -o edit -a $USER -t user uucp

# Or use sudo (not recommended for production)
sudo python3 test_all_instructions.py /dev/tty.usbserial-XXX
```

### Problem: UART connection but no response

**Checklist:**
1. ✓ Is FPGA powered on? (power LED lit)
2. ✓ Is bitstream programmed? (run programming step again)
3. ✓ Are you using the **second** USB serial device? (the UART one, not JTAG)
4. ✓ Is baud rate correct? (115200)

**Debug UART:**
```bash
# Try raw serial communication
screen /dev/tty.usbserial-XXX 115200
# Press Ctrl-A, K to exit

# Or use minicom
brew install minicom
minicom -D /dev/tty.usbserial-XXX -b 115200
```

### Problem: Streaming mode enable fails (0xFF response)

This means the instruction FIFO is not ready.

**Solution:**
```bash
# Reset the FPGA
# 1. Power cycle the Basys 3 (OFF then ON)
# 2. Or re-program the bitstream
# 3. Try again
```

### Problem: Instructions timeout

Check the **7-segment display** and **LEDs** on Basys 3:

- **7-seg:** Shows FIFO buffer occupancy in hex
  - Digits 3-2: Write buffer occupancy (should increase as you stream)
  - Digits 1-0: Read buffer occupancy (should decrease as TPU executes)

- **LEDs:**
  - LED[15:12]: UART state (0=IDLE, 1=CMD, 7=STREAM, etc.)
  - LED[11]: FIFO exec active (should be 1 during execution)
  - LED[10]: FIFO write buffer full (should be 0)
  - LED[8]: Stream mode enabled (should be 1 in streaming)
  - LED[7]: sys_busy (1=systolic array is computing)

**Debug:**
```python
# Add verbose logging to driver
tpu = TPU_Coprocessor(port, verbose=True)  # If driver supports it

# Or add print statements to see status
status = tpu.read_status()
print(f"sys_busy={status.sys_busy}, vpu_busy={status.vpu_busy}")
```

### Problem: Matrix multiply gives wrong results

The systolic array is 3x3, so:
- Matrices must be **3x3 or smaller**
- Data must be **signed 8-bit integers** (`np.int8`)
- Weights must be loaded **before** activations
- SYNC instruction must wait for completion before reading results

---

## 9. Understanding the Results

### UART Protocol Flow

1. **Enable Streaming:**
   ```
   Host → FPGA: 0x07 (STREAM_ENABLE command)
   FPGA → Host: 0x00 (ACK - ready) or 0xFF (NACK - not ready)
   ```

2. **Stream Instructions:**
   ```
   Host → FPGA: 4 bytes (instruction encoding)
   FPGA → Host: 0x00 (accepted), 0xFF (buffer full), 0x01 (buffer swapped)
   ```

3. **Execution:**
   - FPGA streams instructions from FIFO to controller
   - Controller decodes and executes on datapath
   - Double-buffering allows continuous streaming

4. **Completion:**
   - HALT instruction stops execution
   - Status register shows all units idle

### Reading the Status Byte

Status byte format (6 bits):
```
Bit 5: ub_done      - Unified buffer operation complete
Bit 4: ub_busy      - Unified buffer is busy
Bit 3: vpu_done     - VPU operation complete
Bit 2: vpu_busy     - VPU is busy
Bit 1: sys_done     - Systolic array operation complete
Bit 0: sys_busy     - Systolic array is busy
```

Example:
```python
status_byte = 0b000101
# sys_busy=1 (systolic array computing)
# sys_done=0 (not finished yet)
# vpu_busy=1 (VPU also busy)
```

### Basys 3 Visual Feedback

**7-Segment Display:**
```
 ---   ---   ---   ---
|   | |   | |   | |   |
 ---   ---   ---   ---
|   | |   | |   | |   |
 ---   ---   ---   ---
  3     2     1     0

Digit 3-2: Write buffer occupancy (0-32 in hex)
Digit 1-0: Read buffer occupancy (0-32 in hex)
```

**LEDs:**
- Green = active/busy/high
- Off = idle/done/low

During execution, you should see:
- LEDs blinking as instructions execute
- 7-seg display showing buffer levels changing
- LED[11] (exec_active) staying on until HALT

### Performance Metrics

Typical performance for 3x3 matrix multiply:
- **Instruction streaming:** ~9 instructions/second (115200 baud)
- **Matrix multiply latency:** 6-8 clock cycles
- **Total throughput:** Limited by UART bandwidth

UART streaming bandwidth:
- 115200 baud / 10 bits per byte = 11,520 bytes/sec
- 4 bytes per instruction = 2,880 instructions/sec theoretical max
- Actual: ~500-1000 instructions/sec (with flow control overhead)

---

## 10. Next Steps

### Optimizations
1. **Increase UART baud rate** to 921600 for 8x speedup
2. **Batch instructions** to reduce protocol overhead
3. **Use larger matrices** (buffer multiple 3x3 tiles)
4. **Profile with** cocotb testbenches for cycle-accurate simulation

### Advanced Testing
1. **MobileNet inference** - Full CNN with CONV2D + RELU6
2. **BERT attention** - Matrix multiply chains with batch norm
3. **Image processing** - Convolution kernels + pooling

### Integration
1. **Connect to camera** for real-time inference
2. **SPI/I2C** to external memory for larger models
3. **PCIe** for high-bandwidth host interface (requires FPGA with PCIe)

---

## Quick Reference Card

### Common Commands

```bash
# Find UART device
ls /dev/tty.usbserial* | tail -1

# Simple status check
echo -ne '\x06' > /dev/tty.usbserial-XXX

# Run full test suite
python3 test_all_instructions.py /dev/tty.usbserial-XXX

# Program FPGA
vivado -mode tcl -source program_fpga.tcl
```

### Python Quick Test

```python
from tpu_coprocessor_driver import TPU_Coprocessor

tpu = TPU_Coprocessor('/dev/tty.usbserial-XXX')
tpu.enable_stream_mode()
tpu.nop()  # Send NOP
tpu.halt() # Send HALT
tpu.wait_done()
tpu.close()
```

### UART Protocol Commands

| Command | Code | Args | Description |
|---------|------|------|-------------|
| WRITE_UB | 0x01 | addr(2), len(2), data(N) | Write to unified buffer |
| WRITE_WT | 0x02 | addr(2), len(2), data(N) | Write to weight memory |
| WRITE_INSTR | 0x03 | addr(2), instr(4) | Write single instruction |
| READ_UB | 0x04 | addr(2), len(2) | Read from unified buffer |
| EXECUTE | 0x05 | - | Start execution (legacy) |
| READ_STATUS | 0x06 | - | Read TPU status byte |
| STREAM_ENABLE | 0x07 | - | Enter streaming mode |

---

**Good luck testing your TPU coprocessor!** 🚀

If you encounter issues not covered here, check:
- `tpu_coprocessor_driver.py` - Driver source code with detailed comments
- `ISA_Reference.md` - Complete ISA specification
- `OPCODE_REFERENCE.md` - Instruction encoding details
