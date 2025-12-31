# Controller Refactoring Review & Integration Plan

## Overview

The proposed refactoring moves from a pipeline-based architecture to an explicit state machine. This is a good direction, but several critical issues need to be addressed for proper integration.

---

## ✅ **Good Changes**

1. **Explicit State Machine**: Much clearer than implicit pipeline states
2. **Hard-coded ST_UB Sequence**: Prevents zombie mode (critical fix)
3. **Simplified Logic**: Removes complex pipeline hazard detection
4. **Micro-counter**: Good for multi-cycle operations
5. **Removed `timescale`**: Modern practice

---

## ❌ **Critical Issues to Fix**

### 1. **Missing `stored_acc_addr_reg` for ST_UB**

**Problem**: ST_UB needs to know where MATMUL wrote results. The proposed code uses `arg2` directly, but ST_UB doesn't have `arg2` - it uses `exec_arg1` (UB write address) and `exec_arg2` (count).

**Current Implementation** (lines 172-175, 359-363):
```systemverilog
logic [7:0] stored_acc_addr_reg;  // Accumulator base address from MATMUL
logic      stored_acc_buf_sel_reg; // Accumulator buffer selection from MATMUL

// In MATMUL execution:
stored_acc_addr_reg <= exec_arg2;  // Store accumulator address
stored_acc_buf_sel_reg <= exec_acc_buf_sel;
```

**Fix Needed**:
```systemverilog
// In S_EXEC_MATMUL state:
logic [7:0] stored_acc_addr_reg;
logic      stored_acc_buf_sel_reg;

// Store accumulator address when MATMUL starts
if (micro_cnt == 0) begin
    stored_acc_addr_reg <= arg2;  // sys_acc_addr
    stored_acc_buf_sel_reg <= cur_acc_buf_sel;
end

// In S_EXEC_ST_UB:
acc_addr = stored_acc_addr_reg;  // Use stored address, not arg2
acc_buf_sel = stored_acc_buf_sel_reg;
```

---

### 2. **ST_UB Sequence Timing Mismatch**

**Problem**: The proposed sequence (0-5) doesn't match datapath expectations.

**Datapath Expectation** (from `tpu_datapath.sv` lines 316-330, 548-572):
- `acc_rd_en` asserted → triggers `st_ub_passthrough` latch
- Pipeline latency: 2 cycles (activation + quantization in passthrough mode)
- Capture bytes when `st_ub_col_idx` is 0, 1, 2 (datapath captures from activation pipelines)
- Write when `st_ub_col_idx` is 3 (clears passthrough latch)

**Proposed Sequence**:
```
micro_cnt 0: acc_rd_en = 1
micro_cnt 1: Wait BRAM latency
micro_cnt 2: st_ub_col_idx = 0
micro_cnt 3: st_ub_col_idx = 1
micro_cnt 4: st_ub_col_idx = 2
micro_cnt 5: st_ub_col_idx = 3, ub_wr_en = 1
```

**Issue**: The datapath captures bytes from activation pipeline outputs (`ap_data_col0/1/2`) when `ap_valid_col*` is high AND `st_ub_col_idx == 3`. The proposed sequence sets `st_ub_col_idx` to 0, 1, 2 before the pipeline outputs are valid.

**Correct Sequence** (based on datapath implementation):
```systemverilog
S_EXEC_ST_UB: begin
    case (micro_cnt)
        8'd0: begin
            // Step 1: Request Read (triggers passthrough latch in datapath)
            // Datapath: st_ub_passthrough_reg latches when acc_rd_en asserted
            acc_rd_en = 1'b1;
            acc_addr = stored_acc_addr_reg;  // From MATMUL
            acc_buf_sel = stored_acc_buf_sel_reg;
        end
        8'd1, 8'd2: begin
            // Step 2-3: Wait for pipeline latency
            // BRAM: 1 cycle (acc_rd_data valid on cycle 1)
            // Activation pipeline: 2 cycles in passthrough mode
            // Total: 3 cycles from acc_rd_en to ap_valid_col*
        end
        8'd3: begin
            // Step 4: Set col_idx=3 to trigger capture AND write
            // Datapath captures bytes when: ap_valid_col* && st_ub_col_idx == 3
            // This happens simultaneously for all 3 columns
            st_ub_col_idx = 2'd3;
            ub_wr_en = 1'b1;
            ub_wr_addr = {1'b0, exec_arg1};  // UB write address
            ub_wr_count = 9'h001;
        end
        8'd4: begin
            // Step 5: Complete - passthrough latch cleared when col_idx=3
            pc_inc = 1'b1;
            next_state = S_FETCH;
        end
    endcase
end
```

