# Unified Buffer Write Failure - Root Cause Analysis

## Problem Statement

**ST_UB instruction does not write to the Unified Buffer during ISA execution.**

### Evidence
1. ✓ UART can write/read UB successfully (TEST 1 passes)
2. ✗ ST_UB does NOT modify UB (TEST 2 fails - value remains unchanged)
3. ✗ Bank selection issue (TEST 6 fails)
4. ✗ All accumulator values are zero (TEST 7 shows all zeros)

---

## Signal Path Analysis

### ST_UB Write Path (from controller to UB)

```
tpu_controller.sv (ST_UB_OP case):
    ctrl_ub_wr_en = 1'b1
    ctrl_ub_wr_addr = {exec_ub_buf_sel, exec_arg1}
    ctrl_ub_wr_data = (from tpu_datapath) acc_data_out
           ↓
tpu_top.sv (Mux logic):
    use_test_interface = test_ub_wr_en | test_ub_rd_en | test_wt_wr_en | test_instr_wr_en
    ub_wr_en = use_test_interface ? test_ub_wr_en : ctrl_ub_wr_en  ← KEY MUX
    ub_wr_addr = use_test_interface ? {1'b0, test_ub_wr_addr[7:0]} : ctrl_ub_wr_addr
    ub_wr_data = use_test_interface ? test_ub_wr_data : acc_data_out
           ↓
unified_buffer.sv (Write logic):
    if (ub_wr_en) begin
        memory_bank[ub_wr_addr] <= ub_wr_data
    end
```

### UART Write Path (for comparison)

```
uart_dma_basys3.sv:
    uart_instr_wr_en = 1 (pulses for 1 cycle when writing instruction)
           ↓
basys3_test_interface.sv:
    uart_active = uart_ub_wr_en | uart_ub_rd_en | uart_wt_wr_en | uart_instr_wr_en | uart_start_execution
    test_instr_wr_en = uart_active ? uart_instr_wr_en : phys_instr_wr_en
           ↓
tpu_top.sv:
    use_test_interface = test_ub_wr_en | test_ub_rd_en | test_wt_wr_en | test_instr_wr_en ← INCLUDES INSTR!
    ub_wr_en = use_test_interface ? test_ub_wr_en : ctrl_ub_wr_en
```

---

## Critical Mux Logic Issue

The `use_test_interface` signal in `tpu_top.sv:434` is defined as:

```verilog
assign use_test_interface = test_ub_wr_en | test_ub_rd_en | test_wt_wr_en | test_instr_wr_en;
```

**When ANY of these signals is HIGH, ALL UB operations are routed through test interface!**

### The Problem

If `test_instr_wr_en` is HIGH (even if `test_ub_wr_en` is LOW):
- `use_test_interface = 1`
- `ub_wr_en = test_ub_wr_en` = **0** (not `ctrl_ub_wr_en`!)
- **Controller writes are BLOCKED!**

### Timing Analysis

**Expected behavior:**
1. Python sends WRITE_INSTR → `uart_instr_wr_en` pulses HIGH for 1 cycle
2. `test_instr_wr_en` goes HIGH for 1 cycle
3. `use_test_interface` goes HIGH for 1 cycle
4. Next cycle: all signals return to LOW
5. Controller executes → `ctrl_ub_wr_en` should route through

**Potential failure modes:**
1. `uart_instr_wr_en` doesn't clear properly (stays HIGH)
2. `uart_active` stays HIGH due to lingering signal
3. `test_instr_wr_en` stays HIGH after instruction write
4. Race condition during `start_execution` pulse

---

## Bank Selection Analysis

### Bank 0 vs Bank 1

The UB has 2 banks selected by address bit 8:
- `ub_wr_addr[8] = 0` → Bank 0
- `ub_wr_addr[8] = 1` → Bank 1

### UART Always Uses Bank 0

From `tpu_top.sv:441`:
```verilog
assign ub_wr_addr = use_test_interface ? {1'b0, test_ub_wr_addr[7:0]} : ctrl_ub_wr_addr;
//                                         ^^^^^ ALWAYS 0 = Bank 0
```

