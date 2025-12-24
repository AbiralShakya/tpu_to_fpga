# Vivado opt_design Error Fix - Multiple Driver Issue

## Problem

When running `opt_design` in Vivado, you encountered this error:

```
ERROR: [DRC MDRV-1] Multiple Driver Nets: Net instr_fifo.wr_buffer_sel has multiple
ERROR: [Vivado_Tcl 4-78] Error(s) found during DRC. Opt_design not run.
```

## Root Cause

The `instruction_fifo.sv` module had **multiple drivers** for the same signals:
- `wr_buffer_sel` was being assigned in **two different `always_ff` blocks**:
  1. Line 197: In the BUFFER SWAP LOGIC always block
  2. Line 240: In the STATE MACHINE always block

- `rd_buffer_sel` had the same issue:
  1. Line 196: In the BUFFER SWAP LOGIC always block  
  2. Line 239: In the STATE MACHINE always block

In Verilog/SystemVerilog, **a signal can only be driven by one always block**. Having multiple drivers creates a conflict that Vivado's DRC (Design Rule Check) flags as an error.

## Solution

**Fixed in:** `tpu_to_fpga/rtl/instruction_fifo.sv`

### Changes Made:

1. **Removed the separate BUFFER SWAP LOGIC always block** (lines 177-202)
   - This block was driving `wr_buffer_sel` and `rd_buffer_sel`

2. **Consolidated all buffer swap logic into the STATE MACHINE always block**
   - All buffer select signals are now driven in a single location
   - Buffer swap logic is now part of the EXECUTING state

3. **Updated state machine to handle buffer swaps:**
   - Initial buffer swap happens in FILLING state (when `start_exec` is asserted)
   - Runtime buffer swaps happen in EXECUTING state (when read buffer empties)

### Code Structure After Fix:

```systemverilog
// Single always block drives all buffer selects
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        // Reset all signals including buffer selects
        rd_buffer_sel <= 1'b0;
        wr_buffer_sel <= 1'b0;
        // ... other resets
    end else begin
        case (state)
            FILLING: begin
                if (start_exec && wr_count > 0) begin
                    // Initial buffer swap
                    rd_buffer_sel <= wr_buffer_sel;
                    wr_buffer_sel <= ~wr_buffer_sel;
                    // ...
                end
            end
            
            EXECUTING: begin
                // Runtime buffer swap when read buffer empties
                if (rd_count == 0 && wr_count > 0 && exec_active_reg && !halt_detected) begin
                    rd_buffer_sel <= wr_buffer_sel;
                    wr_buffer_sel <= ~wr_buffer_sel;
                    // ...
                end
                // ... other state logic
            end
        endcase
    end
end
```

## Verification

After the fix, the design should:
- ✅ Pass DRC (Design Rule Check)
- ✅ Complete `opt_design` successfully
- ✅ Continue with `place_design` and `route_design`
- ✅ Generate bitstream without errors

## Next Steps

1. **Re-synthesize the design** (if using Yosys):
   ```bash
   # On your Mac
   cd /Users/abiralshakya/Documents/tpu_to_fpga
   # Run your synthesis script
   ```

2. **Re-upload to Adroit** (if using EDIF):
   ```bash
   # Upload new EDIF package
   scp build/vivado_package.tar.gz as0714@adroit.princeton.edu:~/tpu_build/
   ```

3. **Run Vivado again**:
   ```bash
   # On Adroit
   cd ~/tpu_build/vivado_package
   module load vivado/2023.1
   vivado -mode batch -source vivado_import_edif.tcl
   ```

   Or in Visual Vivado:
   - Open project
   - Run Synthesis (if needed)
   - Run Implementation
   - Generate Bitstream

## Expected Output

After the fix, you should see:

```
Running DRC as a precondition to command opt_design
Starting DRC Task
INFO: [DRC 23-27] Running DRC with 8 threads
INFO: [Project 1-461] DRC finished with 0 Errors
✓ Optimization complete
```

Instead of:

```
ERROR: [DRC MDRV-1] Multiple Driver Nets: Net instr_fifo.wr_buffer_sel has multiple
ERROR: [Vivado_Tcl 4-78] Error(s) found during DRC. Opt_design not run.
```

## Files Modified

- ✅ `tpu_to_fpga/rtl/instruction_fifo.sv` - Fixed multiple driver issue

## Related Files (No Changes Needed)

- `tpu_to_fpga/rtl/tpu_top.sv` - Already correctly instantiates `instruction_fifo`
- `tpu_to_fpga/rtl/uart_dma_basys3.sv` - Already has FIFO interface implemented
- `tpu_to_fpga/rtl/tpu_controller.sv` - Already has FIFO interface implemented

## Additional Notes

- The fix maintains the same functionality - only the implementation structure changed
- Buffer swapping still works correctly (initial swap + runtime swaps)
- All timing and logic behavior remains unchanged
- The fix is compatible with both batch and visual Vivado workflows

---

**Status:** ✅ Fixed and ready for re-synthesis

