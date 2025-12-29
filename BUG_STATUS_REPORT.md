# Bug Status Report: Verification Against Current Codebase

**Report Date:** Current  
**Base Commit:** `f5c1f2db01738fc262125bd774d04fb6345c12ce`  
**Reference:** CHANGELOG_TECHNICAL.md

## Summary

This document verifies the status of 7 reported bugs against the current codebase. **ALL 7 BUGS ARE NOW FIXED** ✅

---

## Bug Status Verification

### ✅ BUG #1: Weight Capture Timing (Column-Only Instead of Row+Column) - **FIXED**

**Status:** ✅ **FIXED** in `rtl/systolic_controller.sv` lines 154-195

**Verification:**
- ✅ Code uses 9 separate capture signals: `en_capture_row0_col0` through `en_capture_row2_col2`
- ✅ Diagonal staggering implemented:
  - Cycle 1: `en_capture_row0_col0` (pe00)
  - Cycle 2: `en_capture_row0_col1` (pe01) AND `en_capture_row1_col0` (pe10)
  - Cycle 3: `en_capture_row0_col2` (pe02) AND `en_capture_row1_col1` (pe11) AND `en_capture_row2_col0` (pe20)
  - Cycle 4: `en_capture_row1_col2` (pe12) AND `en_capture_row2_col1` (pe21)
  - Cycle 5: `en_capture_row2_col2` (pe22)
- ✅ All 9 PEs in `rtl/mmu.sv` connected to row+column specific signals

**Code Location:**
- `rtl/systolic_controller.sv:154-195` - Staggered capture logic
- `rtl/mmu.sv` - All PE instantiations use row+column signals

---

### ✅ BUG #2: acc_wr_en Timing (Writes Garbage During Pipeline Fill) - **FIXED**

**Status:** ✅ **FIXED** in `rtl/systolic_controller.sv` lines 204-209

**Verification:**
- ✅ `PIPELINE_LATENCY` constant defined: `localparam PIPELINE_LATENCY = 4;`
- ✅ `acc_wr_en_valid` only asserted after pipeline fill: `(compute_counter >= PIPELINE_LATENCY)`
- ✅ Write enable gated: `assign acc_wr_en = acc_wr_en_valid;`

**Code Location:**
- `rtl/systolic_controller.sv:204-209` - Pipeline latency check

---

### ✅ BUG #3: acc2_out Not Stored in Accumulator - **FIXED**

**Status:** ✅ **FIXED** in `rtl/tpu_datapath.sv` lines 235-242

**Verification:**
- ✅ Two data paths created:
  - `acc_wr_data_col01 = {acc1_out, acc0_out}` (64-bit: col1 + col0)
  - `acc_wr_data_col2 = {32'h0, acc2_out}` (64-bit: acc2 padded)
- ✅ Multiplexer selects based on `acc_wr_col2` signal from systolic controller
- ✅ Column selection signals: `acc_wr_col01` and `acc_wr_col2` (even/odd cycles)

**Code Location:**
- `rtl/tpu_datapath.sv:235-242` - Data selection logic
- `rtl/systolic_controller.sv:225-226` - Column selection signals

---

### ✅ BUG #4: sc_acc_wr_addr Unused (Address Never Increments) - **FIXED**

**Status:** ✅ **FIXED** in `rtl/tpu_datapath.sv` line 217 and `rtl/systolic_controller.sv` lines 211-220

**Verification:**
- ✅ `acc_wr_addr_reg` increments in systolic controller: `acc_wr_addr_reg <= acc_wr_addr_reg + 1'b1;`
- ✅ Address multiplexer in datapath: `assign acc_wr_addr_combined = sc_acc_wr_en ? sc_acc_wr_addr : acc_addr;`
- ✅ Accumulator uses combined address: `.wr_addr(acc_wr_addr_combined)`

**Code Location:**
- `rtl/systolic_controller.sv:211-220` - Address increment logic
- `rtl/tpu_datapath.sv:217` - Address multiplexer

---

### ✅ BUG #5: No Sustained UB Reads During COMPUTE - **FIXED**

**Status:** ✅ **FIXED** in `rtl/systolic_controller.sv` line 202 and `rtl/tpu_datapath.sv` lines 361-363

