# Verify Bitstream Build on Adroit

## Critical Issue

Even `tpu_top_wrapper_verified_rev3.bit` is not responding to UART commands. This suggests the bitstream may not have been built correctly, or UART pins weren't routed.

## Verification Steps (On Adroit)

### Step 1: Check Vivado Project (if still open)

If you still have the Vivado project open on Adroit:

```tcl
# In Vivado TCL Console
open_run impl_1

# Check UART pins are assigned
# Note: In TCL console, grep might not support -i flag
# Try these alternatives:
report_io | grep uart
# OR if that doesn't work:
# report_io | grep -E "uart"
# OR use TCL filtering:
# [report_io -of_objects [get_ports uart*]]

# Expected output:
#   uart_tx    B18    OUT    LVCMOS33
#   uart_rx    A18    IN     LVCMOS33

# If you DON'T see this, the constraints weren't applied!
```

### Step 2: Check Route Status

```tcl
# Check all nets are routed
report_route_status

# Should show:
#   All user specified timing constraints are met.
#   All user specified timing constraints are met.
#   All user specified timing constraints are met.
#   All 12345 nets successfully routed.

# If you see "unrouted" or "unplaced", that's the problem!
```

### Step 3: Check Utilization Report

```tcl
# Check UART DMA is in the design
report_utilization | grep uart

# Should show UART-related resources being used
```

### Step 4: Check Timing

```tcl
# Check timing is met
report_timing_summary

# Expected output:
# - Timing constraints met (if you see warnings about no_clock, unconstrained endpoints,
#   no_input_delay, no_output_delay - these are NORMAL and OK)
# - The warnings you see are:
#   * no_clock (172): Internal signals without clock constraints (normal)
#   * unconstrained internal endpoints (391): Internal logic paths (normal)
#   * no_input_delay (4): Inputs without timing constraints (may include switches/buttons)
#   * no_output_delay (28): Outputs without timing constraints (may include LEDs/7-seg)
#
# These warnings are EXPECTED and won't prevent the design from working.
# The important thing is that critical paths (like UART) are properly constrained.
```

## Rebuild with Verification

If verification shows issues, rebuild with these exact steps:

### On Adroit (TCL Console)

```tcl
cd ~/tpu_build/adroit_upload

# Copy EDIF to match top module name
cp tpu_basys3.edif tpu_top_wrapper.edif

# Read EDIF
read_edif tpu_top_wrapper.edif

# Set top module
set_property top tpu_top_wrapper [current_fileset]

# Read constraints (CRITICAL!)
# Option 1: Use basic constraints (no timing)
read_xdc basys3_nextpnr.xdc

# Option 2: Use constraints with timing (if you want to fix timing warnings)
# read_xdc basys3_with_timing.xdc

# Link design
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper

# ============================================================================
# VERIFY UART PINS ARE ASSIGNED (DO THIS BEFORE IMPLEMENTATION!)
# ============================================================================
report_io | grep -i uart

# MUST show:
#   uart_tx    B18
#   uart_rx    A18
#
# If NOT, stop here and check constraints file!

# Run implementation
opt_design
place_design
route_design

# ============================================================================
# VERIFY ROUTING (DO THIS BEFORE BITSTREAM GENERATION!)
# ============================================================================
report_route_status

# Must show all nets routed (0 unplaced/unrouted)

# Check UART pins are actually routed
report_route_status -of_objects [get_nets -hierarchical -filter {NAME =~ "*uart*"}]

# Generate bitstream
write_bitstream -force tpu_top_wrapper_verified_rev4.bit
```

## Common Issues

### Issue 1: UART Pins Not Assigned

**Symptom**: `report_io | grep -i uart` shows nothing

**Cause**: Constraints file not read, or wrong constraints file

**Fix**: 
- Check constraints file exists: `ls -lh basys3_nextpnr.xdc`
- Verify it has UART pins: `grep -i uart basys3_nextpnr.xdc`
- Re-read constraints: `read_xdc basys3_nextpnr.xdc`

### Issue 2: UART Pins Assigned But Not Routed

**Symptom**: `report_io` shows pins, but `report_route_status` shows unrouted nets

**Cause**: Placement/routing failed for UART nets

**Fix**:
- Check for placement errors: `report_utilization`
- Try different placement strategy: `place_design -directive Explore`
- Check timing constraints aren't too tight

### Issue 3: UART DMA Module Not in Design

**Symptom**: `report_utilization | grep -i uart` shows nothing

**Cause**: EDIF doesn't include UART DMA, or it was optimized away

**Fix**:
- Regenerate EDIF with Yosys (ensure `uart_dma_basys3_improved.sv` is included)
- Check Yosys script includes UART modules: `grep -i uart synthesis/yosys/synth_basys3_vivado.ys`

## Quick Verification Script

Create this on Adroit to verify everything:

```bash
#!/bin/bash
# verify_bitstream.sh

echo "Checking bitstream build..."

# Check EDIF exists
if [ ! -f "tpu_basys3.edif" ]; then
    echo "✗ EDIF not found!"
    exit 1
fi

# Check constraints exist
if [ ! -f "basys3_nextpnr.xdc" ]; then
    echo "✗ Constraints not found!"
    exit 1
fi

# Check constraints has UART
if ! grep -q "uart_tx" basys3_nextpnr.xdc && ! grep -q "uart_rx" basys3_nextpnr.xdc; then
    echo "✗ Constraints file doesn't have UART pins!"
    exit 1
fi

echo "✓ Files look good"
echo ""
echo "Now run Vivado TCL commands to verify routing..."
```

## Summary

The bitstream must be verified at each step:
1. ✅ EDIF includes UART DMA
2. ✅ Constraints file has UART pins
3. ✅ Constraints are read in Vivado
4. ✅ UART pins are assigned (report_io)
5. ✅ All nets are routed (report_route_status)
6. ✅ Timing is met (report_timing_summary)

Only then generate the bitstream!

