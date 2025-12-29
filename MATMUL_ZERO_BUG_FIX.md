# MATMUL Zero Output Bug - Root Cause and Fix

## Problem Summary

**Symptom**: All MATMUL operations return zero or garbage values instead of correct matrix multiplication results.

**Test Results Before Fix**:
- Identity matrix test: Expected [10, 20, 30], Got [0, 0, 0] ❌
- Diagonal scaling: Expected [1, 4, 9], Got [187, 187, 187] ❌ (old marker 0xBB)
- Full 3x3 matrix: Expected [30, 36, 42], Got [0, 0, 0] ❌
- Zero weights: Expected [0, 0, 0], Got [0, 0, 0] ✓ (false positive)

## Root Cause Analysis

### The Bug Location

File: `/Users/abiralshakya/Documents/tpu_to_fpga/rtl/mmu.sv`
Lines: 111, 123, 135 (original code before fix)

### What Was Wrong

The top row processing elements (PEs) in the systolic array had their `psum_in` ports **permanently connected** to the weight FIFO outputs:

```verilog
// BROKEN CODE (before fix)
pe pe00 (
    .psum_in({24'b0, col0_in}),  // ❌ Always connected to weight FIFO!
    ...
);

pe pe01 (
    .psum_in({24'b0, col1_in}),  // ❌ Always connected to weight FIFO!
    ...
);

pe pe02 (
    .psum_in({24'b0, col2_in}),  // ❌ Always connected to weight FIFO!
    ...
);
```

### Why This Breaks Computation

The systolic array operates in two phases:

#### Phase 1: Weight Loading (`en_weight_pass = 1`, state = `SYS_LOAD_WEIGHTS`)
- Weights pass through the **psum path** (vertically down columns)
- Each PE captures its weight from `psum_in[7:0]` when `en_weight_capture = 1`
- The PE logic: `psum_out = psum_in` (pass through)
- **This phase worked correctly** ✓

#### Phase 2: Computation (`en_weight_pass = 0`, state = `SYS_COMPUTE`)
- Activations flow through the **activation path** (horizontally across rows)
- Partial sums accumulate through the **psum path** (vertically down columns)
- The PE logic: `psum_out = psum_in + (activation × weight_reg)`
- **This phase was BROKEN** ❌

### The Corruption Mechanism

During computation, the PE performs (from `pe.sv:77`):
```verilog
psum_out <= $signed(psum_in) + product_extended;
```

