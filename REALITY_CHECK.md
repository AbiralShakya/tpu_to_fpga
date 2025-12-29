# Reality Check: Code Fixes vs. FPGA Reality

**Date:** Current  
**Status:** Critical Clarification Needed

## The Honest Truth

You're absolutely right to be skeptical. Here's the reality:

### ✅ What's Fixed in CODE (but NOT in FPGA yet)

All 7 bugs have fixes in the SystemVerilog code, but **the FPGA hasn't been re-synthesized**, so:
- ❌ **None of these fixes are active on the FPGA**
- ❌ **The FPGA is still running the old broken code**
- ✅ **The fixes are ready for the next synthesis**

### ⚠️ Critical Issues That May Still Exist

Even after re-synthesis, there are potential problems:

#### 1. **ST_UB Data Source Mismatch** (CRITICAL - CONFIRMED BUG)

**Problem:** ST_UB reads from systolic array outputs (`acc0_out`, `acc1_out`, `acc2_out`), NOT from accumulator memory.

**Current Flow:**
```
MATMUL → Systolic Array → acc0_out/acc1_out/acc2_out → Latches → ST_UB
                              ↓
                    Accumulator Memory (stored but NOT read by ST_UB!)
```

**Evidence:**
- ST_UB does NOT assert `acc_rd_en` (unlike RELU/VPU operations)
- ST_UB uses `acc_data_out` which comes from latched systolic outputs
- VPU operations correctly use `acc_rd_en` → `acc_rd_data` (accumulator memory)
- **This is a design inconsistency!**

**Issues:**
- Systolic array outputs may only be valid during `systolic_active`
- After computation ends, outputs might go to zero or invalid
- Latches hold the last value, but if that value is wrong, ST_UB writes wrong data
- **Accumulator memory stores correct results, but ST_UB doesn't read from it!**

**Fix Needed:**
ST_UB should read from accumulator memory like VPU operations do:
```systemverilog
ST_UB_OP: begin
    acc_rd_en        = 1'b1;        // READ from accumulator memory
    acc_addr         = exec_arg1;   // Address to read from
    acc_buf_sel      = exec_acc_buf_sel;
    ub_wr_en         = 1'b1;
    ub_wr_addr       = {1'b0, exec_arg2};  // Write to UB at arg2
    ub_wr_count      = {1'b0, exec_arg3};
    // acc_data_out should come from acc_rd_data, not systolic outputs
    pc_cnt_internal  = 1'b1;
    ir_ld_internal   = 1'b1;
end
```

And in `tpu_datapath.sv`, `acc_data_out` should be:
```systemverilog
// For ST_UB: read from accumulator memory
// For other uses: use systolic outputs (if needed)
assign acc_data_out = (acc_rd_en) ? 
    {232'b0, acc_rd_data[31:0], acc_rd_data[63:32], 32'b0} :  // From accumulator memory
    {232'b0, acc2_latched[7:0], acc1_latched[7:0], acc0_latched[7:0]};  // From systolic (legacy)
```

#### 2. **UB Write Burst Timing** (NEEDS VERIFICATION)

**Status:** Code fix exists, but needs FPGA verification

**Potential Issues:**
- If `ub_wr_ready` timing is wrong, writes might be lost
- If burst count handling is off, wrong number of words written
- If address incrementing is wrong, data written to wrong locations

#### 3. **Accumulator Read Latency** (JUST CHANGED - NEEDS VERIFICATION)

**Status:** Just changed to combinational, but:
- ⚠️ **May cause timing violations** if BRAM can't support combinational reads
- ⚠️ **May break if accumulator memory needs registered reads for timing closure**
- ⚠️ **Needs synthesis timing analysis**

**Risk:** This change might actually BREAK things if timing doesn't work out.

#### 4. **Weight Capture Timing** (NEEDS VERIFICATION)

**Status:** Code fix exists with row+column staggering

**Potential Issues:**
- If timing is still off by even 1 cycle, weights will be wrong
- If PE registered pass-through delay is different than expected, capture will be wrong

#### 5. **Accumulator Write Address Incrementing** (NEEDS VERIFICATION)

**Status:** Code fix exists, but:
- If address incrementing is wrong, results overwrite each other
- If column selection (acc0+acc1 vs acc2) is wrong, data goes to wrong addresses

---

