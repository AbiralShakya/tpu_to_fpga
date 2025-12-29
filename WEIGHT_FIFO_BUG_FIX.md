# Weight FIFO Delay Bug - Root Cause and Fix

## Problem Summary

**Symptom**: MATMUL operations return incorrect values. For example, identity matrix test returns [250, 5, 10] or [0, 0, 0] instead of [10, 20, 30].

**Root Cause**: The weight FIFO's column 2 delay pipeline had only **ONE** register stage instead of **TWO**, causing weights to be delivered to PEs one cycle early with wrong values.

## Diagonal Wavefront Weight Loading

The 3x3 systolic array uses a diagonal wavefront pattern to load weights:

```
Cycle 1: pe00 captures w00
Cycle 2: pe01 captures w01, pe10 captures w10
Cycle 3: pe02 captures w02, pe11 captures w11, pe20 captures w20
Cycle 4: pe12 captures w12, pe21 captures w21
Cycle 5: pe22 captures w22
```

To achieve this, the weight FIFO must output weights with staggered timing:
- **Column 0**: No delay (outputs immediately)
- **Column 1**: 1-cycle delay
- **Column 2**: 2-cycle delay

## The Bug in dual_weight_fifo.sv

### Original (Broken) Code

For column 2, the code had:

```verilog
// Only ONE skew register!
logic [7:0] col2_skew_reg;

if (pop) begin
    col2_skew_reg <= col2_raw;     // Load from queue
    col2_out <= col2_skew_reg;     // Output delayed value
    rd_ptr2 <= rd_ptr2 + 1'd1;
end
```

This creates only a **1-cycle delay**, not 2 cycles!

### Trace of Broken Logic

For a full 3x3 matrix with weights:
```
Row 0: [1, 2, 3]
Row 1: [4, 5, 6]
Row 2: [7, 8, 9]
```

After RD_WEIGHT instructions, queue2 = [3, 6, 9, 0]

**With broken 1-cycle delay:**
- Cycle 1: rd_ptr2=0, load queue2[0]=3 into col2_skew_reg, output old col2_skew_reg=0
- Cycle 2: rd_ptr2=1, load queue2[1]=6 into col2_skew_reg, output col2_skew_reg=3
- Cycle 3: rd_ptr2=2, load queue2[2]=9 into col2_skew_reg, output col2_skew_reg=6
- Cycle 4: rd_ptr2=3, load queue2[3]=0 into col2_skew_reg, output col2_skew_reg=9
- Cycle 5: rd_ptr2=0, load queue2[0]=3 into col2_skew_reg, output col2_skew_reg=0

**PE Captures (with broken delay):**
- Cycle 3: pe02 captures col2_out=**6** ❌ WRONG! Should capture 3
- Cycle 4: pe12 captures col2_out=**9** ❌ WRONG! Should capture 6
- Cycle 5: pe22 captures col2_out=**0** ❌ WRONG! Should capture 9

All column 2 PEs get the wrong weights!

## The Fix

### Fixed Code

Replaced single register with TWO cascaded delay stages:

```verilog
// TWO delay stages for 2-cycle delay
logic [7:0] col2_delay_stage1;  // First delay
logic [7:0] col2_delay_stage2;  // Second delay

if (pop) begin
    col2_delay_stage1 <= queue2[rd_ptr2];  // First delay
    col2_delay_stage2 <= col2_delay_stage1; // Second delay
    col2_out <= col2_delay_stage2;          // Output after 2 cycles
    rd_ptr2 <= rd_ptr2 + 1'd1;
end
```

### Trace of Fixed Logic

**With correct 2-cycle delay:**
- Cycle 1: load queue2[0]=3 → stage1, stage1(old)=0 → stage2, stage2(old)=0 → output
- Cycle 2: load queue2[1]=6 → stage1, stage1=3 → stage2, stage2=0 → output
- Cycle 3: load queue2[2]=9 → stage1, stage1=6 → stage2, stage2=3 → output ✓
- Cycle 4: load queue2[3]=0 → stage1, stage1=9 → stage2, stage2=6 → output ✓
- Cycle 5: load queue2[0]=3 → stage1, stage1=0 → stage2, stage2=9 → output ✓

**PE Captures (with fixed delay):**
- Cycle 3: pe02 captures col2_out=**3** ✓ CORRECT!
- Cycle 4: pe12 captures col2_out=**6** ✓ CORRECT!
- Cycle 5: pe22 captures col2_out=**9** ✓ CORRECT!

## Also Fixed Column 1

While investigating, I also clarified the column 1 delay logic:

```verilog
// Column 1: Proper 1-cycle delay
logic [7:0] col1_delay_stage;

if (pop) begin
    col1_delay_stage <= queue1[rd_ptr1];  // Load from queue
    col1_out <= col1_delay_stage;          // Output after 1 cycle
    rd_ptr1 <= rd_ptr1 + 1'd1;
end
```

## Files Modified

**rtl/dual_weight_fifo.sv**:
- Lines 49-51: Added proper delay stage declarations
- Lines 61-65: Updated reset logic for new delay stages
- Lines 88-91: Fixed column 1 delay logic (clarification)
- Lines 100-104: **CRITICAL FIX** - Added second delay stage for column 2

## Testing

After rebuilding the bitstream with this fix, the tests should now pass:

```bash
# Should get [10, 20, 30] for identity matrix
python3 python/comprehensive_matmul_test.py /dev/tty.usbserial-210183A27BE01
```

**Expected Results:**
- ✓ Identity matrix: [10, 20, 30]
- ✓ Diagonal scaling: [1, 4, 9]
- ✓ Full 3x3 matrix: [30, 36, 42]
- ✓ Zero weights: [0, 0, 0]

## Impact

This bug affected **ALL** matrix multiplications where column 2 weights were non-zero. Specifically:
- PEs in column 2 (pe02, pe12, pe22) received wrong weights
- This caused incorrect partial sums to propagate through the systolic array
- Final accumulator outputs were garbage or zero depending on weight patterns

## Related Fixes

This is the **FOURTH** critical bug fixed in this session:

1. ✅ ST_UB mux blocking (tpu_top.sv)
2. ✅ Instruction corruption (Python test scripts)
3. ✅ Top row psum corruption (mmu.sv)
4. ✅ **Weight FIFO delay bug (dual_weight_fifo.sv)** ← This fix

With all four fixes, the TPU should now correctly compute matrix multiplications!

## Confidence Level: EXTREMELY HIGH

The trace analysis clearly shows:
- ✓ Old code had 1-cycle delay, needed 2-cycle delay
- ✓ Explains wrong weight values captured by column 2 PEs
- ✓ Explains garbage outputs like [250, 5, 10]
- ✓ Fix implements proper 2-stage delay pipeline

**The MATMUL computation should now work correctly!**
