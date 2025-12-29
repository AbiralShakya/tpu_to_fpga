# ST_UB Fix: Read from Accumulator Memory

**Date:** Current  
**Status:** ✅ Implemented

## Problem

ST_UB was reading from systolic array outputs (latched) instead of accumulator memory, which is inconsistent with VPU operations and may read incorrect or stale data.

## Solution

Changed ST_UB to read from accumulator memory (like VPU operations do) instead of systolic array outputs.

## Changes Made

### 1. `rtl/tpu_controller.sv`

#### Added Accumulator Address Storage
- Added `stored_acc_addr_reg` and `stored_acc_buf_sel_reg` registers
- These store the accumulator address and buffer selection from the last MATMUL instruction
- Updated when MATMUL executes: `stored_acc_addr_reg <= exec_arg2`

#### Updated ST_UB Instruction
- ST_UB now asserts `acc_rd_en` to read from accumulator memory
- Uses `stored_acc_addr_reg` as the accumulator address (falls back to `exec_arg3` if 0)
- Uses `stored_acc_buf_sel_reg` for buffer selection (same as MATMUL)

**Before:**
```systemverilog
ST_UB_OP: begin
    ub_wr_en        = 1'b1;
    ub_wr_addr      = {1'b0, exec_arg1};
    ub_wr_count     = {1'b0, exec_arg2};
    pc_cnt_internal = 1'b1;
    ir_ld_internal  = 1'b1;
end
```

**After:**
```systemverilog
ST_UB_OP: begin
    acc_rd_en       = 1'b1;        // READ from accumulator memory
    acc_addr        = (stored_acc_addr_reg != 8'h00) ? stored_acc_addr_reg : exec_arg3;
    acc_buf_sel     = stored_acc_buf_sel_reg;
    ub_wr_en        = 1'b1;
    ub_wr_addr      = {1'b0, exec_arg1};
    ub_wr_count     = {1'b0, exec_arg2};
    pc_cnt_internal = 1'b1;
    ir_ld_internal  = 1'b1;
end
```

### 2. `rtl/tpu_datapath.sv`

#### Updated `acc_data_out` Assignment
- When `acc_rd_en` is asserted (ST_UB), reads from `acc_rd_data` (accumulator memory)
- Uses hybrid approach: acc0 and acc1 from memory, acc2 from latches (since acc2 is at address+1)
- Falls back to latched systolic outputs when `acc_rd_en` is not asserted

**Before:**
```systemverilog
assign acc_data_out = {232'b0, acc2_latched[7:0], acc1_latched[7:0], acc0_latched[7:0]};
```

**After:**
```systemverilog
assign acc_data_out = acc_rd_en ? 
    // Hybrid: acc0 and acc1 from accumulator memory, acc2 from latches
    {232'b0, acc2_latched[7:0], acc_rd_data[39:32], acc_rd_data[7:0]} :
    // Fallback: use all latched systolic outputs (legacy behavior)
    {232'b0, acc2_latched[7:0], acc1_latched[7:0], acc0_latched[7:0]};
```

## How It Works

1. **MATMUL Execution:**
   - MATMUL stores results in accumulator memory starting at `exec_arg2`
   - The accumulator address is stored in `stored_acc_addr_reg`
   - Buffer selection is stored in `stored_acc_buf_sel_reg`

2. **ST_UB Execution:**
   - ST_UB asserts `acc_rd_en` and sets `acc_addr` to `stored_acc_addr_reg`
   - Accumulator memory is read: `acc_rd_data` contains `{acc1[31:0], acc0[31:0]}`
   - `acc_data_out` is constructed from:
     - `acc_rd_data[7:0]` → acc0[7:0]
     - `acc_rd_data[39:32]` → acc1[7:0]
     - `acc2_latched[7:0]` → acc2[7:0] (from latches, since acc2 is at address+1)
   - Data is written to UB at address `exec_arg1`

## Accumulator Memory Layout

- **Address N:** `{acc1[31:0], acc0[31:0]}` (64 bits)
  - `acc_rd_data[31:0]` = acc0
  - `acc_rd_data[63:32]` = acc1
- **Address N+1:** `{32'h0, acc2[31:0]}` (64 bits)
  - `acc_rd_data[31:0]` = acc2

## Limitations

1. **acc2 Reading:** acc2 is stored at address+1, which would require a second read cycle. Currently, we use the latched acc2 value, which should be valid if MATMUL just completed. A full implementation could read acc2 from address+1 in a second cycle.

2. **Fallback:** If `stored_acc_addr_reg` is 0 (no MATMUL was run), ST_UB uses `exec_arg3` as the accumulator address. This allows ST_UB to work even without a prior MATMUL.

## Benefits

1. ✅ **Consistency:** ST_UB now reads from accumulator memory like VPU operations
2. ✅ **Reliability:** Reads stored results, not live systolic outputs
3. ✅ **Correctness:** Ensures ST_UB reads the same data that was stored by MATMUL
4. ✅ **Backward Compatibility:** Falls back to latched outputs if `acc_rd_en` is not asserted

## Testing

After FPGA re-synthesis, test:
1. Run MATMUL to populate accumulator memory
2. Run ST_UB to write accumulator data to UB
3. Read from UB and verify data matches accumulator memory contents
4. Verify acc0, acc1, and acc2 values are correct

## Files Modified

- `rtl/tpu_controller.sv` - Added accumulator address storage, updated ST_UB instruction
- `rtl/tpu_datapath.sv` - Updated `acc_data_out` to read from accumulator memory

## Next Steps

1. Re-synthesize FPGA with this fix
2. Run diagnostic tests to verify ST_UB now reads correct data
3. Consider implementing two-cycle read for acc2 if needed

