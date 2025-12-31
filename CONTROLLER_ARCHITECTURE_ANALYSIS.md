# Controller Architecture Analysis & Refactoring Plan

## Current Implementation vs. Desired Architecture

### Current State: Pipeline-Based (FETCH -> DECODE -> EXECUTE)

**Current Approach:**
- Uses IF/ID pipeline register
- Hazard detection stalls pipeline
- Instructions execute in combinational always block
- ST_UB has sub-state machine but can be interrupted
- No explicit state machine for operation sequencing

**Issues:**
1. ❌ No explicit EXEC_LOAD_WEIGHTS, EXEC_LD_UB, EXEC_MATMUL states
2. ❌ ST_UB sequence can be interrupted (zombie mode risk)
3. ❌ No PIPE_LATENCY parameter for handshaking
4. ❌ No explicit op_cycle_cnt and sub_step_cnt counters
5. ❌ Stale data risk: No enforced LD_UB -> MATMUL sequence
6. ❌ Timing assumptions not explicit

---

## Desired Architecture: Explicit State Machine

### High-Level States

```
IDLE -> DISPATCH -> EXECUTE_SPECIFIC -> WAIT_DONE -> IDLE
```

### Detailed State Breakdown

#### 1. IDLE
- **Purpose**: Wait for instruction or sys_start
- **Transitions**: 
  - `start_execution` → DISPATCH
  - `exec_valid` → DISPATCH

#### 2. DISPATCH
- **Purpose**: Decode opcode and route to specific execution state
- **Transitions**:
  - `RD_WEIGHT` → EXEC_LOAD_WEIGHTS
  - `LD_UB` → EXEC_LD_UB
  - `MATMUL` → EXEC_LD_UB (enforce LD_UB first!)
  - `ST_UB` → EXEC_ST_UB
  - `VPU/RELU/etc` → EXEC_ACTIVATE
  - `SYNC` → EXEC_SYNC

#### 3. EXEC_LOAD_WEIGHTS
- **Purpose**: Fill dual_weight_fifo
- **Actions**:
  - Assert `wt_mem_rd_en`
  - Monitor `weight_load_counter` from systolic_controller
  - Pop FIFO exactly 7 times (flush skew registers)
- **Exit**: `weight_load_counter >= sys_rows` OR `weight_load_counter >= 7` (for 3x3)
- **Counters**: `op_cycle_cnt` tracks weight loading cycles

#### 4. EXEC_LD_UB
- **Purpose**: Load activations from UB to datapath latches
- **Actions**:
  - Assert `ub_rd_en` at address X
  - Wait for `ub_rd_valid` pulse (CRITICAL: 1 cycle wait)
  - Datapath latches on `ub_rd_valid`
- **Exit**: `ub_rd_valid` received
- **Hazard Prevention**: Must complete before MATMUL

#### 5. EXEC_MATMUL
- **Purpose**: Run systolic array computation
- **Actions**:
  - Assert `sys_start`
  - Assert `systolic_active` (via systolic_controller)
  - Count cycles: `Rows + Cols + Latency`
  - For 3x3: Need `PIPELINE_LATENCY + sys_rows` cycles
- **Exit**: `sys_done` pulse OR counter reaches total
- **Counters**: `op_cycle_cnt` tracks compute cycles
- **Self-Correction**: Ensure `systolic_active` stays high until last partial sum captured

#### 6. EXEC_ST_UB (Sub-State Machine: "Column Walker")
- **Purpose**: Move 96-bit accumulator → 8-bit UB
- **Sub-States** (HARD-CODED SEQUENCE, CANNOT ABORT):
  - **READ_REQ**: Assert `acc_rd_en` (Address Y)
  - **WAIT_PIPE**: Wait `PIPE_LATENCY` cycles (BRAM + Pipeline latency)
  - **BYTE_0**: `st_ub_col_idx = 0`, latch byte 0
  - **BYTE_1**: `st_ub_col_idx = 1`, latch byte 1
  - **BYTE_2**: `st_ub_col_idx = 2`, latch byte 2
  - **WRITE**: `st_ub_col_idx = 3`, assert `ub_wr_en`
- **Exit**: Must complete sequence 0→1→2→3 (zombie mode prevention)
- **Counters**: `sub_step_cnt` tracks 0-1-2-3 sequence
- **Critical**: Sequence completion clears `st_ub_passthrough` latch in datapath

#### 7. EXEC_ACTIVATE
- **Purpose**: Run VPU/Activation pipeline
- **Actions**:
  - Assert `vpu_start`
  - Wait for `vpu_done`
- **Exit**: `vpu_done` pulse

#### 8. EXEC_SYNC
- **Purpose**: Toggle double-buffers
- **Actions**:
  - Check `sys_busy == 0` AND `ub_busy == 0` (truly idle)
  - Toggle `acc_buf_sel`
  - Toggle `ub_buf_sel` (via `ub_buf_sel`)
- **Exit**: Immediate (1 cycle)

#### 9. WAIT_DONE
- **Purpose**: Wait for operation completion signal
- **Exit**: Operation-specific done signal

