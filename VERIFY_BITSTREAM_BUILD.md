# Verify Bitstream Build - UART DMA Issue

## Problem

Both bitstreams (`tpu_top_wrapper.bit` and `tpu_top_wrapper_rev2.bit`) fail to respond to UART commands. This suggests the bitstreams may not have UART DMA properly connected.

## Critical Questions

1. **How was `tpu_top_wrapper_rev2.bit` built?**
   - Was it built with Vivado on Adroit?
   - Was it built locally?
   - Which constraints file was used?

2. **Does the bitstream actually include UART DMA?**
   - Check build logs
   - Verify synthesis included `uart_dma_basys3_improved.sv`
   - Verify constraints file was used

3. **Are UART pins properly routed?**
   - Check Vivado implementation report
   - Verify pins B18/A18 are actually used

## How to Verify Bitstream Contents

### Option 1: Check Vivado Implementation Report

If you have access to the Vivado project that built the bitstream:

```tcl
# In Vivado TCL console
open_project <path_to_vivado_project>
open_run impl_1
report_utilization -file utilization.rpt
report_io -file io.rpt
```

Look for:
- `uart_dma_basys3_improved` in utilization report
- Pins B18/A18 assigned to `uart_tx`/`uart_rx` in IO report

### Option 2: Rebuild with Verification

Rebuild the bitstream and verify each step:

```bash
# 1. Generate EDIF (verify UART DMA is included)
yosys synthesis/yosys/synth_basys3_vivado.ys

# Check EDIF contains UART DMA (grep for module names)
grep -i "uart" build/tpu_basys3.edif | head -20

# 2. Build with Vivado (on Adroit)
# Upload EDIF and constraints
scp build/tpu_basys3.edif constraints/basys3_nextpnr.xdc as0714@adroit.princeton.edu:~/tpu_build/

# 3. In Vivado TCL (on Adroit)
read_edif tpu_basys3.edif
set_property top tpu_top_wrapper [current_fileset]
read_xdc basys3_nextpnr.xdc
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper

# 4. Check UART pins are assigned
report_io | grep -i uart

# 5. Run implementation
opt_design
place_design
route_design

# 6. Verify UART pins are routed
report_route_status -file route.rpt
grep -i "uart" route.rpt

# 7. Generate bitstream
write_bitstream -force tpu_top_wrapper_verified.bit
```

### Option 3: Test with Simple UART Loopback

Create a minimal test design that just loops UART RX to TX:

```systemverilog
module uart_loopback (
    input  clk,
    input  uart_rx,
    output uart_tx
);
    assign uart_tx = uart_rx;  // Simple loopback
endmodule
```

If this works, UART pins are fine, issue is with UART DMA module.

## Most Likely Issues

1. **Constraints file not used**: Bitstream built without `basys3_nextpnr.xdc`
2. **UART pins not routed**: Pins B18/A18 not actually assigned
3. **UART DMA optimized away**: Module exists but not connected
4. **Wrong top module**: Bitstream uses wrong top (not `tpu_top_wrapper`)

## Quick Check

Run this to see if bitstream has UART-related strings:

```bash
strings build/tpu_top_wrapper_rev2.bit | grep -i uart | head -10
```

If you see UART-related strings, the module might be there but not connected.

## Next Steps

1. **Verify build process**: How was rev2.bit actually created?
2. **Check constraints**: Was `basys3_nextpnr.xdc` used?
3. **Rebuild with verification**: Rebuild and check each step
4. **Test simple loopback**: Verify UART pins work at all

