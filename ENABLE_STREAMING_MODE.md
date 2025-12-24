# Enable Streaming Mode - Step by Step Guide

## Problem

Your current bitstream uses `uart_dma_basys3.sv` which **doesn't support** command 0x07 (STREAM_INSTR). It only supports legacy commands 0x01-0x06.

## Solution

Switch to `uart_dma_basys3_improved.sv` which supports streaming mode (0x07) and has better UART RX with oversampling.

## Step 1: Update basys3_test_interface.sv

The UART DMA module is instantiated in `basys3_test_interface.sv`. We need to change it from `uart_dma_basys3` to `uart_dma_basys3_improved`.

**File**: `rtl/basys3_test_interface.sv`

**Change**:
- Replace `uart_dma_basys3` with `uart_dma_basys3_improved`
- Replace `uart_rx` with `uart_rx_improved` (if needed)

## Step 2: Update Yosys Synthesis Script

**File**: `synthesis/yosys/synth_basys3_vivado.ys`

**Change line 29**:
```tcl
# OLD:
read_verilog -sv rtl/uart_dma_basys3.sv

# NEW:
read_verilog -sv rtl/uart_dma_basys3_improved.sv
read_verilog -sv rtl/uart_rx_improved.sv
```

Also update the regular synthesis script:
**File**: `synthesis/yosys/synth_basys3.ys`

**Change line 28**:
```tcl
# OLD:
read_verilog -sv rtl/uart_dma_basys3.sv

# NEW:
read_verilog -sv rtl/uart_dma_basys3_improved.sv
read_verilog -sv rtl/uart_rx_improved.sv
```

## Step 3: Rebuild Bitstream

### On Your Mac (Generate EDIF):

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga
yosys synthesis/yosys/synth_basys3_vivado.ys
```

This generates: `build/tpu_basys3.edif`

### On Adroit (Generate Bitstream):

1. **Upload new EDIF**:
   ```bash
   # From Mac
   scp build/tpu_basys3.edif as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/
   ```

2. **In Vivado TCL Console**:
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

3. **Download new bitstream**:
   ```bash
   # From Mac
   scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper.bit build/
   ```

## Step 4: Test Streaming Mode

```bash
# Program FPGA
openFPGALoader -b basys3 build/tpu_top_wrapper.bit

# Test with streaming mode
UART_PORT=$(ls /dev/tty.usbserial-* | tail -1)
python3 python/test_all_instructions.py $UART_PORT
```

Now streaming mode should work! ✅

## What Changed

**Old (uart_dma_basys3.sv)**:
- Commands: 0x01-0x06 only
- No streaming mode
- Basic UART RX

**New (uart_dma_basys3_improved.sv)**:
- Commands: 0x01-0x07 (adds STREAM_INSTR)
- Streaming mode with ACK
- Improved UART RX with 15x oversampling
- Better noise immunity

## Quick Summary

1. Update `basys3_test_interface.sv` to use `uart_dma_basys3_improved`
2. Update Yosys scripts to read `uart_dma_basys3_improved.sv` and `uart_rx_improved.sv`
3. Rebuild EDIF with Yosys
4. Rebuild bitstream on Adroit
5. Test with streaming mode!