**Note**: The datapath's passthrough latch is cleared when `st_ub_col_idx == 3` (line 330 in tpu_datapath.sv), so the sequence must reach this state to prevent zombie mode.

---

### 3. **LD_UB Should Wait for `ub_rd_valid`**

**Problem**: The proposed code waits 2 cycles, but should wait for the actual `ub_rd_valid` signal to ensure data is latched.

**Current Datapath** (lines 452-456):
```systemverilog
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        row0_act_latched <= 8'h00;
        // ...
    end else if (ub_rd_valid) begin  // CRITICAL: Latch on ub_rd_valid
        row0_act_latched <= ub_rd_data[7:0];
        // ...
    end
end
```

**Fix Needed**:
```systemverilog
S_EXEC_LD_UB: begin
    ub_rd_en = 1'b1;
    ub_rd_addr = {cur_ub_bank_sel, arg1};
    ub_rd_count = 9'h001;
    
    // Wait for ub_rd_valid (data latched in datapath)
    if (ub_rd_valid || micro_cnt >= 3) begin  // Timeout after 3 cycles
        pc_inc = 1'b1;
        next_state = S_FETCH;
    end
end
```

**Note**: Need to add `ub_rd_valid` as input to controller.

---

### 4. **MATMUL Doesn't Enforce LD_UB Sequence**

**Problem**: The architecture document requires LD_UB → MATMUL sequence, but the proposed code doesn't enforce it.

**Fix Needed**:
```systemverilog
// Add flag to track LD_UB completion
logic ld_ub_completed;

// In S_EXEC_LD_UB:
if (ub_rd_valid) begin
    ld_ub_completed <= 1'b1;
    pc_inc = 1'b1;
    next_state = S_FETCH;
end

// In S_DECODE for MATMUL:
6'h10, 6'h11, 6'h12: begin
    if (!ld_ub_completed) begin
        // Force LD_UB first
        next_state = S_EXEC_LD_UB;
        // Store MATMUL instruction to re-execute after LD_UB
        // OR: Just proceed and let LD_UB happen first in program order
    end else begin
        next_state = S_EXEC_MATMUL;
    end
end

// In S_EXEC_MATMUL when done:
if (sys_done && !sys_busy) begin
    ld_ub_completed <= 1'b0;  // Reset for next MATMUL
    pc_inc = 1'b1;
    next_state = S_FETCH;
end
```

**Alternative**: If instructions are always in correct order (LD_UB before MATMUL), we can skip this check.

---

### 5. **Missing Instruction Field Storage**

**Problem**: The proposed code decodes `opcode`, `arg1`, `arg2`, `arg3`, `flags` from `ir` in S_DECODE, but these values are lost when transitioning to execution states.

**Fix Needed**:
```systemverilog
// Store decoded fields
logic [5:0] exec_opcode;
logic [7:0] exec_arg1, exec_arg2, exec_arg3;
logic [1:0] exec_flags;

// In S_DECODE:
exec_opcode <= opcode;
exec_arg1 <= arg1;
exec_arg2 <= arg2;
exec_arg3 <= arg3;
exec_flags <= flags;

// In execution states, use exec_* instead of opcode/arg*
```

---

### 6. **Missing CFG_REG Instruction**

**Problem**: The proposed code doesn't handle `6'h31: CFG_REG`.

**Fix Needed**:
```systemverilog
// Add configuration registers
logic [15:0] cfg_registers [0:255];

// In S_DECODE:
6'h31: next_state = S_EXEC_CFG_REG;

// Add new state:
S_EXEC_CFG_REG: begin
    cfg_registers[exec_arg1] <= {exec_arg2, exec_arg3};
    pc_inc = 1'b1;
    next_state = S_FETCH;
end
```

---

### 7. **`clear_complete` Signal**

**Problem**: The proposed `tpu_top.sv` ties `clear_complete` to `1'b1`, but the comment suggests it should come from datapath.

**Current Architecture**: The `systolic_controller` handles accumulator clear internally. The main controller just needs to wait for `sys_busy` to go low.

**Fix**: The proposed approach is correct - `clear_complete` isn't needed as a separate signal. The controller can use `sys_busy` to know when clear is complete.

**However**: If we want explicit clear completion, we need to export it from datapath:
```systemverilog
// In tpu_datapath.sv, add output:
output logic acc_clear_complete,  // Export accumulator clear completion

// In tpu_top.sv:
.acc_clear_complete(acc_clear_complete),  // From datapath
```

---

### 8. **ST_UB Instruction Arguments**

