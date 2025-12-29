# Quick Rebuild and Test Guide

## What Was Fixed

**Root Cause:** `use_test_interface` mux in `tpu_top.sv` included `test_instr_wr_en`, which blocked ALL controller UB writes whenever instructions were being written.

**Fix:** Created separate `use_test_interface_ub` signal that ONLY depends on UB read/write enables, allowing controller to write to UB even when other test operations are active.

**Files Modified:**
- `rtl/tpu_top.sv` - Fixed mux logic + added 5 debug signals
- `rtl/tpu_top_wrapper.sv` - Connected debug signals to LEDs

---

## Rebuild Steps

### Option 1: Local Build with Vivado (if you have Vivado installed)

```bash
cd ~/Documents/tpu_to_fpga

# Create build directory
mkdir -p ~/tpu_build/rtl
cp rtl/*.sv ~/tpu_build/rtl/
cp constraints/basys3_vivado.xdc ~/tpu_build/

# Run Vivado synthesis
cd ~/tpu_build
vivado -mode batch -source ~/Documents/tpu_to_fpga/vivado/build_with_vivado_synthesis.tcl
```

### Option 2: Build on Adroit (Recommended)

```bash
# 1. Upload updated RTL to Adroit
cd ~/Documents/tpu_to_fpga
./scripts/upload_to_adroit.sh

# 2. SSH to Adroit
ssh -X as0714@adroit.princeton.edu

# 3. Extract and setup
cd /scratch/network/as0714/tpu_to_fpga
tar -xzf tpu_vivado_project.tar.gz

mkdir -p ~/tpu_build/rtl
cp rtl/*.sv ~/tpu_build/rtl/
cp constraints/basys3_vivado.xdc ~/tpu_build/

# 4. Run Vivado synthesis
cd ~/tpu_build
module load vivado/2023.1
vivado -mode batch -source ~/Documents/tpu_to_fpga/vivado/build_with_vivado_synthesis.tcl

# 5. Download bitstream (from local machine)
scp as0714@adroit.princeton.edu:~/tpu_build/tpu_basys3_vivado_synth.bit ~/Documents/tpu_to_fpga/build/
```

---

## Program FPGA

```bash
cd ~/Documents/tpu_to_fpga

# Program Basys3 with new bitstream
openFPGALoader -b basys3 build/tpu_basys3_vivado_synth.bit
```

---

## Test the Fix

### Run Comprehensive Test Suite

```bash
python3 python/ub_comprehensive_test.py /dev/tty.usbserial-210183A27BE01
```

**Expected Results (ALL SHOULD PASS NOW):**
```
======================================================================
DIAGNOSTIC SUMMARY
======================================================================
  uart_baseline            : ✓ PASS
  minimal_st_ub            : ✓ PASS  ← Should change from FAIL to PASS
  st_ub_marker             : ✓ PASS  ← Should change from FAIL to PASS
  multiple_st_ub           : ✓ PASS  ← Should change from FAIL to PASS
  st_ub_after_nop          : ✓ PASS  ← Should change from FAIL to PASS
  acc_data_out_path        : ✓ PASS  ← Should change from FAIL to PASS
  bank_selection           : ✓ PASS  ← Should change from FAIL to PASS
```

### Run Original Diagnostic

```bash
python3 python/diagnostic_isolated.py /dev/tty.usbserial-210183A27BE01
```

**Watch for:**
- TEST 2: ST_UB should now write (value should change)
- TEST 6: Bank selection should work

---

## LED Debug Signals (During Execution)

Watch the Basys3 LEDs when running tests:

```
LED[15:12] = Bank state info
LED[11]    = use_test_interface (original - includes all test signals)
LED[10]    = ctrl_ub_wr_en (controller wants to write) ← SHOULD PULSE DURING ST_UB
LED[9]     = test_ub_wr_en (UART wants to write UB)
LED[8]     = test_instr_wr_en (UART writing instruction) ← Should be LOW during execution
LED[7]     = final ub_wr_en (actual write to UB) ← SHOULD PULSE DURING ST_UB
LED[6:5]   = UART state
LED[4]     = TPU busy
LED[3]     = TPU done
```

**Critical Check:**
- During ST_UB execution, LED[10] AND LED[7] should both pulse HIGH briefly
- If LED[10]=HIGH but LED[7]=LOW, the mux is still broken
- If LED[8]=HIGH during execution, test interface isn't clearing

---

## If Tests Still Fail

### Scenario 1: Tests still fail but LEDs show activity

If LED[10] pulses during ST_UB but UB still doesn't change:
- Check accumulator path (`acc_data_out`)
- Verify systolic_active signal triggers
- This would be a DIFFERENT bug (not the mux)

### Scenario 2: LED[10] never pulses

If LED[10] stays LOW during ST_UB execution:
- Controller isn't asserting `ctrl_ub_wr_en`
- Check pipeline advancement (exec stage)
- Check if ST_UB opcode decodes correctly

### Scenario 3: LED[8] stays HIGH during execution

If LED[8] (`test_instr_wr_en`) is HIGH during execution:
- Test interface signal isn't clearing
- Check `uart_dma_basys3.sv` state machine
- Check `basys3_test_interface.sv` multiplexing

---

## Next Steps After Successful Fix

Once ST_UB writes successfully:

1. **Investigate accumulator values (TEST 7)**
   - If accumulators are all zeros, check MATMUL execution
   - Verify `systolic_active` signal
   - Check accumulator latching in `tpu_datapath.sv`

2. **Run full test suite**
   ```bash
   python3 python/test_stub_verification.py /dev/tty.usbserial-210183A27BE01
   python3 python/simple_matmul_test.py /dev/tty.usbserial-210183A27BE01
   ```

3. **Profile performance**
   - Measure MATMUL latency
   - Check throughput
   - Verify timing constraints met

---

## Quick Command Reference

```bash
# Upload to Adroit
./scripts/upload_to_adroit.sh

# Build on Adroit (after SSH)
cd ~/tpu_build && module load vivado/2023.1 && vivado -mode batch -source build_with_vivado_synthesis.tcl

# Download bitstream
scp as0714@adroit.princeton.edu:~/tpu_build/tpu_basys3_vivado_synth.bit ~/Documents/tpu_to_fpga/build/

# Program FPGA
openFPGALoader -b basys3 build/tpu_basys3_vivado_synth.bit

# Test
python3 python/ub_comprehensive_test.py /dev/tty.usbserial-210183A27BE01
python3 python/diagnostic_isolated.py /dev/tty.usbserial-210183A27BE01
```

---

## Summary

**The fix is surgical and minimal:**
- Created `use_test_interface_ub` signal
- Updated UB mux assignments to use new signal
- Added debug outputs for monitoring
- No changes to controller, datapath, or unified buffer logic

**Confidence level: VERY HIGH**
- Root cause clearly identified through comprehensive testing
- Fix directly addresses the mux blocking issue
- Debug signals will confirm if fix works in real-time

If this doesn't work, the comprehensive test results + LED patterns will immediately reveal the next issue to investigate.
