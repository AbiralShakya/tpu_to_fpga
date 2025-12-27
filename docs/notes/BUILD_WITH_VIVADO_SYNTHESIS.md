# Build TPU with Vivado Synthesis (Bypassing Yosys)

**Why:** Yosys has memory inference issues with the unified_buffer's conditional write logic. Vivado's synthesis handles this correctly.

## Step 1: Upload Files to Adroit

```bash
# Upload all RTL files
scp -r rtl/ as0714@adroit.princeton.edu:~/tpu_build/

# Upload constraints
scp constraints/basys3_nextpnr.xdc as0714@adroit.princeton.edu:~/tpu_build/

# Upload Vivado TCL script
scp vivado/build_with_vivado_synthesis.tcl as0714@adroit.princeton.edu:~/tpu_build/
```

## Step 2: Build on Adroit

```bash
# SSH to Adroit
ssh as0714@adroit.princeton.edu

# Load Vivado module
module load vivado/2023.1

# Create build directory
mkdir -p ~/tpu_build/vivado_direct
cd ~/tpu_build

# Run Vivado synthesis
vivado -mode batch -source build_with_vivado_synthesis.tcl
```

This will:
- Synthesize RTL directly in Vivado (no Yosys EDIF)
- Run implementation
- Generate bitstream: `~/tpu_build/vivado_direct/tpu_basys3_vivado_synth.bit`

## Step 3: Download Bitstream

```bash
# From Mac
scp as0714@adroit.princeton.edu:~/tpu_build/vivado_direct/tpu_basys3_vivado_synth.bit build/
```

## Step 4: Program and Test

```bash
# Program FPGA
openFPGALoader -b basys3 build/tpu_basys3_vivado_synth.bit

# Test with 32-byte writes
python3 python/comprehensive_test.py
```

## What's Fixed

- ✅ Bank selection: unified_buffer now uses same bank for read/write (not opposite)
- ✅ Synthesis: Vivado handles conditional memory writes correctly
- ✅ Test data: comprehensive_test.py now writes 32-byte chunks

## Expected Result

If Phase 5 shows:
```
✓✓✓ DATA MATCH! Write/Read working!
```

Then the unified buffer fix is successful!
