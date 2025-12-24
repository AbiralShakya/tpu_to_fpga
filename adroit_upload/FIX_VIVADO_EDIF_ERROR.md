# Fix: "No HDL sources found in project" Error

## Problem

When you create an **RTL Project** in Vivado and add an EDIF file, Vivado tries to **synthesize** it. But EDIF is already synthesized - you need to **import** it, not synthesize it.

**Error**: `[Common 17-69] Command failed: Unable to run synthesis. No HDL sources found in project`

## Solution: Use TCL Commands in Vivado

Instead of using the GUI project flow, use TCL commands to import EDIF directly.

### Method 1: TCL Console in Vivado (Easiest)

1. **Open Vivado** (if not already open)
2. **Open TCL Console** (bottom panel, or Window → Tcl Console)
3. **Run these commands**:

```tcl
# Navigate to your directory
cd /home/as0714/tpu_build/adroit_upload

# Read EDIF (this imports it, doesn't synthesize)
read_edif tpu_basys3.edif

# Set top module
set_property top tpu_top_wrapper [current_fileset]

# Read constraints
read_xdc basys3_nextpnr.xdc

# Link design (validates everything)
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper

# Now you can run implementation directly
# (skip synthesis - EDIF is already synthesized)
opt_design
place_design
route_design

# Generate bitstream
write_bitstream -force tpu_top_wrapper.bit
```

### Method 2: Use the TCL Script (Recommended)

**Close the current project** and run:

```bash
# In terminal on Adroit
cd /home/as0714/tpu_build/adroit_upload
module load vivado/2023.1
vivado -mode batch -source build_tpu_vivado.tcl
```

This script does everything automatically and avoids the GUI project issues.

### Method 3: Fix Current Project (GUI)

If you want to keep using the GUI:

1. **Close current project**: File → Close Project
2. **Open TCL Console**: Window → Tcl Console
3. **Run these commands**:

```tcl
cd /home/as0714/tpu_build/adroit_upload

# Create project but don't add sources yet
create_project tpu_basys3 . -part xc7a35tcpg236-1 -force

# Read EDIF directly (bypasses synthesis)
read_edif tpu_basys3.edif

# Set top module
set_property top tpu_top_wrapper [current_fileset]

# Add constraints
read_xdc basys3_nextpnr.xdc

# Link design
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper
```

4. **Now in Flow Navigator**:
   - **Skip "Run Synthesis"** (EDIF is already synthesized)
   - Go directly to **IMPLEMENTATION → Run Implementation**
   - Then **PROGRAM AND DEBUG → Generate Bitstream**

## Why This Happens

- **RTL Project** = Expects HDL sources (Verilog/VHDL) to synthesize
- **EDIF** = Already synthesized netlist (gate-level)
- **Solution** = Import EDIF directly, skip synthesis

## Quick Fix (Copy-Paste)

In Vivado TCL Console:

```tcl
cd /home/as0714/tpu_build/adroit_upload
read_edif tpu_basys3.edif
set_property top tpu_top_wrapper [current_fileset]
read_xdc basys3_nextpnr.xdc
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper
opt_design
place_design
route_design
write_bitstream -force tpu_top_wrapper.bit
```

This will generate `tpu_top_wrapper.bit` in the current directory.

