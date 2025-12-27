# Enable Streaming Mode - Complete Setup Guide

## What Changed

✅ **Updated `basys3_test_interface.sv`**:
- Added instantiation of `uart_dma_basys3_improved` module
- Added multiplexing between UART DMA and physical interface
- UART DMA takes priority when active

✅ **Updated Yosys synthesis scripts**:
- `synthesis/yosys/synth_basys3_vivado.ys` - Now reads improved modules
- `synthesis/yosys/synth_basys3.ys` - Now reads improved modules

## Step-by-Step: Rebuild with Streaming Mode

### Step 1: Regenerate EDIF (On Mac)

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga
yosys synthesis/yosys/synth_basys3_vivado.ys
```

**Check**: `ls -lh build/tpu_basys3.edif` (should be ~1.4 MB)

### Step 2: Upload to Adroit

```bash
# From Mac
scp build/tpu_basys3.edif as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/
```

### Step 3: Rebuild Bitstream (On Adroit)

**In Vivado TCL Console**:
```tcl
cd /home/as0714/tpu_build/adroit_upload
cp tpu_basys3.edif tpu_top_wrapper.edif
read_edif tpu_top_wrapper.edif
set_property top tpu_top_wrapper [current_fileset]
read_xdc basys3_nextpnr.xdc
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper
opt_design
place_design
route_design
write_bitstream -force tpu_top_wrapper.bit
```

**Wait**: 15-30 minutes for P&R

### Step 4: Download New Bitstream

```bash
# From Mac
scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper.bit build/
```

### Step 5: Program FPGA

```bash
openFPGALoader -b basys3 build/tpu_top_wrapper.bit
```

### Step 6: Test Streaming Mode

```bash
# Find UART port
UART_PORT=$(ls /dev/tty.usbserial-* | tail -1)

# Test with streaming mode (should work now!)
python3 python/test_all_instructions.py $UART_PORT
```

## What's Different Now

**Old Bitstream**:
- ❌ No command 0x07 (STREAM_INSTR)
- ❌ Only legacy mode (WRITE_INSTR + EXECUTE)
- ❌ Basic UART RX

**New Bitstream** (after rebuild):
- ✅ Command 0x07 (STREAM_INSTR) supported
- ✅ Streaming mode with ACK
- ✅ Improved UART RX (15x oversampling)
- ✅ Better noise immunity

## Quick Test After Rebuild

```bash
# Program FPGA
openFPGALoader -b basys3 build/tpu_top_wrapper.bit

# Test streaming mode
python3 python/test_all_instructions.py /dev/tty.usbserial-XXXXX
```

**Expected**: All tests should pass with streaming mode enabled! ✅

## Summary

1. ✅ Code updated: `basys3_test_interface.sv` now uses `uart_dma_basys3_improved`
2. ✅ Synthesis scripts updated: Read improved modules
3. ⏳ **Next**: Rebuild EDIF → Upload to Adroit → Rebuild bitstream → Test!

**Total time**: ~30-45 minutes (mostly waiting for Vivado)