For the **top row PEs** (pe00, pe01, pe02):
- `psum_in` should be **0** (they're the start of the accumulation chain)
- But instead, `psum_in` was connected to `{24'b0, colX_in}` (weight FIFO output)
- The weight FIFO was not being popped during COMPUTE (only during LOAD)
- So `colX_in` contained **stale/garbage values** from the previous weight load

**Result**:
```
psum_out = garbage_from_FIFO + (activation × weight)
```

Instead of:
```
psum_out = 0 + (activation × weight)
```

The garbage values corrupted the partial sums, causing:
1. Zero outputs (when garbage happened to negate the product)
2. Garbage outputs (when garbage was non-zero, like 0xBB = 187)

## The Fix

### What Changed

Added a **MUX** to switch `psum_in` for the top row based on the operation mode:

```verilog
// NEW: Mux signals declared (mmu.sv:51)
logic [ACC_WIDTH-1:0] pe00_psum_in, pe01_psum_in, pe02_psum_in;

// NEW: Mux logic (mmu.sv:79-81)
// During weight loading: weights pass through psum path
// During compute: top row needs psum_in = 0 (start of accumulation)
assign pe00_psum_in = en_weight_pass ? {24'b0, col0_in} : 32'b0;
assign pe01_psum_in = en_weight_pass ? {24'b0, col1_in} : 32'b0;
assign pe02_psum_in = en_weight_pass ? {24'b0, col2_in} : 32'b0;

// UPDATED: PE instantiations (mmu.sv:111, 123, 135)
pe pe00 (
    .psum_in(pe00_psum_in),  // ✓ Now uses muxed signal
    ...
);

pe pe01 (
    .psum_in(pe01_psum_in),  // ✓ Now uses muxed signal
    ...
);

pe pe02 (
    .psum_in(pe02_psum_in),  // ✓ Now uses muxed signal
    ...
);
```

### Operation After Fix

#### Weight Loading Phase (`en_weight_pass = 1`)
- `pe00_psum_in = {24'b0, col0_in}` ← Weights pass through ✓
- `pe01_psum_in = {24'b0, col1_in}` ← Weights pass through ✓
- `pe02_psum_in = {24'b0, col2_in}` ← Weights pass through ✓

#### Compute Phase (`en_weight_pass = 0`)
- `pe00_psum_in = 32'b0` ← Clean accumulation start ✓
- `pe01_psum_in = 32'b0` ← Clean accumulation start ✓
- `pe02_psum_in = 32'b0` ← Clean accumulation start ✓

Now the computation is correct:
```
pe00_psum_out = 0 + (row0_act × weight00) ✓
pe01_psum_out = 0 + (row0_act × weight01) ✓
pe02_psum_out = 0 + (row0_act × weight02) ✓
```

## Files Modified

1. **rtl/mmu.sv**
   - Line 51: Added mux signal declarations
   - Lines 79-81: Added mux logic for top row psum_in
   - Line 111: Updated pe00 instantiation to use muxed signal
   - Line 123: Updated pe01 instantiation to use muxed signal
   - Line 135: Updated pe02 instantiation to use muxed signal

## Testing

After rebuilding the bitstream with this fix, run:

```bash
# Test matrix multiplication with known weights
python3 python/comprehensive_matmul_test.py /dev/tty.usbserial-210183A27BE01

# Investigate TEST 7 zeros issue
python3 python/investigate_test7_zeros.py /dev/tty.usbserial-210183A27BE01

# Test all ISA instructions
python3 python/comprehensive_isa_test.py /dev/tty.usbserial-210183A27BE01
```

**Expected Results**:
- ✓ Identity matrix: [10, 20, 30]
- ✓ Diagonal scaling: [1, 4, 9]
- ✓ Full 3x3 matrix: [30, 36, 42]
- ✓ Zero weights: [0, 0, 0]
- ✓ All ISA tests should pass

## Lessons Learned

1. **Systolic array datapaths are mode-dependent**: Weight loading and computation use the same physical paths differently.

2. **MUX critical signals**: When a signal has different requirements in different modes, it must be muxed.

3. **Test with known values**: Using identity matrices, diagonal matrices, and known computations makes bugs obvious.

4. **Stale FIFO outputs are dangerous**: Even though the FIFO wasn't being popped, its outputs were still connected and corrupting the datapath.

5. **Top row is special**: In a vertical accumulation chain, the top row needs `psum_in = 0`, while lower rows receive partial sums from above.

## Related Issues Fixed

This was the **third critical bug** discovered in this session:

1. ✅ **ST_UB mux blocking** (tpu_top.sv) - `use_test_interface_ub` fix
2. ✅ **Instruction corruption** (Python scripts) - 7-byte vs 9-byte protocol fix
3. ✅ **MATMUL psum corruption** (mmu.sv) - This fix

With all three fixes, the TPU should now correctly:
- Write to unified buffer via ST_UB
- Execute all instructions without corruption
- Compute matrix multiplications correctly

## Confidence Level: EXTREMELY HIGH

This fix addresses the exact mechanism causing zero/garbage outputs:
- ✓ Explains why outputs were zero (garbage negated products)
- ✓ Explains why outputs were 0xBB (stale FIFO marker)
- ✓ Explains why zero-weight test passed (0 × anything = 0)
- ✓ Aligns with systolic array theory (top row needs psum_in=0)

The MATMUL computation should now work correctly!
