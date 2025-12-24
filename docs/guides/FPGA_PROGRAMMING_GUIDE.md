# FPGA Programming and UART Communication Guide

## Understanding the Two Phases

There are **TWO SEPARATE PROCESSES**:

1. **FPGA Programming (One-Time Setup)** - Load your design onto the FPGA
2. **Runtime Communication (Continuous)** - Send instructions/data via UART while FPGA runs

---

## Phase 1: FPGA Programming (Bitstream Loading)

### How Basys 3 FPGA Gets Programmed

**Basys 3 uses JTAG via USB** - NOT UART for programming!

```
Your Computer → USB Cable → Basys 3 USB-UART Bridge → JTAG → FPGA Configuration Memory
```

### Step-by-Step Programming Process

#### Option A: Using Vivado (GUI)
```bash
1. Open Vivado
2. Open your project: vivado tpu_basys3.xpr
3. Run Synthesis → Run Implementation → Generate Bitstream
4. Open Hardware Manager → Connect to FPGA
5. Right-click FPGA → Program Device → Select .bit file
6. Click "Program"
```

#### Option B: Using openFPGALoader (Command Line)
```bash
# After building bitstream
openFPGALoader -b basys3 build/tpu_basys3.bit
```

#### Option C: Using Vivado TCL (Script)
```tcl
open_hw_manager
connect_hw_server
open_hw_target
current_hw_device [get_hw_devices xc7a35t_0]
set_property PROGRAM.FILE {build/tpu_basys3.bit} [current_hw_device]
program_hw_devices [current_hw_device]
```

### What Happens During Programming

1. **Bitstream (.bit file)** contains:
   - Complete FPGA configuration
   - Logic routing
   - Memory initialization
   - I/O pin assignments

2. **FPGA loads bitstream** into configuration memory (SRAM-based)
   - Takes ~1-2 seconds
   - FPGA starts running your design immediately
   - Configuration is **volatile** - lost on power cycle

3. **After programming:**
   - Your TPU design is running on FPGA
   - UART pins are now connected to your `uart_rx`/`uart_tx` modules
   - Ready for runtime communication!

---

## Phase 2: Runtime UART Communication

### After FPGA is Programmed

Once the FPGA is running your design, **UART is used for runtime communication**:

```
Host PC → USB-UART → FPGA UART Module → Your TPU Design
```

### How UART Works on Basys 3

**Physical Connection:**
- Basys 3 has **USB-UART bridge** (FTDI chip)
- USB cable connects to your computer
- Shows up as `/dev/ttyUSB0` (Linux) or `COM3` (Windows)

**UART Pins (from constraints):**
```systemverilog
uart_rx → Pin A18 (receives data from PC)
uart_tx → Pin B18 (sends data to PC)
```

### Runtime Communication Flow

```
┌─────────────┐         ┌──────────────┐         ┌─────────────┐
│   Host PC   │────────▶│  USB-UART    │────────▶│   FPGA      │
│  (Python)   │  USB    │   Bridge     │  UART   │  (Your TPU) │
│             │         │  (FTDI Chip) │         │             │
└─────────────┘         └──────────────┘         └─────────────┘
     │                        │                        │
     │                        │                        │
  Python              Hardware Bridge          SystemVerilog
  Driver              (Automatic)              UART Module
```

### Example: Sending Instructions via UART

**1. FPGA is already programmed** (from Phase 1)

**2. Host sends instructions via UART:**
```python
# tpu_driver.py
import serial

# Connect to FPGA's UART (already programmed)
ser = serial.Serial('/dev/ttyUSB0', 115200)

# Send instruction command
ser.write(bytes([0x03]))  # CMD_WRITE_INSTR
ser.write(bytes([0x00]))  # Address high
ser.write(bytes([0x00]))  # Address low

# Send 32-bit instruction (4 bytes)
instruction = 0x10000000  # MATMUL opcode
ser.write(struct.pack('>I', instruction))

# FPGA receives via uart_rx → uart_dma_basys3 → instruction buffer
```

