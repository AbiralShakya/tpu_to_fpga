# Enable Streaming Mode - Quick Start

## ✅ What's Done

1. **Updated `basys3_test_interface.sv`**:
   - Instantiates `uart_dma_basys3_improved` (supports command 0x07)
   - Multiplexes between UART DMA and physical interface
   - UART takes priority when active

2. **Updated Yosys scripts**:
   - Read `uart_dma_basys3_improved.sv`
   - Read `uart_rx_improved.sv` (15x oversampling)

## 🚀 Quick Rebuild Steps

### Step 1: Regenerate EDIF (On Mac)

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga
yosys synthesis/yosys/synth_basys3_vivado.ys
```

**Output**: `build/tpu_basys3.edif`

### Step 2: Upload to Adroit

```bash
scp build/tpu_basys3.edif as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/
```

### Step 3: Rebuild Bitstream (On Adroit - Vivado TCL Console)

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

**Wait**: 15-30 minutes

### Step 4: Download & Test

```bash
# Download
scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper.bit build/

# Program
openFPGALoader -b basys3 build/tpu_top_wrapper.bit

# Test streaming mode (should work now!)
UART_PORT=$(ls /dev/tty.usbserial-* | tail -1)
python3 python/test_all_instructions.py $UART_PORT
```

## What Changed

**Before**:
- ❌ No command 0x07 (STREAM_INSTR)
- ❌ Only legacy mode
- ❌ Basic UART RX

**After** (after rebuild):
- ✅ Command 0x07 supported
- ✅ Streaming mode with ACK
- ✅ Improved UART RX (15x oversampling)
- ✅ Better noise immunity

## One-Liner Summary

```bash
# 1. Rebuild EDIF
yosys synthesis/yosys/synth_basys3_vivado.ys

# 2. Upload
scp build/tpu_basys3.edif as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/

# 3. On Adroit: Run Vivado commands (see Step 3 above)

# 4. Download & test
scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper.bit build/
openFPGALoader -b basys3 build/tpu_top_wrapper.bit
python3 python/test_all_instructions.py /dev/tty.usbserial-XXXXX
```

**Total time**: ~30-45 minutes (mostly waiting for Vivado)

