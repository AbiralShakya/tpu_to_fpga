# Using Vivado GUI on Adroit Cluster

## Overview

This guide shows how to:
1. Upload files to Adroit
2. Access Vivado GUI via X11 forwarding
3. Import EDIF and generate bitstream
4. Download bitstream back to your Mac

## Step 1: Prepare Files for Upload

### Files You Need (Minimum Package)

Create a package with just the essential files:

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga

# Create a clean directory for upload
mkdir -p adroit_upload
cd adroit_upload

# Copy essential files
cp ../build/tpu_basys3.edif .
cp ../constraints/basys3_nextpnr.xdc .
cp ../vivado/build_tpu_vivado.tcl .

# Create a README
cat > README.txt << 'EOF'
TPU Bitstream Build Package for Adroit
=======================================

Files:
- tpu_basys3.edif: Synthesized netlist from Yosys
- basys3_nextpnr.xdc: FPGA constraints (pin locations)
- build_tpu_vivado.tcl: Automated build script (optional)

To build:
1. Load Vivado: module load vivado/2023.1
2. Open Vivado GUI: vivado &
3. Follow GUI steps in ADROIT_VIVADO_GUI_GUIDE.md
   OR run: vivado -mode batch -source build_tpu_vivado.tcl
EOF

# Create tarball for easy upload
cd ..
tar -czf tpu_vivado_package.tar.gz adroit_upload/
ls -lh tpu_vivado_package.tar.gz
```

**Package size**: ~1.4 MB (very small!)

## Step 2: Upload to Adroit

### Option A: Using SCP (Recommended)

```bash
# From your Mac terminal
cd /Users/abiralshakya/Documents/tpu_to_fpga

# Upload the package
scp tpu_vivado_package.tar.gz YOUR_NETID@adroit.princeton.edu:~/tpu_build/

# Or upload to scratch (more space)
scp tpu_vivado_package.tar.gz YOUR_NETID@adroit.princeton.edu:/scratch/network/YOUR_NETID/tpu_build/
```

**Replace `YOUR_NETID` with your Princeton NetID!**

### Option B: Using SFTP

```bash
# Connect to Adroit
sftp YOUR_NETID@adroit.princeton.edu

# Navigate to your directory
cd ~/tpu_build
# or
cd /scratch/network/YOUR_NETID/tpu_build

# Upload
put tpu_vivado_package.tar.gz

# Exit
exit
```

## Step 3: Extract on Adroit

```bash
# SSH into Adroit
ssh YOUR_NETID@adroit.princeton.edu

# Navigate to your build directory
cd ~/tpu_build
# or
cd /scratch/network/YOUR_NETID/tpu_build

# Extract the package
tar -xzf tpu_vivado_package.tar.gz
cd adroit_upload

# Verify files
ls -lh
# Should see:
# - tpu_basys3.edif (1.4 MB)
# - basys3_nextpnr.xdc (6 KB)
# - build_tpu_vivado.tcl (2 KB)
# - README.txt
```

## Step 4: Access Vivado GUI

### Setup X11 Forwarding

**On your Mac**, you need XQuartz for X11 forwarding:

```bash
# Install XQuartz (if not already installed)
brew install --cask xquartz

# Start XQuartz
open -a XQuartz

# In XQuartz preferences, enable:
# - "Allow connections from network clients"
# - Restart XQuartz after enabling
```

**SSH with X11 forwarding**:

```bash
# SSH with X11 forwarding
ssh -X YOUR_NETID@adroit.princeton.edu

# Test X11 (should open a window)
xeyes
# or
xclock
```

### Load Vivado and Launch GUI

```bash
# On Adroit, load Vivado module
module load vivado/2023.1

# Verify Vivado is loaded
which vivado
vivado -version

# Launch Vivado GUI (will open in XQuartz window)
vivado &

