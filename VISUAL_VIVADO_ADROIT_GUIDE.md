# Visual Vivado Setup on Princeton Adroit - Complete Guide

## Overview

This guide shows you how to set up your TPU project in **Visual Vivado (GUI)** on Princeton's Adroit cluster, upload your files, and generate the bitstream.

**Your project location:** `/scratch/network/as0714/tpu_to_fpga`

---

## Step 1: Prepare Files for Upload

### Files You Need to Upload

From your local Mac, you need to upload these files to Adroit:

**Required Files:**
```
rtl/                    # All SystemVerilog source files
  ├── tpu_top.sv        # Top-level module
  ├── tpu_controller.sv
  ├── tpu_datapath.sv
  ├── unified_buffer.sv
  ├── systolic_controller.sv
  ├── accumulator.sv
  ├── weight_fifo.sv
  ├── uart_rx.sv
  ├── uart_tx.sv
  ├── uart_dma_basys3.sv
  └── ... (all other .sv files)

tpu_constraints.xdc     # FPGA constraints file
xilinx_primitives.v     # Xilinx primitives (if exists)
```

**Optional but Recommended:**
```
create_vivado_project.tcl  # Project creation script
README.md                   # Documentation
```

### Create Upload Package

On your **Mac terminal**:

```bash
# Navigate to your project
cd /Users/abiralshakya/Documents/tpu_to_fpga

# Create a clean package with only necessary files
tar -czf tpu_vivado_project.tar.gz \
    rtl/*.sv \
    rtl/*.v \
    tpu_constraints.xdc \
    xilinx_primitives.v \
    create_vivado_project.tcl \
    2>/dev/null

# Verify package
ls -lh tpu_vivado_project.tar.gz
# Should show: ~500KB - 2MB depending on files
```

---

## Step 2: Upload Files to Adroit

### Upload via SCP

From your **Mac terminal**:

```bash
# Upload to your scratch directory on Adroit
scp tpu_vivado_project.tar.gz as0714@adroit.princeton.edu:/scratch/network/as0714/tpu_to_fpga/

# Expected output:
# tpu_vivado_project.tar.gz    100%  1.2MB   2.5MB/s   00:00
```

**Alternative: Upload entire directory (if you want everything):**

```bash
# Upload entire project directory
rsync -avz --exclude='*.vvp' --exclude='*.vcd' --exclude='__pycache__' \
    --exclude='venv' --exclude='tpu_env' --exclude='obj_dir' \
    /Users/abiralshakya/Documents/tpu_to_fpga/ \
    as0714@adroit.princeton.edu:/scratch/network/as0714/tpu_to_fpga/
```

---

## Step 3: SSH to Adroit with X11 Forwarding

### Enable X11 Forwarding for GUI

From your **Mac terminal**:

```bash
# SSH with X11 forwarding (required for Vivado GUI)
ssh -X as0714@adroit.princeton.edu

# If -X doesn't work, try trusted X11 forwarding
ssh -Y as0714@adroit.princeton.edu
```