## What Needs to Happen

### Step 1: Re-synthesize FPGA
- All code fixes need to be synthesized into the FPGA
- Current `.bit` file is from commit `f5c1f2d` (before most fixes)

### Step 2: Verify Each Fix
After synthesis, test each fix individually:

1. **UB Write Burst:** Write known pattern, read back, verify data integrity
2. **Weight Capture:** Load weights, verify all 9 PEs have correct values
3. **Accumulator Write:** Run MATMUL, verify accumulator memory has correct values
4. **ST_UB:** Run MATMUL, then ST_UB, verify data matches accumulator memory
5. **Accumulator Read:** Test VPU operations (RELU, pooling) with combinational read

### Step 3: Debug Remaining Issues
If tests still fail:
- Check if ST_UB should read from accumulator memory instead of systolic outputs
- Verify accumulator memory actually contains correct values after MATMUL
- Check if accumulator read latency fix broke timing
- Verify all timing relationships are correct

---

## Known Issues That May Still Exist

### Issue #1: ST_UB Reads Wrong Source
**Suspected Problem:** ST_UB reads from systolic array outputs (which may be invalid after computation) instead of accumulator memory (which stores the results).

**Evidence:**
- Accumulator memory stores results: `acc_wr_data` → `accumulator.wr_data`
- ST_UB reads from: `acc0_out`/`acc1_out`/`acc2_out` (systolic array outputs)
- These are different sources!

**Potential Fix:**
```systemverilog
// Instead of reading from systolic outputs:
assign acc_data_out = {232'b0, acc2_latched[7:0], acc1_latched[7:0], acc0_latched[7:0]};

// Should read from accumulator memory:
// ST_UB should assert acc_rd_en and read from acc_rd_data
// Then pack acc_rd_data into acc_data_out
```

### Issue #2: Accumulator Memory May Not Be Written Correctly
**Suspected Problem:** Even if accumulator writes are happening, the data might be wrong due to:
- Wrong address incrementing
- Wrong column selection (acc0+acc1 vs acc2)
- Pipeline latency miscalculation
- Write timing issues

**Evidence:**
- Tests show ST_UB writes zeros or wrong values
- This could mean accumulator memory is empty/wrong, OR ST_UB reads wrong source

### Issue #3: UB Bank Selection May Still Be Wrong
**Suspected Problem:** Even with fixes, there might be:
- Timing issues with bank selection latching
- Address encoding mismatches
- Race conditions between UART and controller

---

## Action Items

### Immediate (Before Re-synthesis)
1. ✅ **Verify ST_UB data source** - Should it read from accumulator memory?
2. ✅ **Check accumulator memory write path** - Are writes actually happening?
3. ✅ **Review accumulator read latency fix** - Will it cause timing violations?

### After Re-synthesis
1. Run comprehensive diagnostic tests
2. Verify each fix individually
3. Debug any remaining failures
4. Fix additional bugs discovered

### Critical Questions to Answer
1. **Does accumulator memory contain correct values after MATMUL?**
   - Need to add debug path to read accumulator memory directly
   
2. **Should ST_UB read from accumulator memory or systolic outputs?**
   - Current design uses systolic outputs (latched)
   - But accumulator memory stores the "official" results
   
3. **Is the accumulator read latency fix safe?**
   - Combinational read might cause timing violations
   - May need to revert or add pipeline registers

---

## Summary

**What We Know:**
- ✅ 7 bugs have code fixes
- ❌ FPGA hasn't been re-synthesized (still running old code)
- ⚠️ Additional bugs may exist beyond the 7 reported
- ⚠️ Some fixes may cause new problems (e.g., combinational read)

**What We Don't Know:**
- ❓ Will the fixes work when synthesized?
- ❓ Are there additional bugs not yet identified?
- ❓ Is ST_UB reading from the correct source?
- ❓ Does accumulator memory contain correct values?

**What We Need:**
1. Re-synthesize FPGA with all fixes
2. Run comprehensive tests
3. Debug any remaining failures
4. Fix additional bugs as discovered

---

**Bottom Line:** The code fixes are a good start, but they're not magic. The FPGA needs re-synthesis, and there may be additional issues beyond the 7 bugs we've addressed. ST_UB reading from systolic outputs instead of accumulator memory is a particularly suspicious design choice that needs investigation.

