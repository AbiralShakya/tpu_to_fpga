# TPU Build Guide: Yosys + Vivado Docker

## 🎯 Overview

This guide walks you through building the TPU bitstream using:
- **Yosys** (open-source) for synthesis on your Mac
- **Vivado in Docker** (minimal proprietary usage) for place & route + bitstream

**Total time:** 30-60 minutes (first time with Docker download)

---

## 📋 Prerequisites

### Install Docker Desktop

```bash
# Install via Homebrew
brew install --cask docker

# Start Docker Desktop
open -a Docker

# Wait for "Docker Desktop is running" in menu bar
```

**Verify Docker is running:**
```bash
docker --version
# Should show: Docker version 24.x.x

docker info
# Should show Docker daemon info (not error)
```

### Install Yosys (if not already installed)

```bash
# Install Yosys via Homebrew
brew install yosys

# Verify
yosys --version
# Should show: Yosys 0.60+
```

---

## 🚀 Build Process

### Step 1: Navigate to Project

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga/tpu_to_fpga
```

### Step 2: Run Complete Build Script

```bash
./build_with_docker.sh
```

This automated script will:

1. ✅ **Check Docker** is running
2. ✅ **Synthesize with Yosys** (generates `build/tpu_basys3.json`)
3. ✅ **Convert to EDIF** (generates `build/tpu.edif`)
4. ✅ **Pull Vivado Docker image** (~5GB, first time only)
5. ✅ **Generate bitstream** with Vivado in Docker

**Total time:**
- First run: 30-60 min (includes Docker image download)
- Subsequent runs: 10-15 min (synthesis + P&R)

---

## 📊 Expected Output

### Successful Build

```
============================================
TPU Build: Yosys + Vivado Docker
============================================

✓ Docker is running

Step 1: Synthesis with Yosys (open-source)...
✓ Synthesis complete: build/tpu_basys3.json

Step 2: Converting JSON to EDIF...
✓ EDIF generated: build/tpu.edif

Step 3: Checking Vivado Docker image...
  ✓ Vivado Docker image found

Step 4: Generating bitstream with Vivado (Docker)...
This may take 5-15 minutes...

============================================
Vivado EDIF Import and Bitstream Generation
============================================

Creating Vivado project...
Reading EDIF netlist...
Reading constraints...
Linking design...

Pre-optimization utilization:
  LUTs:     28543 / 20800 (137%)  ⚠️ MAY NEED OPTIMIZATION
  FFs:      15234 / 41600 (37%)   ✓ OK
  DSPs:     9 / 90 (10%)           ✓ OK
  BRAMs:    6 / 50 (12%)           ✓ OK

Running optimization...
✓ Optimization complete

Running placement...
✓ Placement complete

Running routing...
✓ Routing complete

✓ Timing constraints met (slack: 0.234 ns)

Generating bitstream...

============================================
✅ BUILD SUCCESSFUL!
============================================