### Controller Bank Selection

From `tpu_controller.sv:580`:
```verilog
ST_UB_OP: begin
    ub_wr_addr = {exec_ub_buf_sel, exec_arg1};  // Bank depends on exec_ub_buf_sel
end
```

### Initialization State

From `tpu_controller.sv:302-308`:
```verilog
else if (start_execution) begin
    ub_buf_sel_reg  <= 1'b0;  // Reset to Bank 0
```

**Conclusion:** Both UART and ST_UB should use Bank 0 initially. **Bank mismatch is unlikely.**

---

## Accumulator Latching Analysis

### acc_data_out Construction

From `tpu_datapath.sv`:
```verilog
always_ff @(posedge clk) begin
    if (systolic_active) begin
        acc0_latched <= acc0_out;
        acc1_latched <= acc1_out;
        acc2_latched <= acc2_out;
    end
end

assign acc_data_out = {232'b0, acc2_latched[7:0], acc1_latched[7:0], acc0_latched[7:0]};
```

### Critical Dependency

**Accumulators are only latched when `systolic_active` is HIGH.**

From `systolic_controller.sv`:
```verilog
assign systolic_active = (current_state == SYS_COMPUTE);
```

### TEST 7 Results: All Zeros

```
After RD_WEIGHT x3:  0000000000000000 → [0, 0, 0]
After LD_UB:         0000000000000000 → [0, 0, 0]
After MATMUL:        0000000000000000 → [0, 0, 0]
```

**This indicates:**
1. Either `systolic_active` never goes HIGH
2. OR accumulators never compute anything
3. OR the read path for `acc_data_out` is broken

### If acc_data_out is All Zeros

Even if ST_UB writes successfully, writing all zeros to a location that already has a marker pattern like `0xDEADBEEF` should change it to `0x000000000000`. But TEST 2 shows the value **unchanged**, meaning **ST_UB is not writing at all**.

---

## Hypotheses Ranked by Likelihood

### Hypothesis 1: `use_test_interface` Mux Blocking Writes ⭐⭐⭐⭐⭐

**Description:** A test interface signal remains HIGH during execution, causing `use_test_interface = 1`, which blocks `ctrl_ub_wr_en`.

**Evidence:**
- Mux explicitly checks `test_instr_wr_en`
- If `test_instr_wr_en` stays HIGH, all controller writes blocked
- TEST 2/3/5/6 all show UB unchanged

**How to verify:**
- Add debug output to monitor `use_test_interface` signal
- Check if any test signal is stuck HIGH
- Use comprehensive test suite to check timing

**Potential fix:**
- Ensure all test signals (especially `test_instr_wr_en`) clear properly
- Modify mux logic to exclude `test_instr_wr_en` from `use_test_interface`
- Add synchronization/edge detection

### Hypothesis 2: `start_execution` Timing Issue ⭐⭐⭐⭐

**Description:** `uart_start_execution` pulses HIGH, making `uart_active` HIGH, which propagates through the mux hierarchy and potentially interferes with execution.

**Evidence:**
- `uart_active` includes `uart_start_execution` (basys3_test_interface.sv:445)
- Timing between `start_execution` pulse and first instruction execution unclear

**How to verify:**
- Check if `uart_active` clears before first instruction reaches exec stage
- Add delay between instruction writes and execution command

**Potential fix:**
- Ensure `start_execution` is a true 1-cycle pulse
- Add state machine to guarantee clean transition

### Hypothesis 3: Pipeline Stall/Flush Preventing ST_UB ⭐⭐⭐

**Description:** ST_UB instruction never reaches exec stage due to stalls or flushes.

**Evidence:**
- Recent pipeline fixes for stall handling
- Programs execute to completion (HALT works), suggesting pipeline advances

**Counterevidence:**
- HALT instruction executes successfully
- Program doesn't timeout

**How to verify:**
- Check if `exec_valid` is HIGH when ST_UB is in exec stage
- Monitor PC advancement

### Hypothesis 4: WRITE_INSTR Protocol Corruption ⭐⭐