**3. FPGA processes instruction:**
```systemverilog
// In your FPGA design (already running)
uart_rx uart_rx_inst (
    .clk(clk),
    .rx(uart_rx),  // Pin A18
    .rx_data(rx_data),
    .rx_valid(rx_valid)
);

uart_dma_basys3 uart_dma (
    .clk(clk),
    .uart_rx(rx_data),
    .instr_wr_en(instr_wr_en),
    .instr_wr_data(instr_wr_data)
);
```

---

## Complete Workflow: From Code to Running TPU

### Step 1: Write Your Design
```bash
# Your SystemVerilog files
rtl/tpu_top.sv
rtl/tpu_controller.sv
rtl/uart_dma_basys3.sv
# etc...
```

### Step 2: Build Bitstream
```bash
# Using Vivado or open-source tools
vivado -mode batch -source build.tcl
# OR
yosys synth_basys3.ys
nextpnr-xilinx ...
# Result: build/tpu_basys3.bit
```

### Step 3: Program FPGA (ONE TIME)
```bash
# Connect Basys 3 via USB
# Program bitstream
openFPGALoader -b basys3 build/tpu_basys3.bit

# FPGA now running your TPU design!
```

### Step 4: Runtime Communication (MANY TIMES)
```bash
# FPGA is already running - now communicate via UART
python3 tpu_driver.py

# Or use your test script
python3 test_isa_fpga.py
```

---

## Key Differences

| Aspect | FPGA Programming | Runtime UART |
|--------|------------------|--------------|
| **Purpose** | Load design onto FPGA | Send data/instructions |
| **Protocol** | JTAG (via USB) | UART (via USB-UART bridge) |
| **Frequency** | Once per design change | Continuously while running |
| **Tool** | Vivado/openFPGALoader | Python serial library |
| **Data** | Bitstream (.bit file) | Instructions/data bytes |
| **Speed** | ~1-2 seconds | 115200 baud (configurable) |
| **Persistence** | Lost on power cycle | Immediate execution |

---

## Why You Can't Program via UART

**Basys 3 Artix-7 FPGA:**
- Does NOT support UART-based configuration
- Only supports JTAG configuration
- UART is for **user application communication**, not FPGA programming

**Some FPGAs DO support UART configuration:**
- Lattice iCE40 (via SPI)
- Some Xilinx FPGAs (via BPI/SPI flash)
- But NOT Basys 3's Artix-7

---

## Practical Example: Complete Session

```bash
# 1. Build your design
cd /Users/abiralshakya/Documents/tpu_to_fpga
vivado -mode batch -source build.tcl

# 2. Program FPGA (one-time, takes 2 seconds)
openFPGALoader -b basys3 build/tpu_basys3.bit
# Output: "Programming done!"

# 3. FPGA is now running - verify connection
python3 -c "
import serial
ser = serial.Serial('/dev/ttyUSB0', 115200)
print('Connected to FPGA!')
ser.close()
"

# 4. Send instructions via UART (runtime)
python3 tpu_driver.py
# Or
python3 test_isa_fpga.py

# 5. FPGA executes instructions and responds via UART
```

---

## Troubleshooting

### "Can't find /dev/ttyUSB0"
- Check USB cable connection
- Check if Basys 3 is powered on
- Install FTDI drivers (if needed)
- Try `ls /dev/tty*` to find correct port

### "FPGA not responding to UART"
- Verify FPGA is programmed (LEDs should show activity)
- Check baud rate matches (115200)
- Verify UART pins in constraints file
- Check if UART modules are in your design

### "Want to reprogram FPGA"
- Just run `openFPGALoader` again
- No need to power cycle
- New bitstream overwrites old one

---

## Summary

1. **FPGA Programming** = Load your design (JTAG via USB) - **ONE TIME**
2. **UART Communication** = Send instructions/data (UART via USB) - **CONTINUOUS**

**You program once, then communicate many times!**

The UART is part of **your design** that runs **on the FPGA** - it's not used to program the FPGA itself.

