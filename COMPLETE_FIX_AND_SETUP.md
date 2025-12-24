# Complete Fix and Setup Guide

## Root Cause Analysis

**Diagnostic Results**:
- ✅ Both serial ports tested: `/dev/tty.usbserial-210183A27BE00` and `BE01`
- ❌ **NEITHER port responds** (0 bytes received)
- ✅ Commands ARE being sent successfully (driver works)
- ❌ FPGA NEVER responds (bitstream issue)

**Conclusion**: The bitstreams (`tpu_top_wrapper.bit` and `tpu_top_wrapper_rev2.bit`) likely don't have UART DMA properly connected or UART pins aren't routed.

## Solution: Rebuild Bitstream with Verification

### Step 1: Regenerate EDIF (On Mac)

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga
yosys synthesis/yosys/synth_basys3_vivado.ys
```

**Verify EDIF contains UART DMA**:
```bash
grep -i "uart" build/tpu_basys3.edif | head -10
```

You should see references to `uart_dma_basys3_improved`, `uart_rx_improved`, `uart_tx`.

### Step 2: Upload to Adroit

```bash
scp build/tpu_basys3.edif constraints/basys3_nextpnr.xdc as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/
```

### Step 3: Build with Vivado (On Adroit - TCL Console)

**IMPORTANT**: Use this exact sequence to ensure UART pins are routed:

```tcl
cd ~/tpu_build/adroit_upload

# Copy EDIF to match top module name (Vivado requirement)
cp tpu_basys3.edif tpu_top_wrapper.edif

# Read EDIF
read_edif tpu_top_wrapper.edif

# Set top module
set_property top tpu_top_wrapper [current_fileset]

# Read constraints (CRITICAL - this assigns UART pins)
read_xdc basys3_nextpnr.xdc

# Link design
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper

# VERIFY UART PINS ARE ASSIGNED (do this before implementation!)
report_io | grep -i uart
# Should show:
#   uart_tx    B18
#   uart_rx    A18

# If UART pins are NOT shown, the constraints weren't applied correctly!

# Run implementation
opt_design
place_design
route_design

# VERIFY UART PINS ARE ROUTED
report_route_status
# Check that all nets are routed (no unplaced/unrouted)

# Generate bitstream
write_bitstream -force tpu_top_wrapper_verified.bit
```

### Step 4: Download and Test

```bash
# Download from Adroit
scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper_verified.bit build/

# Program FPGA
openFPGALoader -b basys3 build/tpu_top_wrapper_verified.bit

# Test basic connectivity
python3 python/test_uart_basic.py /dev/tty.usbserial-XXXXX

# If that works, test ISA
python3 python/test_isa_legacy.py /dev/tty.usbserial-XXXXX
```

## Quick Automated Rebuild

Use the script I created:

```bash
./REBUILD_VERIFIED_BITSTREAM.sh
```

This will:
1. Generate EDIF
2. Verify UART modules are included
3. Verify constraints file has UART pins
4. Give you exact commands for Adroit

## Setup for New Bitstream (tpu_top_wrapper_rev2.bit)

If you want to use the existing `tpu_top_wrapper_rev2.bit`:

### Step 1: Verify It Was Built Correctly

**Question**: How was `tpu_top_wrapper_rev2.bit` built?
- Was it built on Adroit with Vivado?
- Was `basys3_nextpnr.xdc` used?
- Were UART pins verified in the IO report?

**If you're not sure**, rebuild it using the steps above.

### Step 2: Program and Test

```bash
# Program the new bitstream
openFPGALoader -b basys3 build/tpu_top_wrapper_rev2.bit

# Test basic connectivity
python3 python/test_uart_basic.py /dev/tty.usbserial-XXXXX

# If that works:
# Test legacy mode (commands 0x01-0x06)
python3 python/test_isa_legacy.py /dev/tty.usbserial-XXXXX

# Test with verification
python3 python/test_isa_legacy_verified.py /dev/tty.usbserial-XXXXX

# Test streaming mode (if bitstream supports 0x07)
python3 python/test_all_instructions.py /dev/tty.usbserial-XXXXX
```

## Troubleshooting

### If Still No Response After Rebuild

1. **Check Vivado IO Report**:
   ```tcl
   report_io | grep -i uart
   ```
   Must show B18/A18 assigned to uart_tx/uart_rx

2. **Check Route Status**:
   ```tcl
   report_route_status
   ```
   All nets must be routed (no unplaced/unrouted)

3. **Check Utilization**:
   ```tcl
   report_utilization | grep -i uart
   ```
   Should show UART DMA module is present

4. **Test with Simple Loopback**:
   Create a minimal design that just loops UART RX to TX to verify pins work.

## Summary

**Current Issue**: Bitstreams don't have UART DMA responding (likely not connected/routed properly)

**Solution**: Rebuild bitstream with verification that:
1. EDIF includes UART DMA modules
2. Constraints file is used
3. UART pins (B18/A18) are assigned
4. All nets are routed

**Next Steps**: Follow the rebuild process above, verify each step, then test.