# Wait 30-60 seconds for GUI to open
```

**Note**: If GUI doesn't open, try:
```bash
# More permissive X11 forwarding
ssh -Y YOUR_NETID@adroit.princeton.edu
```

## Step 5: Build Bitstream in Vivado GUI

### Method 1: Manual GUI Steps (Recommended for Learning)

#### Step 5.1: Create New Project

1. In Vivado GUI: **File → New Project**
2. **Project Name**: `tpu_basys3`
3. **Project Location**: `/scratch/network/YOUR_NETID/tpu_build/adroit_upload`
4. Click **Next**
5. **Project Type**: Select **RTL Project**
6. Click **Next**
7. **Add Sources**: Click **Next** (we'll add EDIF later)
8. **Add Constraints**: Click **Next** (we'll add XDC later)
9. **Default Part**: 
   - Search for: `xc7a35tcpg236-1`
   - Select: **xc7a35tcpg236-1** (Basys 3)
   - Click **Next**
10. Click **Finish**

#### Step 5.2: Import EDIF Netlist

1. In **Sources** window (left panel), right-click **Design Sources**
2. Select **Add Sources...**
3. Choose **Add or create design sources**
4. Click **Add Files...**
5. Navigate to and select: `tpu_basys3.edif`
6. Click **OK**
7. **Copy sources into project**: Leave **unchecked** (file is already in project directory)
8. Click **Finish**
9. When prompted, set **Top module**: `tpu_top_wrapper`
10. Click **OK**

#### Step 5.3: Add Constraints

1. In **Sources** window, right-click **Constraints**
2. Select **Add Sources...**
3. Choose **Add or create constraints**
4. Click **Add Files...**
5. Navigate to and select: `basys3_nextpnr.xdc`
6. Click **OK**
7. Click **Finish**

#### Step 5.4: Set Top Module

1. In **Sources** window, right-click **tpu_top_wrapper** (under Design Sources)
2. Select **Set as Top**
3. Verify it shows as top module

#### Step 5.5: Run Implementation

1. In **Flow Navigator** (left panel), expand **PROGRAM AND DEBUG**
2. Click **Generate Bitstream**
3. Vivado will automatically run:
   - **Synthesis** (skipped - EDIF is pre-synthesized)
   - **Implementation** (opt_design → place_design → route_design)
   - **Bitstream Generation**
4. **Wait 15-35 minutes** for completion
5. Watch progress in **Design Runs** window (bottom)

#### Step 5.6: Find Bitstream

After completion, the bitstream will be at:
```
~/tpu_build/adroit_upload/tpu_basys3.runs/impl_1/tpu_top_wrapper.bit
```

Or if you used scratch:
```
/scratch/network/YOUR_NETID/tpu_build/adroit_upload/tpu_basys3.runs/impl_1/tpu_top_wrapper.bit
```

### Method 2: Automated TCL Script (Faster)

If you prefer automation:

```bash
# On Adroit, in the adroit_upload directory
module load vivado/2023.1
vivado -mode batch -source build_tpu_vivado.tcl
```

This will:
- Create project
- Import EDIF
- Add constraints
- Run implementation
- Generate bitstream

**Output**: `build/tpu_top_wrapper.bit`

## Step 6: Download Bitstream to Mac

```bash
# From your Mac terminal
cd /Users/abiralshakya/Documents/tpu_to_fpga

# Download bitstream
scp YOUR_NETID@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_basys3.runs/impl_1/tpu_top_wrapper.bit build/

# Or from scratch:
scp YOUR_NETID@adroit.princeton.edu:/scratch/network/YOUR_NETID/tpu_build/adroit_upload/tpu_basys3.runs/impl_1/tpu_top_wrapper.bit build/

# Verify
ls -lh build/tpu_top_wrapper.bit
# Should be ~2-5 MB
```

## Step 7: Program FPGA

```bash
# On your Mac
cd /Users/abiralshakya/Documents/tpu_to_fpga
openFPGALoader -b basys3 build/tpu_top_wrapper.bit
```

## Troubleshooting

### X11 Forwarding Issues

**Problem**: GUI doesn't open

**Solutions**:
```bash
# Try more permissive forwarding
ssh -Y YOUR_NETID@adroit.princeton.edu

# Check X11 is working
echo $DISPLAY
# Should show something like: localhost:10.0

# Test with simple app
xeyes
```

**If still not working**:
- Make sure XQuartz is running on Mac
- Check XQuartz preferences: "Allow connections from network clients"
- Restart XQuartz and SSH session

### Vivado Module Not Found

```bash
# Check available modules
module avail vivado

# Try different version
module load vivado/2022.1
# or
module load vivado/2023.2
```

### EDIF Import Errors

**Problem**: "Top module not found"

**Solution**: 
- Make sure you set top module to `tpu_top_wrapper`
- Check EDIF file is valid: `head tpu_basys3.edif`

### Build Takes Too Long

**Normal times**:
- opt_design: 1-2 minutes
- place_design: 5-15 minutes
- route_design: 5-15 minutes
- write_bitstream: 1-2 minutes
- **Total**: 15-35 minutes

**If it's taking longer**:
- Check Adroit cluster load: `squeue -u YOUR_NETID`
- Design is large (6706 cells) - be patient

## Quick Reference

### Upload Package
```bash
# On Mac
scp tpu_vivado_package.tar.gz YOUR_NETID@adroit.princeton.edu:~/tpu_build/
```

### Extract on Adroit
```bash
# On Adroit
cd ~/tpu_build
tar -xzf tpu_vivado_package.tar.gz
cd adroit_upload
```

### Launch Vivado GUI
```bash
# On Adroit
module load vivado/2023.1
ssh -X YOUR_NETID@adroit.princeton.edu  # From Mac
vivado &
```

### Download Bitstream
```bash
# From Mac
scp YOUR_NETID@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_basys3.runs/impl_1/tpu_top_wrapper.bit build/
```

## Summary

1. **Prepare**: Create `tpu_vivado_package.tar.gz` (1.4 MB)
2. **Upload**: `scp` to Adroit
3. **Extract**: `tar -xzf` on Adroit
4. **Access**: SSH with X11 forwarding, launch Vivado GUI
5. **Build**: Import EDIF, add constraints, generate bitstream
6. **Download**: `scp` bitstream back to Mac
7. **Program**: `openFPGALoader` on Mac

**Total time**: ~30-45 minutes (mostly waiting for Vivado)