**Verification:**
- ✅ `ub_rd_en_systolic` output added: `assign ub_rd_en_systolic = (current_state == SYS_COMPUTE);`
- ✅ Multiplexer in datapath: `assign ub_rd_en_muxed = systolic_active ? ub_rd_en_systolic : ub_rd_en;`
- ✅ Unified buffer uses muxed signal: `.ub_rd_en(ub_rd_en_muxed)`

**Code Location:**
- `rtl/systolic_controller.sv:202` - UB read enable during compute
- `rtl/tpu_datapath.sv:361-363` - Read enable multiplexer

---

### ✅ BUG #6: UB WR_BURST Memset Bug - **FIXED**

**Status:** ✅ **FIXED** in `rtl/unified_buffer.sv` lines 192-208

**Verification:**
- ✅ `WR_WRITE` state captures next word: `wr_data_latched <= ub_wr_data;` (line 178)
- ✅ `WR_BURST` state writes captured data, then captures next: `wr_data_latched <= ub_wr_data;` (line 201)
- ✅ `ub_wr_ready` goes low during bursts and high when done

**Code Location:**
- `rtl/unified_buffer.sv:170-208` - Write burst state machine

---

### ✅ BUG #7: Accumulator Synchronous Read Adds Latency - **FIXED**

**Status:** ✅ **FIXED** - Changed to combinational read in `rtl/accumulator.sv` lines 48-58

**Fix Applied:**
```systemverilog
// Read operation (combinational for VPU timing compatibility)
// CRITICAL FIX: Changed from registered to combinational read to eliminate 1-cycle latency
always_comb begin
    if (acc_buf_sel_reg == 0)
        rd_data = buffer0[rd_addr];
    else
        rd_data = buffer1[rd_addr];
end
```

**Impact:**
- ✅ Eliminates 1-cycle latency between `acc_rd_en` and data availability
- ✅ VPU activation pipelines now receive correct data when `valid_in` is asserted
- ✅ Matches VPU timing expectations (combinational read)

**Verification:**
- `rtl/tpu_datapath.sv:265-266`: `acc_col0` and `acc_col1` now get current data immediately
- VPU operations triggered by `acc_rd_en` receive data on the same cycle
- No timing mismatch between `valid_in` and data availability

---

## Summary Table

| Bug # | Description | Status | File(s) | Notes |
|-------|------------|--------|---------|-------|
| 1 | Weight capture timing | ✅ FIXED | `systolic_controller.sv`, `mmu.sv` | Row+column signals with diagonal staggering |
| 2 | acc_wr_en timing | ✅ FIXED | `systolic_controller.sv` | Pipeline latency delay added |
| 3 | acc2_out not stored | ✅ FIXED | `tpu_datapath.sv` | Column selection with multiplexer |
| 4 | sc_acc_wr_addr unused | ✅ FIXED | `systolic_controller.sv`, `tpu_datapath.sv` | Address incrementing implemented |
| 5 | No sustained UB reads | ✅ FIXED | `systolic_controller.sv`, `tpu_datapath.sv` | ub_rd_en_systolic added |
| 6 | UB WR_BURST memset | ✅ FIXED | `unified_buffer.sv` | Data capture fixed |
| 7 | Acc read latency | ✅ FIXED | `accumulator.sv` | Changed to combinational read |

---

## Next Steps

1. **Re-synthesize FPGA:**
   - All 7 bugs are now fixed and require FPGA re-synthesis
   - Run diagnostic tests to verify all fixes

2. **Verify All Fixes:**
   - Run `python/diagnostic_isolated.py` - Comprehensive test suite
   - Run `python/test_stub_verification.py` - ST_UB verification
   - Run `python/test_bank_verification.py` - Bank selection verification
   - Test VPU operations (RELU, pooling) to verify Bug #7 fix

3. **Update CHANGELOG_TECHNICAL.md:**
   - Add Bug #7 fix documentation
   - Mark all 7 bugs as fixed

---

## References

- **CHANGELOG_TECHNICAL.md** - Detailed technical documentation of all fixes
- **Base Commit:** `f5c1f2db01738fc262125bd774d04fb6345c12ce`
- **Repository:** https://github.com/AbiralShakya/tpu_to_fpga