---

## Required Changes

### 1. Add State Machine Definition

```systemverilog
typedef enum logic [3:0] {
    STATE_IDLE           = 4'h0,
    STATE_DISPATCH       = 4'h1,
    STATE_EXEC_LOAD_WT   = 4'h2,
    STATE_EXEC_LD_UB     = 4'h3,
    STATE_EXEC_MATMUL    = 4'h4,
    STATE_EXEC_ST_UB     = 4'h5,
    STATE_EXEC_ACTIVATE  = 4'h6,
    STATE_EXEC_SYNC      = 4'h7,
    STATE_WAIT_DONE      = 4'h8
} controller_state_t;

// ST_UB sub-states (hard-coded sequence)
typedef enum logic [2:0] {
    ST_UB_READ_REQ = 3'h0,
    ST_UB_WAIT_PIPE = 3'h1,
    ST_UB_BYTE_0 = 3'h2,
    ST_UB_BYTE_1 = 3'h3,
    ST_UB_BYTE_2 = 3'h4,
    ST_UB_WRITE = 3'h5
} st_ub_substate_t;
```

### 2. Add Parameters

```systemverilog
localparam PIPE_LATENCY = 2;  // BRAM (1 cycle) + Pipeline (1 cycle) = 2 cycles
localparam MATMUL_MIN_CYCLES = 7;  // For 3x3: PIPELINE_LATENCY (4) + writes (3)
```

### 3. Add Counters

```systemverilog
logic [7:0] op_cycle_cnt;      // General purpose counter for state duration
logic [2:0] sub_step_cnt;       // Micro-sequencing (0-1-2-3 for ST_UB)
```

### 4. Enforce LD_UB -> MATMUL Sequence

```systemverilog
// In DISPATCH state:
if (exec_opcode == MATMUL_OP) begin
    // CRITICAL: Check if LD_UB was executed
    if (!ld_ub_completed) begin
        // Force LD_UB first
        next_state = STATE_EXEC_LD_UB;
    end else begin
        next_state = STATE_EXEC_MATMUL;
    end
end
```

### 5. Hard-Code ST_UB Sequence

```systemverilog
// ST_UB state machine - CANNOT BE INTERRUPTED
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        st_ub_substate <= ST_UB_READ_REQ;
    end else if (current_state == STATE_EXEC_ST_UB) begin
        case (st_ub_substate)
            ST_UB_READ_REQ: st_ub_substate <= ST_UB_WAIT_PIPE;
            ST_UB_WAIT_PIPE: 
                if (sub_step_cnt >= PIPE_LATENCY)
                    st_ub_substate <= ST_UB_BYTE_0;
            ST_UB_BYTE_0: st_ub_substate <= ST_UB_BYTE_1;
            ST_UB_BYTE_1: st_ub_substate <= ST_UB_BYTE_2;
            ST_UB_BYTE_2: st_ub_substate <= ST_UB_WRITE;
            ST_UB_WRITE: 
                // Sequence complete, can exit
                if (ub_wr_en && !ub_busy)
                    st_ub_substate <= ST_UB_READ_REQ;  // Reset for next time
            default: st_ub_substate <= ST_UB_READ_REQ;
        endcase
    end
    // CRITICAL: Cannot exit ST_UB state until sequence completes
    // This prevents zombie mode
end
```

### 6. Add Handshaking for LD_UB

```systemverilog
// In EXEC_LD_UB state:
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ld_ub_completed <= 1'b0;
    end else if (current_state == STATE_EXEC_LD_UB) begin
        if (ub_rd_valid) begin
            ld_ub_completed <= 1'b1;  // Mark LD_UB as complete
            next_state = STATE_WAIT_DONE;
        end
    end else if (current_state == STATE_EXEC_MATMUL && sys_done) begin
        ld_ub_completed <= 1'b0;  // Reset for next MATMUL
    end
end
```

---

## Implementation Priority

### Phase 1: Critical Fixes (Immediate)
1. ✅ Add `PIPE_LATENCY` parameter
2. ✅ Hard-code ST_UB sequence (prevent zombie mode)
3. ✅ Add `op_cycle_cnt` and `sub_step_cnt` counters
4. ✅ Enforce LD_UB -> MATMUL sequence

### Phase 2: State Machine Refactoring (Next)
1. Convert to explicit state machine
2. Add DISPATCH state
3. Separate EXEC states for each operation type
4. Add WAIT_DONE state

### Phase 3: Timing Refinement (Later)
1. Verify all handshaking timing
2. Add explicit cycle counting
3. Verify hazard prevention

---

## Current Code Locations

- **State Machine**: Currently implicit in combinational always block (line 531)
- **ST_UB Sub-State**: Lines 192-199, 369-392
- **Counters**: Missing - need to add
- **Handshaking**: Partial - needs LD_UB completion tracking

---

## Notes

- The current pipeline approach works but lacks explicit state visibility
- The refactoring will make timing and sequencing explicit
- ST_UB zombie mode fix is CRITICAL and should be done first
- LD_UB -> MATMUL enforcement prevents stale data bugs