**Note:** You need X11 server on Mac:
- **macOS:** Install [XQuartz](https://www.xquartz.org/) (free)
- After installing XQuartz, restart terminal and try again

### Verify X11 is Working

On **Adroit terminal**:

```bash
# Test X11 forwarding
echo $DISPLAY
# Should show: localhost:10.0 or similar

# Test with simple GUI app
xeyes  # If this opens a window, X11 is working!
# Press Ctrl+C to close
```

---

## Step 4: Extract Files on Adroit

On **Adroit terminal**:

```bash
# Navigate to your project directory
cd /scratch/network/as0714/tpu_to_fpga

# Extract package (if you uploaded tar.gz)
tar -xzf tpu_vivado_project.tar.gz

# Verify files are present
ls -lh rtl/*.sv | head -10
ls -lh tpu_constraints.xdc

# Expected output:
# -rw-r--r-- 1 as0714 users 15K Dec 22 19:30 rtl/tpu_top.sv
# -rw-r--r-- 1 as0714 users 3.1K Dec 22 19:30 tpu_constraints.xdc
```

---

## Step 5: Load Vivado Module

On **Adroit terminal**:

```bash
# Search for available Vivado versions
module avail vivado

# Load Vivado (use latest available, typically 2023.1 or 2024.1)
module load vivado/2023.1

# Verify Vivado is loaded
which vivado
# Should show: /usr/licensed/vivado/2023.1/bin/vivado

vivado -version
# Should show: Vivado v2023.1 (64-bit)
```

**If module command not found:**
```bash
# Add to your ~/.bashrc
echo 'source /etc/profile.d/modules.sh' >> ~/.bashrc
source ~/.bashrc
```

---

## Step 6: Launch Visual Vivado

On **Adroit terminal**:

```bash
# Make sure you're in project directory
cd /scratch/network/as0714/tpu_to_fpga

# Launch Vivado GUI
vivado &

# Vivado GUI should open in a new window
# This may take 30-60 seconds to start
```

**If Vivado doesn't open:**
- Check X11 forwarding: `echo $DISPLAY`
- Try: `export DISPLAY=localhost:10.0`
- Make sure XQuartz is running on Mac

---

## Step 7: Create Vivado Project (Visual Method)

### Method A: Using Project Wizard (Recommended for First Time)

1. **In Vivado GUI:**
   - Click **"Create Project"** (or File → New Project)
   - Click **Next**

2. **Project Name:**
   - Project name: `tpu_basys3`
   - Project location: `/scratch/network/as0714/tpu_to_fpga`
   - Check **"Create project subdirectory"**
   - Click **Next**

3. **Project Type:**
   - Select **"RTL Project"**
   - Check **"Do not specify sources at this time"** (we'll add manually)
   - Click **Next**

4. **Default Part:**
   - Click **"Boards"** tab
   - Search for: `Basys 3`
   - Select **"Basys 3"** (Artix-7)
   - Click **Next**

5. **Summary:**
   - Review settings
   - Click **Finish**

### Method B: Using TCL Script (Faster)

If you uploaded `create_vivado_project.tcl`:

1. **In Vivado GUI:**
   - Click **Tools → Run Tcl Script...**
   - Navigate to: `/scratch/network/as0714/tpu_to_fpga/create_vivado_project.tcl`
   - Click **OK**

2. **Project will be created automatically!**

---

## Step 8: Add Source Files to Project

### Add SystemVerilog Files

1. **In Vivado GUI:**
   - In **Sources** panel (left side), right-click **"Design Sources"**
   - Select **"Add Sources..."**

2. **Add Sources Dialog:**
   - Select **"Add or create design sources"**
   - Click **Next**

3. **Add Files:**
   - Click **"Add Files..."** button
   - Navigate to: `/scratch/network/as0714/tpu_to_fpga/rtl/`
   - Select **ALL** `.sv` files:
     - `tpu_top.sv` (IMPORTANT: This is your top module)
     - `tpu_controller.sv`
     - `tpu_datapath.sv`
     - `unified_buffer.sv`
     - `systolic_controller.sv`
     - `accumulator.sv`
     - `accumulator_align.sv`
     - `accumulator_mem.sv`
     - `weight_fifo.sv`
     - `dual_weight_fifo.sv`
     - `uart_rx.sv`
     - `uart_tx.sv`
     - `uart_dma_basys3.sv`
     - `pe.sv`
     - `pe_dsp.sv`
     - `mmu.sv`
     - `vpu.sv`
     - `activation_func.sv`
     - `activation_pipeline.sv`
     - `normalizer.sv`
     - `loss_block.sv`
     - `clock_manager.sv`
     - `basys3_test_interface.sv`
     - `mlp_top.sv`
     - `simple_test_top.sv`
     - Any other `.sv` files in `rtl/`

4. **Copy Sources:**
   - Check **"Copy sources into project"** (recommended)
   - Click **Finish**

5. **Set Top Module:**
   - In **Sources** panel, find `tpu_top.sv`
   - Right-click → **"Set as Top"**

### Add Verilog Primitives (if exists)

If you have `xilinx_primitives.v`:

1. Right-click **"Design Sources"**
2. **Add Sources...** → **Add Files...**
3. Select `xilinx_primitives.v`
4. Click **Finish**

---

## Step 9: Add Constraints File

1. **In Vivado GUI:**
   - In **Sources** panel, right-click **"Constraints"**
   - Select **"Add Sources..."**

2. **Add Constraints Dialog:**
   - Select **"Add or create constraints"**
   - Click **Next**

3. **Add Constraints File:**
   - Click **"Add Files..."**
   - Navigate to: `/scratch/network/as0714/tpu_to_fpga/`
   - Select: `tpu_constraints.xdc`
   - Click **Finish**

**Verify constraints are loaded:**
- In **Sources** panel, expand **"Constraints"**
- You should see `tpu_constraints.xdc`

---

## Step 10: Configure Project Settings

### Set File Types

1. **In Vivado GUI:**
   - Click **Settings** (gear icon) or **Project Settings**

2. **General Settings:**
   - **Top module name:** `tpu_top`
   - **Target language:** `SystemVerilog`

3. **Synthesis Settings:**
   - Click **"Synthesis"** in left panel
   - **Strategy:** `Flow_PerfOptimized_high` (or `Vivado Synthesis Defaults`)

4. **Implementation Settings:**
   - Click **"Implementation"** in left panel
   - **Strategy:** `Performance_ExtraTimingOpt` (or `Vivado Implementation Defaults`)

5. Click **OK**

---

## Step 11: Run Synthesis

1. **In Vivado GUI:**
   - In left panel, click **"Run Synthesis"** (or **Flow Navigator → Synthesis → Run Synthesis**)
   - Click **OK** in dialog

2. **Synthesis Progress:**
   - Watch progress in **Tcl Console** (bottom panel)
   - Takes **5-10 minutes**
   - Status shown in **Design Runs** tab

3. **Synthesis Complete:**
   - Dialog will appear: **"Synthesis completed successfully"**
   - Select **"Run Implementation"** → Click **OK**

**If synthesis fails:**
- Check **Messages** panel for errors
- Common issues:
  - Missing files → Add missing `.sv` files
  - Syntax errors → Fix in source files
  - Missing primitives → Add `xilinx_primitives.v`

---

## Step 12: Run Implementation

1. **Implementation Dialog:**
   - Should open automatically after synthesis
   - Click **OK**

2. **Implementation Progress:**
   - **Placement:** 5-8 minutes
   - **Routing:** 5-10 minutes
   - Total: **10-20 minutes**

3. **Implementation Complete:**
   - Dialog: **"Implementation completed successfully"**
   - Select **"Generate Bitstream"** → Click **OK**

**Check Timing:**
- After implementation, check **Timing Summary**:
  - **WNS (Worst Negative Slack):** Should be **≥ 0** (positive)
  - If negative, design may not work at 100 MHz

---

## Step 13: Generate Bitstream

1. **Generate Bitstream Dialog:**
   - Should open automatically after implementation
   - Click **OK**

2. **Bitstream Generation:**
   - Takes **1-2 minutes**
   - Creates `.bit` file

3. **Bitstream Complete:**
   - Dialog: **"Bitstream Generation completed successfully"**
   - Click **OK**

**Find Bitstream:**
- Location: `/scratch/network/as0714/tpu_to_fpga/tpu_basys3/tpu_basys3.runs/impl_1/tpu_top.bit`
- File size: **~2-3 MB**

---

## Step 14: Verify Bitstream

On **Adroit terminal**:

```bash
# Check bitstream exists
ls -lh /scratch/network/as0714/tpu_to_fpga/tpu_basys3/tpu_basys3.runs/impl_1/tpu_top.bit

# Expected output:
# -rw-r--r-- 1 as0714 users 2.1M Dec 22 20:15 tpu_top.bit
```

**Success indicators:**
- ✅ File exists
- ✅ File size ~2-3 MB
- ✅ No errors in Vivado messages

---

## Step 15: Download Bitstream to Mac

From your **Mac terminal** (new terminal, keep Adroit session open):

```bash
# Download bitstream
scp as0714@adroit.princeton.edu:/scratch/network/as0714/tpu_to_fpga/tpu_basys3/tpu_basys3.runs/impl_1/tpu_top.bit \
    /Users/abiralshakya/Documents/tpu_to_fpga/build/

# Verify download
ls -lh /Users/abiralshakya/Documents/tpu_to_fpga/build/tpu_top.bit
```

---

## Step 16: Download Reports (Optional)

```bash
# Download utilization report
scp as0714@adroit.princeton.edu:/scratch/network/as0714/tpu_to_fpga/tpu_basys3/tpu_basys3.runs/impl_1/tpu_top_utilization_placed.rpt \
    /Users/abiralshakya/Documents/tpu_to_fpga/build/

# Download timing report
scp as0714@adroit.princeton.edu:/scratch/network/as0714/tpu_to_fpga/tpu_basys3/tpu_basys3.runs/impl_1/tpu_top_timing_summary_routed.rpt \
    /Users/abiralshakya/Documents/tpu_to_fpga/build/
```

---

## Visual Vivado Workflow Summary

```
1. Upload files to Adroit
   ↓
2. SSH with X11 forwarding
   ↓
3. Load Vivado module
   ↓
4. Launch Vivado GUI
   ↓
5. Create project (wizard or TCL)
   ↓
6. Add all .sv files from rtl/
   ↓
7. Add tpu_constraints.xdc
   ↓
8. Set tpu_top.sv as top module
   ↓
9. Run Synthesis (5-10 min)
   ↓
10. Run Implementation (10-20 min)
   ↓
11. Generate Bitstream (1-2 min)
   ↓
12. Download .bit file to Mac
```

---

## Troubleshooting

### Problem: Vivado GUI doesn't open

**Solutions:**
```bash
# 1. Check X11 forwarding
echo $DISPLAY
# Should show: localhost:10.0

# 2. Install XQuartz on Mac
# Download: https://www.xquartz.org/

# 3. Restart terminal after XQuartz install

# 4. Try trusted forwarding
ssh -Y as0714@adroit.princeton.edu
```

### Problem: "Cannot find module vivado"

**Solution:**
```bash
# Load modules system
source /etc/profile.d/modules.sh

# Check available versions
module avail vivado

# Load available version
module load vivado/2023.1  # or whatever is available
```

### Problem: Synthesis fails with "Cannot find module"

**Solution:**
- Check **Messages** panel in Vivado
- Find missing module name
- Add corresponding `.sv` file to project
- Common missing files: `uart_rx.sv`, `uart_tx.sv`, `clock_manager.sv`

### Problem: Implementation fails with "Resource overflow"

**Error:** `ERROR: [Place 30-58] Design has more LUTs than available`

**Solution:**
- Your design is too large for Basys 3
- Check utilization report
- Reduce design complexity or use larger FPGA

### Problem: Timing violations

**Error:** `CRITICAL WARNING: Timing constraints are not met`

**Solution:**
1. Check timing report in Vivado
2. Look for **WNS (Worst Negative Slack)**
3. If negative, reduce clock frequency in `tpu_constraints.xdc`:
   ```tcl
   # Change from 100 MHz to 50 MHz
   create_clock -period 20.0 [get_ports clk]
   ```
4. Re-run implementation

---

## Quick Reference

### Files to Upload
```
rtl/*.sv              # All SystemVerilog files
tpu_constraints.xdc  # Constraints file
xilinx_primitives.v  # Primitives (if exists)
```

### Vivado Project Location
```
/scratch/network/as0714/tpu_to_fpga/tpu_basys3/
```

### Bitstream Location
```
/scratch/network/as0714/tpu_to_fpga/tpu_basys3/tpu_basys3.runs/impl_1/tpu_top.bit
```

### Key Vivado Commands (TCL Console)
```tcl
# Open project
open_project /scratch/network/as0714/tpu_to_fpga/tpu_basys3/tpu_basys3.xpr

# Run synthesis
launch_runs synth_1

# Run implementation
launch_runs impl_1

# Generate bitstream
launch_runs impl_1 -to_step write_bitstream
```

---

## Next Steps

After downloading bitstream:

1. **Program FPGA:**
   ```bash
   openocd -f basys3.cfg -c "program build/tpu_top.bit verify reset exit"
   ```

2. **Test UART:**
   ```bash
   python3 tpu_coprocessor_driver.py
   ```

3. **Run full test:**
   ```bash
   python3 test_isa_fpga.py
   ```

---

## Tips

1. **Keep Vivado open** - Don't close between runs
2. **Use Design Runs tab** - Monitor progress
3. **Check Messages panel** - Watch for warnings/errors
4. **Save project frequently** - File → Save Project
5. **Use TCL console** - Faster than clicking for repeated operations

---

**Ready to start? Begin with Step 1: Prepare Files for Upload!** 🚀

