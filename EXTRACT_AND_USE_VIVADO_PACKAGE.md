# How to Extract and Use Vivado Package on Adroit

## Quick Steps

### Step 1: Extract the Package

On **Adroit terminal**:

```bash
# Navigate to your build directory
cd /home/as0714/tpu_build

# Extract the most recent package (vivado_package_v2.tar.gz)
tar -xzf vivado_package_v2.tar.gz

# This will create/extract to: vivado_package_v2/ directory
```

**Alternative: Extract to a specific directory name**

```bash
# Extract to a clean directory
mkdir -p vivado_package_latest
cd vivado_package_latest
tar -xzf ../vivado_package_v2.tar.gz
cd ..
```

### Step 2: Verify Contents

```bash
# Check what was extracted
cd vivado_package_v2  # or vivado_package_latest
ls -lh

# Expected files:
# - tpu.edif              (EDIF netlist from Yosys)
# - tpu_constraints.xdc   (FPGA constraints)
# - vivado_import_edif.tcl (Vivado build script)
# - README.txt            (Optional documentation)
```

### Step 3: Use with Vivado

#### Option A: Batch Mode (Command Line)

```bash
# Make sure you're in the extracted directory
cd /home/as0714/tpu_build/vivado_package_v2

# Load Vivado module
module load vivado/2023.1

# Run Vivado batch mode
vivado -mode batch -source vivado_import_edif.tcl

# This will:
# 1. Create project
# 2. Import EDIF
# 3. Read constraints
# 4. Run opt_design (should now pass!)
# 5. Run place_design
# 6. Run route_design
# 7. Generate bitstream
```

#### Option B: Visual Vivado (GUI)

```bash
# Make sure you're in the extracted directory
cd /home/as0714/tpu_build/vivado_package_v2

# Load Vivado module
module load vivado/2023.1

# Launch Vivado GUI
vivado &

# Then in Vivado GUI:
# 1. File → New Project
# 2. Select "RTL Project"
# 3. Add Files → Add all .sv files from rtl/ directory
# 4. Add Constraints → Add tpu_constraints.xdc
# 5. Set Top Module: tpu_top
# 6. Run Synthesis → Run Implementation → Generate Bitstream
```

**OR use the TCL script:**

```bash
# In Vivado GUI TCL console:
cd /home/as0714/tpu_build/vivado_package_v2
source vivado_import_edif.tcl
```

## What's Inside the Package?

The `.tar.gz` file contains:

```
vivado_package_v2/
├── tpu.edif                 # Synthesized netlist (from Yosys)
├── tpu_constraints.xdc      # FPGA pin and timing constraints
├── vivado_import_edif.tcl   # Automated build script
└── README.txt               # Optional documentation
```

## Troubleshooting

### If extraction fails:

```bash
# Check file integrity
file vivado_package_v2.tar.gz
# Should show: "gzip compressed data"

# Try verbose extraction
tar -xzvf vivado_package_v2.tar.gz
```

### If files are missing after extraction:

```bash
# List archive contents without extracting
tar -tzf vivado_package_v2.tar.gz

# Should show:
# vivado_package_v2/tpu.edif
# vivado_package_v2/tpu_constraints.xdc
# vivado_package_v2/vivado_import_edif.tcl
```

### If Vivado can't find files:

```bash
# Make sure you're in the right directory
pwd
# Should show: /home/as0714/tpu_build/vivado_package_v2

# Check files exist
ls -lh tpu.edif tpu_constraints.xdc
```

## Quick Reference Commands

```bash
# Extract
cd /home/as0714/tpu_build
tar -xzf vivado_package_v2.tar.gz

# Use with Vivado
cd vivado_package_v2
module load vivado/2023.1
vivado -mode batch -source vivado_import_edif.tcl

# Check results
ls -lh build/tpu_top.bit
# Should show: ~2-3 MB bitstream file
```

## Next Steps After Bitstream Generation

1. **Download bitstream to your Mac:**
   ```bash
   # From Mac terminal
   scp as0714@adroit.princeton.edu:/home/as0714/tpu_build/vivado_package_v2/build/tpu_top.bit \
       /Users/abiralshakya/Documents/tpu_to_fpga/build/
   ```

2. **Program FPGA:**
   ```bash
   # On Mac
   cd /Users/abiralshakya/Documents/tpu_to_fpga
   openocd -f basys3.cfg -c "program build/tpu_top.bit verify reset exit"
   ```

3. **Test UART:**
   ```bash
   python3 tpu_coprocessor_driver.py
   ```

---

**The most recent package (4:31:20 PM) should have the fixed `instruction_fifo.sv` that resolves the multiple driver error!**