**Problem**: The proposed code uses `arg2` for accumulator address, but ST_UB instruction format is:
- `arg1`: UB write address
- `arg2`: Byte count (should be 3 for 3x3)
- Accumulator address comes from `stored_acc_addr_reg` (from MATMUL)

**Fix**: Already addressed in issue #1, but ensure `arg1` is used for UB write address.

---

### 9. **VPU Mode Mapping**

**Problem**: The proposed code uses `opcode[3:0]` for VPU mode, but opcodes are:
- `6'h18`: RELU → VPU mode 1
- `6'h19`: RELU6 → VPU mode 2
- `6'h1A`: SIGMOID → VPU mode 3
- `6'h1B`: TANH → VPU mode 4
- `6'h20`: MAXPOOL → VPU mode 6
- `6'h21`: AVGPOOL → VPU mode 7
- `6'h22`: ADD_BIAS → VPU mode 5
- `6'h23`: BATCH_NORM → VPU mode 8

**Fix Needed**:
```systemverilog
S_EXEC_VPU: begin
    vpu_start = 1'b1;
    case (exec_opcode)
        6'h18: vpu_mode = 4'h1;  // RELU
        6'h19: vpu_mode = 4'h2;  // RELU6
        6'h1A: vpu_mode = 4'h3;  // SIGMOID
        6'h1B: vpu_mode = 4'h4;  // TANH
        6'h20: vpu_mode = 4'h6;  // MAXPOOL
        6'h21: vpu_mode = 4'h7;  // AVGPOOL
        6'h22: vpu_mode = 4'h5;  // ADD_BIAS
        6'h23: vpu_mode = 4'h8;  // BATCH_NORM
        default: vpu_mode = 4'h0;
    endcase
    // ... rest of VPU logic
end
```

---

### 10. **SYNC Buffer Toggle Timing**

**Problem**: The proposed code toggles buffers when `micro_cnt == 0`, but should ensure buffers are idle first.

**Fix Needed**:
```systemverilog
S_SYNC: begin
    // Only toggle if buffers are idle
    if (micro_cnt == 0 && !ub_busy && !wt_busy) begin
        // Toggle happens in sequential block
        pc_inc = 1'b1;
        next_state = S_FETCH;
    end else if (micro_cnt > 0) begin
        // If buffers were busy, wait
        if (!ub_busy && !wt_busy) begin
            pc_inc = 1'b1;
            next_state = S_FETCH;
        end
    end
end
```

---

## 📋 **Integration Checklist**

### Controller (`tpu_controller.sv`)
- [ ] Add `stored_acc_addr_reg` and `stored_acc_buf_sel_reg`
- [ ] Fix ST_UB sequence timing (wait for pipeline, then set col_idx=3)
- [ ] Add `ub_rd_valid` input and wait for it in LD_UB
- [ ] Add `ld_ub_completed` flag and enforce LD_UB → MATMUL
- [ ] Store instruction fields (`exec_opcode`, `exec_arg1/2/3`, `exec_flags`)
- [ ] Add CFG_REG instruction handling
- [ ] Fix VPU mode mapping
- [ ] Fix SYNC buffer toggle safety
- [ ] Remove `timescale` directive

### Top-Level (`tpu_top.sv`)
- [ ] Verify `clear_complete` handling (can tie to 1'b1 or export from datapath)
- [ ] Update debug outputs (`fsm_state_debug` replaces `pipeline_stage`)
- [ ] Verify all signal routing matches new controller interface
- [ ] Remove `timescale` directive

### Datapath (`tpu_datapath.sv`)
- [ ] Verify `ub_rd_valid` is properly exposed (already is)
- [ ] Verify ST_UB passthrough timing matches controller sequence
- [ ] No changes needed - datapath should work with new controller

---

## 🔧 **Recommended Implementation Order**

1. **Phase 1**: Fix critical ST_UB sequence and timing
2. **Phase 2**: Add missing instruction field storage
3. **Phase 3**: Add LD_UB → MATMUL enforcement
4. **Phase 4**: Add missing instructions (CFG_REG)
5. **Phase 5**: Fix VPU mode mapping and SYNC safety
6. **Phase 6**: Integration testing

---

## 📝 **Additional Notes**

1. **Instruction Memory**: The proposed code assumes 1-cycle BRAM latency. Verify this matches actual memory implementation.

2. **Hazard Detection**: The new architecture removes explicit hazard detection. Ensure state machine prevents conflicts (e.g., don't start MATMUL while ST_UB is active).

3. **Buffer Selection**: The proposed code manages buffers internally. Ensure this matches datapath expectations.

4. **Testing**: After integration, verify:
   - ST_UB sequence completes atomically
   - LD_UB → MATMUL sequence works
   - All instructions execute correctly
   - Buffer toggling is safe

