# CRITICAL FIX: ST_UB Write Failure - Mux Logic Corrected

## Problem Summary

**ST_UB instruction was COMPLETELY BLOCKED from writing to Unified Buffer.**

All diagnostic tests failed:
- ✗ TEST 1: Minimal ST_UB
- ✗ TEST 2: ST_UB overwrites marker
- ✗ TEST 3: Multiple ST_UB
- ✗ TEST 4: ST_UB after NOPs
- ✗ TEST 5: Accumulator data path
- ✗ TEST 6: Bank selection

**Root Cause:** `use_test_interface` mux logic in `tpu_top.sv` was including `test_instr_wr_en` in the mux control, which blocked ALL controller UB writes whenever ANY test signal was active.

---

## The Fix

### Modified Files

1. **rtl/tpu_top.sv** (lines 440-461)
   - Created separate mux control signal `use_test_interface_ub`
   - Only depends on `test_ub_wr_en | test_ub_rd_en` (NOT instruction/weight writes)
   - All UB mux assignments now use `use_test_interface_ub`
   - Added 5 new debug output ports for mux monitoring

2. **rtl/tpu_top_wrapper.sv** (lines 63-132)
   - Added internal wires for new debug signals with `keep` attributes
   - Connected debug signals to tpu_inst
   - Updated LED assignments to include debug signals (prevent optimization)

### Key Changes

**Before (BROKEN):**
```verilog
assign use_test_interface = test_ub_wr_en | test_ub_rd_en | test_wt_wr_en | test_instr_wr_en;
assign ub_wr_en = use_test_interface ? test_ub_wr_en : ctrl_ub_wr_en;
// ^^^ PROBLEM: test_instr_wr_en=1, test_ub_wr_en=0 → ub_wr_en = 0 (blocked!)
```

**After (FIXED):**
```verilog
// Original (still used for DMA busy detection)
assign use_test_interface = test_ub_wr_en | test_ub_rd_en | test_wt_wr_en | test_instr_wr_en;

// NEW: Separate mux for UB operations only
assign use_test_interface_ub = test_ub_wr_en | test_ub_rd_en;

// All UB muxes now use the new signal
assign ub_wr_en = use_test_interface_ub ? test_ub_wr_en : ctrl_ub_wr_en;
// ^^^ FIX: Instruction writes no longer block controller UB writes!
```

---

## New Debug Signals

### Added to tpu_top.sv

5 new debug outputs for real-time mux monitoring:

| Signal | Description |
|--------|-------------|
| `debug_use_test_interface` | Original mux control (includes all test signals) |
| `debug_ctrl_ub_wr_en` | Controller's UB write enable signal |
| `debug_test_ub_wr_en` | Test interface UB write enable |
| `debug_test_instr_wr_en` | Test interface instruction write enable |
| `debug_final_ub_wr_en` | Final muxed UB write enable (to unified_buffer) |

### LED Mapping (tpu_top_wrapper.sv)

Updated LED assignments to include debug signals:

```
LED[15:12] = debug_bank_state[7:4]     (bank state info)
LED[11]    = debug_use_test_interface  (original mux control)
LED[10]    = debug_ctrl_ub_wr_en       (controller wants to write)
LED[9]     = debug_test_ub_wr_en       (UART wants to write UB)
LED[8]     = debug_test_instr_wr_en    (UART writing instruction)
LED[7]     = debug_final_ub_wr_en      (final UB write enable)
LED[6:5]   = uart_debug_state[1:0]     (UART state machine)
LED[4]     = tpu_busy                  (TPU executing)
LED[3]     = tpu_done                  (TPU idle)
LED[2:0]   = (from tpu_led_internal)
```

**Critical observation during ST_UB execution:**
- LED[10] (`ctrl_ub_wr_en`) should be HIGH when ST_UB executes
- LED[7] (`debug_final_ub_wr_en`) should ALSO be HIGH (if mux works correctly)
- LED[8] (`test_instr_wr_en`) should be LOW during execution

---

## Expected Behavior After Fix

### During UART Programming Phase

1. Python sends WRITE_INSTR command
   - `test_instr_wr_en` = HIGH (1 cycle)
   - `use_test_interface` = HIGH
   - `use_test_interface_ub` = LOW (NEW!)
   - UB mux selects controller (no UART UB activity)

2. Python sends WRITE_UB command
   - `test_ub_wr_en` = HIGH
   - `use_test_interface` = HIGH
   - `use_test_interface_ub` = HIGH
   - UB mux selects test interface (correct)

### During ISA Execution Phase

3. ST_UB instruction executes
   - `ctrl_ub_wr_en` = HIGH (from controller)
   - `test_instr_wr_en` = LOW (UART idle)
   - `test_ub_wr_en` = LOW (UART idle)
   - `use_test_interface_ub` = LOW (NEW!)
   - `ub_wr_en` = `ctrl_ub_wr_en` = HIGH ✓
   - **UB WRITE SUCCEEDS!**

---

## Verification Steps

### 1. Rebuild Bitstream

```bash
# On Adroit (after uploading updated RTL)
cd ~/tpu_build
vivado -mode batch -source build_with_vivado_synthesis.tcl
```

### 2. Program FPGA