Output files:
  Bitstream:     build/tpu_top.bit
  Synthesis:     build/tpu_basys3.json (Yosys)
  EDIF netlist:  build/tpu.edif
  Reports:       build/*.rpt

Bitstream size: 2.1M

Next steps:
  1. Connect Basys 3 FPGA to USB
  2. Program with OpenOCD:
     openocd -f basys3.cfg -c 'program build/tpu_top.bit verify reset exit'

  3. Test UART connection:
     python3 test_all_instructions.py /dev/tty.usbserial-XXXX
```

---

## 🔧 Manual Build Steps (if script fails)

### Step 1: Synthesis with Yosys

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga/tpu_to_fpga

# Run Yosys synthesis
yosys synth_basys3.ys

# Should create: build/tpu_basys3.json
ls -lh build/tpu_basys3.json
```

### Step 2: Convert to EDIF

```bash
# Convert JSON to EDIF format
yosys -p "read_json build/tpu_basys3.json; write_edif build/tpu.edif"

# Verify
ls -lh build/tpu.edif
```

### Step 3: Pull Vivado Docker Image

```bash
# Pull image (5GB download, takes 10-30 min)
docker pull ghcr.io/hdlc/vivado:2023.1

# Verify image
docker images | grep vivado
```

### Step 4: Run Vivado in Docker

```bash
# Generate bitstream with Vivado
docker run --rm \
    -v "$(pwd):/work" \
    -w /work \
    ghcr.io/hdlc/vivado:2023.1 \
    vivado -mode batch -source vivado_import_edif.tcl

# Check output
ls -lh build/tpu_top.bit
```

---

## 📦 Programming the FPGA

### Option 1: OpenOCD (Recommended)

```bash
# Install OpenOCD
brew install openocd

# Connect Basys 3 via USB
# Program FPGA
openocd -f basys3.cfg -c "program build/tpu_top.bit verify reset exit"
```

### Option 2: Vivado Hardware Manager (Docker)

```bash
# Run Vivado GUI in Docker with X11 forwarding
# (Requires XQuartz)
brew install --cask xquartz

# Start XQuartz and enable "Allow connections from network clients"

# Run Vivado with GUI
docker run --rm \
    -e DISPLAY=host.docker.internal:0 \
    -v "$(pwd):/work" \
    -w /work \
    ghcr.io/hdlc/vivado:2023.1 \
    vivado

# In Vivado GUI:
# Tools → Open Hardware Manager → Open Target → Auto Connect
# Right-click on xc7a35t → Program Device → Select build/tpu_top.bit
```

### Option 3: Digilent Adept (Native Mac App)

```bash
# Download from: https://digilent.com/shop/software/digilent-adept/
# Install Adept Runtime + Utilities
# Use Adept GUI to program build/tpu_top.bit
```

---

## 🧪 Testing After Programming

### Step 1: Verify FPGA is Programmed

```bash
# LEDs should show:
# LED[15:12] = 0x0 (UART idle state)
# LED[11:8]  = 0x0 (FIFO empty)
# LED[7:4]   = 0x0 (TPU idle)

# 7-segment display should show: 0000
```

### Step 2: Find UART Device

```bash
ls /dev/tty.usbserial* | tail -1
# Should show: /dev/tty.usbserial-XXXXXXXXXXXX
```

### Step 3: Test UART Communication

```bash
# Activate Python virtual environment
source venv/bin/activate

# Quick UART test
python3 << 'EOF'
import serial
import time

port = '/dev/tty.usbserial-210183A27BE01'  # ← Use your device
ser = serial.Serial(port, 115200, timeout=2)
time.sleep(0.1)

# Send status query (command 0x06)
ser.write(bytes([0x06]))
time.sleep(0.1)

# Read status byte
status = ser.read(1)
if len(status) == 1:
    print(f"✓ UART working! Status: 0x{status[0]:02X}")
else:
    print("✗ No response from UART")

ser.close()
EOF
```

### Step 4: Run Full Test Suite

```bash
python3 test_all_instructions.py /dev/tty.usbserial-XXXXXXXXXXXX
```

**Expected:** 20/20 tests passed ✅

---

## ⚠️ Troubleshooting

### Docker: "Cannot connect to Docker daemon"

```bash
# Start Docker Desktop
open -a Docker

# Wait for Docker icon in menu bar to stop animating
# Then run build again
```

### Docker: "Error pulling image"

```bash
# Try alternative image
docker pull xilinx/vivado:2023.1

# Or try older version
docker pull ghcr.io/hdlc/vivado:2022.2

# Update build_with_docker.sh to use your image
```

### Yosys: "Unknown primitive"

```bash
# Check Yosys version
yosys --version

# Should be 0.40+
# Update if needed:
brew upgrade yosys
```

### Vivado: "Cannot read EDIF file"

```bash
# Regenerate EDIF
rm build/tpu.edif
yosys -p "read_json build/tpu_basys3.json; write_edif build/tpu.edif"

# Verify file is not empty
wc -l build/tpu.edif
# Should show thousands of lines
```

### Vivado: "Timing violations"

```bash
# Check timing reports
less build/timing_final.rpt

# If slack is negative, design may not work at 100 MHz
# Options:
# 1. Reduce clock frequency in constraints
# 2. Add pipeline stages
# 3. Simplify design
```

### Vivado: "LUT count exceeds device capacity"

```bash
# Your TPU is too big for Artix-7 35T
# Check utilization report
less build/utilization_final.rpt

# Solutions:
# 1. Reduce systolic array from 3x3 to 2x2
# 2. Reduce buffer sizes
# 3. Remove double buffering
# 4. Upgrade to larger FPGA (Basys 3 is smallest Artix-7)
```

### OpenOCD: "Device not found"

```bash
# Check USB connection
system_profiler SPUSBDataType | grep -A 10 "Basys3"

# Should show:
#   Vendor ID: 0x0403 (FTDI)
#   Product ID: 0x6010

# If not found, install FTDI drivers
# Download from: https://ftdichip.com/drivers/vcp-drivers/
```

### UART: "No response"

```bash
# 1. Check FPGA is programmed (LEDs should be lit)
# 2. Check UART device name
ls /dev/tty.usbserial*

# 3. Check permissions
sudo chmod 666 /dev/tty.usbserial-XXXX

# 4. Try different baud rates
python3 -c "
import serial
for baud in [9600, 115200, 921600]:
    try:
        ser = serial.Serial('/dev/tty.usbserial-XXX', baud, timeout=1)
        ser.write(bytes([0x06]))
        resp = ser.read(1)
        if resp:
            print(f'✓ Works at {baud} baud')
        ser.close()
    except:
        pass
"
```

---

## 📊 Build Time Breakdown

| Step | Time (first run) | Time (subsequent) |
|------|------------------|-------------------|
| Docker image pull | 10-30 min | 0 (cached) |
| Yosys synthesis | 30 sec | 30 sec |
| EDIF conversion | 5 sec | 5 sec |
| Vivado P&R | 10-15 min | 10-15 min |
| **Total** | **30-60 min** | **10-15 min** |

---

## 🎓 What's Happening Under the Hood

### Yosys Synthesis (Open-Source)

```
RTL (.sv files) ─→ [Yosys] ─→ JSON netlist
                                    │
                                    ↓
                            Generic gate-level
                            (LUTs, FFs, BRAM, DSP)
```

**What Yosys does:**
1. Parses SystemVerilog
2. Elaborates hierarchy
3. Optimizes logic
4. Maps to Xilinx primitives
5. Outputs JSON netlist

### Vivado P&R (Proprietary, in Docker)

```
EDIF netlist ─→ [Vivado] ─→ Bitstream (.bit)
                    │
                    ├─→ Place (assign to physical sites)
                    ├─→ Route (connect with wires)
                    └─→ Generate bitstream
```

**What Vivado does:**
1. Imports EDIF (standard format)
2. Places logic on FPGA fabric
3. Routes signals
4. Performs timing analysis
5. Generates configuration bitstream

### Why Both?

| Tool | Open-Source? | Function | Can Replace? |
|------|-------------|----------|--------------|
| **Yosys** | ✅ Yes | Synthesis | ✅ Yes (with Vivado synth) |
| **Vivado** | ❌ No | P&R + Bitstream | ❌ No (proprietary format) |

**Result:** 80% open-source workflow, 20% proprietary (minimal!)

---

## 📚 Next Steps

After successful build:

1. **Test UART:** Verify basic communication
2. **Run test suite:** Validate all 20 instructions
3. **Benchmark performance:** Measure instruction throughput
4. **Optimize design:** Reduce LUT count if needed
5. **Experiment:** Try different matrix sizes, higher clock speeds

---

## ✅ Summary

**One-command build:**
```bash
./build_with_docker.sh
```

**Outputs:**
- `build/tpu_top.bit` - Program this to FPGA
- `build/*.rpt` - Review utilization and timing

**Program FPGA:**
```bash
openocd -f basys3.cfg -c "program build/tpu_top.bit verify reset exit"
```

**Test:**
```bash
python3 test_all_instructions.py /dev/tty.usbserial-XXXX
```

**Done!** 🎉