**Description:** Python script sends 5-byte header but UART expects 3 bytes, corrupting instructions.

**Evidence:**
- Python: `send_cmd(WRITE_INSTR, 0, addr, 0, 4)` → sends 5 bytes + 4 bytes
- UART: READ_ADDR_LO → WRITE_INSTR (expects only 4 instruction bytes)

**Counterevidence:**
- User reverted my fix to Python script, yet tests still run
- HALT instruction works (would be corrupted if protocol wrong)

**Status:** Likely NOT the issue, or corruption is consistent

---

## Recommended Actions

### 1. Run Comprehensive Test Suite (IMMEDIATE)

```bash
python3 python/ub_comprehensive_test.py /dev/tty.usbserial-210183A27BE01
```

This will systematically test:
- UART baseline
- Minimal ST_UB
- ST_UB overwriting markers
- Multiple ST_UB sequences
- ST_UB after NOPs
- Accumulator data path
- Bank selection

### 2. Add Debug Signals (HIGH PRIORITY)

Add these signals to `tpu_top.sv` for observation:

```verilog
// Debug outputs
output logic debug_use_test_interface,
output logic debug_ctrl_ub_wr_en,
output logic debug_test_instr_wr_en,
output logic debug_test_ub_wr_en,
output logic [7:0] debug_state

// Assignments
assign debug_use_test_interface = use_test_interface;
assign debug_ctrl_ub_wr_en = ctrl_ub_wr_en;
assign debug_test_instr_wr_en = test_instr_wr_en;
assign debug_test_ub_wr_en = test_ub_wr_en;
```

Route to LEDs for visual debugging.

### 3. Fix Mux Logic (IF HYPOTHESIS 1 CONFIRMED)

**Option A:** Exclude `test_instr_wr_en` from UB mux

```verilog
// Old (problematic):
assign use_test_interface = test_ub_wr_en | test_ub_rd_en | test_wt_wr_en | test_instr_wr_en;

// New (fixed):
assign use_test_interface_ub = test_ub_wr_en | test_ub_rd_en;
assign ub_wr_en = use_test_interface_ub ? test_ub_wr_en : ctrl_ub_wr_en;
```

**Option B:** Add synchronization to ensure clean state before execution

```verilog
logic start_execution_prev;
logic execution_started;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        execution_started <= 1'b0;
        start_execution_prev <= 1'b0;
    end else begin
        start_execution_prev <= start_execution;
        if (start_execution && !start_execution_prev) begin
            execution_started <= 1'b1;  // Execution has started
        end
        if (ctrl_halt) begin
            execution_started <= 1'b0;  // Reset on HALT
        end
    end
end

// Only allow test interface when NOT executing
assign use_test_interface = !execution_started && (test_ub_wr_en | test_ub_rd_en | test_wt_wr_en | test_instr_wr_en);
```

### 4. Verify Accumulator Path (MEDIUM PRIORITY)

Check if `systolic_active` ever goes HIGH:
- Add debug output for `systolic_active`
- Monitor accumulator latching
- Verify MATMUL instruction triggers systolic array

---

## Next Steps

1. ✅ Run `python/ub_comprehensive_test.py` to gather more data
2. ⬜ Analyze test results to confirm/reject hypotheses
3. ⬜ Add debug signals to monitor `use_test_interface` timing
4. ⬜ If Hypothesis 1 confirmed, fix mux logic
5. ⬜ Rebuild bitstream and retest
6. ⬜ If still failing, investigate accumulator latching path

---

## Files Modified for Analysis

- `/Users/abiralshakya/Documents/tpu_to_fpga/python/ub_comprehensive_test.py` - New comprehensive test suite
- `/Users/abiralshakya/Documents/tpu_to_fpga/UB_WRITE_FAILURE_ANALYSIS.md` - This document

## Files to Modify for Fix

- `/Users/abiralshakya/Documents/tpu_to_fpga/rtl/tpu_top.sv` - Mux logic (lines 434-444)
- `/Users/abiralshakya/Documents/tpu_to_fpga/rtl/basys3_test_interface.sv` - uart_active signal (line 445)