```bash
# Download bitstream from Adroit
scp as0714@adroit.princeton.edu:~/tpu_build/tpu_basys3_vivado_synth.bit ./build/

# Program Basys3
openFPGALoader -b basys3 ./build/tpu_basys3_vivado_synth.bit
```

### 3. Run Comprehensive Tests

```bash
python3 python/ub_comprehensive_test.py /dev/tty.usbserial-210183A27BE01
```

**Expected Results:**
- ✓ TEST 0: UART baseline (already passing)
- ✓ TEST 1: Minimal ST_UB (should now PASS)
- ✓ TEST 2: ST_UB overwrites marker (should now PASS)
- ✓ TEST 3: Multiple ST_UB (should now PASS)
- ✓ TEST 4: ST_UB after NOPs (should now PASS)
- ✓ TEST 5: Accumulator data path (should now PASS)
- ✓ TEST 6: Bank selection (should now PASS)

### 4. Run Original Diagnostic

```bash
python3 python/diagnostic_isolated.py /dev/tty.usbserial-210183A27BE01
```

**Expected:**
- ✓ TEST 2: ST_UB writes to UB (should show changed value)
- ✓ TEST 6: Bank selection correct

### 5. Observe LEDs During Execution

Watch LED[10] (`ctrl_ub_wr_en`) and LED[7] (`debug_final_ub_wr_en`):
- Both should pulse HIGH briefly when ST_UB executes
- LED[8] (`test_instr_wr_en`) should be LOW during execution
- This confirms the mux is working correctly

---

## Technical Details

### Why This Fix Works

**Problem:** The original mux logic created a dependency between instruction writes and UB writes:

```
test_instr_wr_en=1 → use_test_interface=1 → ub_wr_en = test_ub_wr_en (=0)
```

Even though the controller asserted `ctrl_ub_wr_en=1`, the mux selected `test_ub_wr_en=0` instead.

**Solution:** Decouple UB operations from instruction/weight operations:

```verilog
// UB operations have their own mux control
use_test_interface_ub = test_ub_wr_en | test_ub_rd_en

// Now instruction writes don't affect UB mux
test_instr_wr_en=1, test_ub_wr_en=0 → use_test_interface_ub=0 → ub_wr_en = ctrl_ub_wr_en ✓
```

### Why Other Operations Still Need Original Mux

The original `use_test_interface` signal is still used for:
1. `dma_busy_out` logic (tpu_top.sv:459)
2. General test interface activity detection

This ensures backward compatibility while fixing the UB write issue.

---

## Files Modified

### RTL Changes

1. `/Users/abiralshakya/Documents/tpu_to_fpga/rtl/tpu_top.sv`
   - Added 5 debug output ports (lines 48-53)
   - Created `use_test_interface_ub` signal (line 443)
   - Updated all UB mux assignments to use new signal (lines 455-461)
   - Added debug signal assignments (lines 524-530)

2. `/Users/abiralshakya/Documents/tpu_to_fpga/rtl/tpu_top_wrapper.sv`
   - Added 5 internal debug wires with `keep` attributes (lines 64-68)
   - Connected debug signals to tpu_inst (lines 107-111)
   - Updated LED assignment with debug signals (lines 121-132)

### Test Suite

1. `/Users/abiralshakya/Documents/tpu_to_fpga/python/ub_comprehensive_test.py`
   - Created comprehensive UB diagnostic suite (NEW FILE)
   - 7 independent tests for systematic failure isolation

### Documentation

1. `/Users/abiralshakya/Documents/tpu_to_fpga/UB_WRITE_FAILURE_ANALYSIS.md`
   - Complete root cause analysis
   - Signal path tracing
   - Hypothesis ranking

2. `/Users/abiralshakya/Documents/tpu_to_fpga/CRITICAL_FIX_ST_UB_WRITE.md`
   - This document (fix summary and verification steps)

---

## What to Expect

### If Fix Is Successful

All 7 comprehensive tests should pass:
- UB values will change when ST_UB executes
- acc_data_out will be written to UB
- Even if accumulators are zero, zeros should overwrite existing markers
- Bank selection will be consistent (both UART and ISA use bank 0)

### If Still Failing

Check LED patterns:
- If LED[10]=1 but LED[7]=0 during execution: Mux still broken (logic error)
- If LED[10]=0 during ST_UB: Controller not asserting `ctrl_ub_wr_en` (pipeline issue)
- If LED[8]=1 during execution: Test interface signal not clearing (timing issue)

---

## Next Steps After Verification

Once ST_UB writes successfully:

1. **Investigate TEST 7 (all zero accumulators)**
   - Check if `systolic_active` signal triggers
   - Verify accumulator latching during MATMUL
   - This is a separate issue from UB writes

2. **Run full TPU test suite**
   - MATMUL with actual computation
   - Multi-layer inference tests
   - Performance benchmarks

3. **Document the fix**
   - Update architecture documentation
   - Add comments explaining mux separation
   - Create test case for regression testing

---

## Author Notes

This fix was identified through systematic architectural analysis:
1. Signal path tracing from controller → mux → UB
2. Comprehensive test suite to isolate failure point
3. Mux logic analysis revealing instruction write dependency
4. Debug signal insertion for real-time monitoring
5. Surgical fix to separate UB operations from other test operations

**Critical lesson:** Shared mux control signals can create unexpected dependencies. Always use the most specific control signal possible.
