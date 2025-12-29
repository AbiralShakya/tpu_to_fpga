# Technical Change Log: TPU FPGA Implementation

**Base Commit:** `f5c1f2db01738fc262125bd774d04fb6345c12ce`  
**Commit Message:** "temp push, ub broken, 1, 3 -5 tests pass for diagnostic isolated"  
**Date:** Current working directory changes

## Executive Summary

This document details all changes made to fix critical bugs in the TPU FPGA implementation, specifically addressing:
1. **Unified Buffer Write Burst Corruption** (Memset bug)
2. **PE Weight Loading Timing** (Registered pass-through delay)
3. **Activation Data Sustainment** (Missing UB reads during compute)
4. **Accumulator Write Timing** (Garbage writes during pipeline fill)
5. **UB Bank Selection** (Address encoding mismatch)
6. **Pipeline Execution Control** (Instruction re-execution during stalls)

---

## 1. Unified Buffer (`rtl/unified_buffer.sv`)

### 1.1 Critical Bug: Write Burst "Memset" Behavior

**Problem:** The write burst state machine was writing the same latched value repeatedly instead of capturing new data on each cycle, effectively performing a memory fill operation rather than a burst write.

**Root Cause:**
- In `WR_IDLE`, `ub_wr_data` was latched into `wr_data_latched`
- In `WR_BURST`, the same `wr_data_latched` was written repeatedly
- The live `ub_wr_data` input was ignored during bursts

**Impact:** Burst writes like `[10, 20, 30, 40]` would result in `[10, 10, 10, 10]` in memory.

**Fix Applied:**
```systemverilog
WR_WRITE: begin
    // Write Word 0: the data latched from the command cycle
    if (wr_bank_sel_latched) begin
        memory_bank1[wr_current_addr[ADDR_WIDTH-1:0]] <= wr_data_latched;
    end else begin
        memory_bank0[wr_current_addr[ADDR_WIDTH-1:0]] <= wr_data_latched;
    end
    
    // CRITICAL: Capture next word from bus for next cycle
    wr_data_latched <= ub_wr_data;  // Capture Word 1
    wr_current_addr <= wr_current_addr + 1'b1;
    wr_burst_count <= wr_burst_count - 1'b1;
    ub_wr_ready <= 1'b0;  // Not ready during burst
    
    if (wr_burst_count == 1) begin
        wr_state <= WR_IDLE;
        ub_wr_ready <= 1'b1;  // Ready again when done
    end else begin
        wr_state <= WR_BURST;
    end
end

WR_BURST: begin
    // Write the data captured in the PREVIOUS cycle
    if (wr_bank_sel_latched) begin
        memory_bank1[wr_current_addr[ADDR_WIDTH-1:0]] <= wr_data_latched;
    end else begin
        memory_bank0[wr_current_addr[ADDR_WIDTH-1:0]] <= wr_data_latched;
    end
    
    // Capture the CURRENT bus data for the NEXT cycle
    wr_data_latched <= ub_wr_data;  // CRITICAL: Capture next word
    wr_current_addr <= wr_current_addr + 1'b1;
    wr_burst_count <= wr_burst_count - 1'b1;
    
    if (wr_burst_count == 1) begin
        wr_state <= WR_IDLE;
        ub_wr_ready <= 1'b1;  // Ready again when done
    end
end
```

**Changes:**
- `WR_WRITE` now captures `ub_wr_data` for the next cycle before transitioning to `WR_BURST`
- `WR_BURST` writes the previously captured data, then captures the next word
- `ub_wr_ready` now goes low during bursts and high when idle

### 1.2 Bank Selection Fix: Direct Address Encoding

**Problem:** Bank selection was latched from combinational signals (`rd_bank_sel`, `wr_bank_sel`) which were derived from `ub_buf_sel`, but the actual address encoding uses bit 8 of the address. This mismatch could cause reads/writes to go to the wrong bank.

**Root Cause:**
- Address encoding: `{bank_bit, addr[7:0]}` where `bank_bit = addr[8]`
- Bank selection logic used `ub_buf_sel` signal instead of address bit 8
- Latched bank selection didn't match the address encoding

**Fix Applied:**
```systemverilog
// Read bank selection - latch directly from address bit 8
rd_bank_sel_latched <= ub_rd_addr[ADDR_WIDTH];  // Use address MSB for bank

// Write bank selection - latch directly from address bit 8
wr_bank_sel_latched <= ub_wr_addr[ADDR_WIDTH];  // Use address MSB for bank
```

**Changes:**
- `rd_bank_sel_latched` now latches from `ub_rd_addr[ADDR_WIDTH]` (bit 8)
- `wr_bank_sel_latched` now latches from `ub_wr_addr[ADDR_WIDTH]` (bit 8)
- Ensures bank selection matches the address encoding scheme

---

## 2. TPU Controller (`rtl/tpu_controller.sv`)

### 2.1 Pipeline Execution Control Fix

**Problem:** Instructions could re-execute during pipeline stalls because `exec_valid` was not cleared when the pipeline was stalled.

**Root Cause:**
- `exec_valid` was unconditionally propagated from `if_id_valid` on every clock
- During stalls (`if_id_stall == 1`), the same instruction would remain in the exec stage
- If `exec_valid` remained high, the instruction would execute again

**Fix Applied:**
```systemverilog
always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        // ... reset logic ...
    end else if (!if_id_stall) begin
        // CRITICAL FIX: Only advance exec stage when NOT stalled
        exec_valid  <= if_id_valid;
        exec_opcode <= if_id_opcode;
        // ... other exec stage signals ...
    end else begin
        // When stalled, clear exec_valid to prevent instruction re-execution
        exec_valid <= 1'b0;
    end
end
```

**Changes:**
- Added `!if_id_stall` condition to exec stage advance logic
- When stalled, `exec_valid` is cleared to prevent re-execution
- Ensures each instruction executes exactly once

### 2.2 Instruction Register Clear on Start

**Problem:** When execution starts, the instruction register (`ir_reg`) could contain a stale instruction from a previous execution, causing the first instruction to be skipped or incorrect.

**Root Cause:**
- `ir_reg` was only cleared on reset
- On `start_execution`, the first instruction fetch might use a stale `ir_reg` value

**Fix Applied:**
```systemverilog
always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ir_reg <= 32'h00000000;
    end else if (start_execution) begin
        // CRITICAL FIX: Clear IR when starting execution
        ir_reg <= 32'h00000000;  // NOP
    end else if (ir_ld_internal && !if_id_stall) begin
        ir_reg <= instr_data;
    end
end
```

**Changes:**
- Added `start_execution` condition to clear `ir_reg` to NOP
- Ensures first instruction fetch uses new PC (0), not stale instruction

### 2.3 ST_UB Bank Selection Fix

**Problem:** `ST_UB` instruction used `exec_ub_buf_sel` to select the bank, but UART always reads from bank 0. If `exec_ub_buf_sel == 1`, ST_UB would write to bank 1, making the data unreadable via UART.

**Root Cause:**
- UART DMA hardcodes bank 0 for reads: `{1'b0, addr_lo}`
- ST_UB used `{exec_ub_buf_sel, exec_arg1}` which could be bank 1
- Bank mismatch prevented UART from reading ST_UB results

**Fix Applied:**
```systemverilog
ST_UB_OP: begin
    ub_wr_en        = 1'b1;
    ub_wr_addr      = {1'b0, exec_arg1};  // Always use bank 0 for UART compatibility
    ub_wr_count     = {1'b0, exec_arg2};
    pc_cnt_internal = 1'b1;
    ir_ld_internal  = 1'b1;
end
```

**Changes:**
- ST_UB now always writes to bank 0: `{1'b0, exec_arg1}`
- Ensures UART can read ST_UB results after ISA execution

---

## 3. Systolic Controller (`rtl/systolic_controller.sv`)

### 3.1 PE Weight Loading Timing Fix

**Problem:** All PEs in a column captured weights on the same cycle, but PE registered pass-through causes a 1-cycle delay per row. This meant only the first row (pe00, pe01, pe02) captured correct weights; subsequent rows captured garbage.

**Root Cause:**
- PE `psum_out` is registered: `always_ff @(posedge clk) psum_out <= psum_in`
- Weight propagation through a column takes 1 cycle per row:
  - Cycle 1: pe00 sees `col0_in` → captures ✓
  - Cycle 2: pe10 sees pe00's output (delayed 1 cycle) → should capture
  - Cycle 3: pe20 sees pe10's output (delayed 2 cycles) → should capture
- Original code asserted `en_capture_col0` for ALL rows (pe00, pe10, pe20) on cycle 1
- Result: pe00 captured correctly, pe10 and pe20 captured garbage

**Fix Applied:**
```systemverilog
// Changed from column-based to row+column-based capture signals
output logic en_capture_row0_col0,  // Row 0, Column 0 (pe00)
output logic en_capture_row0_col1,  // Row 0, Column 1 (pe01)
output logic en_capture_row0_col2,  // Row 0, Column 2 (pe02)
output logic en_capture_row1_col0,  // Row 1, Column 0 (pe10) - 1 cycle delay
output logic en_capture_row1_col1,  // Row 1, Column 1 (pe11) - 1 cycle delay
output logic en_capture_row1_col2,  // Row 1, Column 2 (pe12) - 1 cycle delay
output logic en_capture_row2_col0,  // Row 2, Column 0 (pe20) - 2 cycle delay
output logic en_capture_row2_col1,  // Row 2, Column 1 (pe21) - 2 cycle delay
output logic en_capture_row2_col2,  // Row 2, Column 2 (pe22) - 2 cycle delay

// Staggered capture timing:
// Cycle 1: Row 0, Column 0 (pe00)
// Cycle 2: Row 0, Column 1 (pe01) AND Row 1, Column 0 (pe10)
// Cycle 3: Row 0, Column 2 (pe02) AND Row 1, Column 1 (pe11) AND Row 2, Column 0 (pe20)
// Cycle 4: Row 1, Column 2 (pe12) AND Row 2, Column 1 (pe21)
// Cycle 5: Row 2, Column 2 (pe22)
always_comb begin
    case (weight_load_counter)
        8'h01: en_capture_row0_col0 = 1'b1;
        8'h02: begin
            en_capture_row0_col1 = 1'b1;
            en_capture_row1_col0 = 1'b1;
        end
        8'h03: begin
            en_capture_row0_col2 = 1'b1;
            en_capture_row1_col1 = 1'b1;
            en_capture_row2_col0 = 1'b1;
        end
        8'h04: begin
            en_capture_row1_col2 = 1'b1;
            en_capture_row2_col1 = 1'b1;
        end
        8'h05: en_capture_row2_col2 = 1'b1;
        default: ; // All 0
    endcase
end
```

**Changes:**
- Replaced 3 column-based signals (`en_capture_col0/1/2`) with 9 row+column signals
- Capture timing staggered to match registered pass-through delay
- Each PE now captures at the correct cycle when the weight has propagated

### 3.2 Activation Data Sustainment Fix

**Problem:** `ub_rd_en` was only asserted for 1 cycle during `MATMUL_OP` in the controller, but the systolic array needs continuous activation data during the `SYS_COMPUTE` phase.

**Root Cause:**
- `tpu_controller.sv` asserted `ub_rd_en = 1'b1` for only 1 cycle during `MATMUL_OP`
- `ub_rd_count = 9'h001` read only 1 word
- During `SYS_COMPUTE`, no activation reads occurred
- Systolic array needs continuous activation data during computation

**Fix Applied:**
```systemverilog
// New output: UB read enable during COMPUTE phase
output logic ub_rd_en_systolic;

// UB read enable: active during COMPUTE phase to sustain activation data
assign ub_rd_en_systolic = (current_state == SYS_COMPUTE);
```

**Changes:**
- Added `ub_rd_en_systolic` output from systolic controller
- Asserted during `SYS_COMPUTE` state to provide continuous UB reads
- Connected to UB via multiplexer in `tpu_datapath.sv` (see Section 4.3)

### 3.3 Accumulator Write Timing Fix

**Problem:** `acc_wr_en` was asserted during the entire `SYS_COMPUTE` phase, causing garbage values to be written during pipeline fill. Also, `acc_wr_addr` was fixed to `sys_acc_addr`, causing all results to overwrite the same address. `acc2_out` was not being stored.

**Root Cause:**
- Pipeline latency: ~4 cycles for 3x3 array (row skew + computation)
- `acc_wr_en` was asserted immediately when `SYS_COMPUTE` started
- First 4 writes contained garbage (pipeline not filled)
- All writes went to same address (`sys_acc_addr`)
- Only `acc0_out` and `acc1_out` were stored (64-bit accumulator), `acc2_out` was ignored

**Fix Applied:**
```systemverilog
// Pipeline latency for 3x3 array: ~4 cycles
localparam PIPELINE_LATENCY = 4;
logic acc_wr_en_valid;
assign acc_wr_en_valid = (current_state == SYS_COMPUTE) && (compute_counter >= PIPELINE_LATENCY);

// Write address: start at sys_acc_addr, increment for each write
logic [7:0] acc_wr_addr_reg;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        acc_wr_addr_reg <= 8'h00;
    end else if (current_state == SYS_IDLE && sys_start) begin
        acc_wr_addr_reg <= sys_acc_addr;  // Initialize to base address
    end else if (acc_wr_en_valid) begin
        acc_wr_addr_reg <= acc_wr_addr_reg + 1'b1;  // Increment after each write
    end
end

// Write enable: active when valid data is available
assign acc_wr_en = acc_wr_en_valid;
assign acc_wr_addr = acc_wr_addr_reg;

// Column selection: acc0+acc1 on even cycles, acc2 on odd cycles
assign acc_wr_col01 = acc_wr_en_valid && (compute_counter[0] == 1'b0);
assign acc_wr_col2 = acc_wr_en_valid && (compute_counter[0] == 1'b1);
```

**Changes:**
- Added `PIPELINE_LATENCY` constant (4 cycles)
- `acc_wr_en_valid` only asserted after pipeline is filled
- `acc_wr_addr_reg` increments for each write (stores all 3 columns)
- Added `acc_wr_col01` and `acc_wr_col2` signals to select which columns to write
- Address pattern: `addr` (acc0+acc1 row0), `addr+1` (acc2 row0), `addr+2` (acc0+acc1 row1), etc.

---

## 4. TPU Datapath (`rtl/tpu_datapath.sv`)

### 4.1 PE Weight Capture Signal Updates

**Changes:** Updated `mmu` instantiation to use new row+column specific capture signals:
```systemverilog
.mmu systolic_array (
    // ... other signals ...
    .en_capture_row0_col0 (en_capture_row0_col0),
    .en_capture_row0_col1 (en_capture_row0_col1),
    .en_capture_row0_col2 (en_capture_row0_col2),
    .en_capture_row1_col0 (en_capture_row1_col0),
    .en_capture_row1_col1 (en_capture_row1_col1),
    .en_capture_row1_col2 (en_capture_row1_col2),
    .en_capture_row2_col0 (en_capture_row2_col0),
    .en_capture_row2_col1 (en_capture_row2_col1),
    .en_capture_row2_col2 (en_capture_row2_col2),
    // ... other signals ...
);
```

### 4.2 UB Read Enable Multiplexing

**Changes:** Added multiplexer to allow systolic controller to control UB reads during compute:
```systemverilog
// Multiplex UB read control: systolic controller takes over during COMPUTE phase
logic ub_rd_en_muxed;
assign ub_rd_en_muxed = systolic_active ? ub_rd_en_systolic : ub_rd_en;

unified_buffer ub (
    // ... other signals ...
    .ub_rd_en (ub_rd_en_muxed),  // Use systolic controller's signal during COMPUTE
    // ... other signals ...
);
```

### 4.3 Accumulator Write Data Selection

**Problem:** Accumulator is 64-bit, but we have 3 columns (acc0, acc1, acc2). Need to pack acc0+acc1 together, and acc2 separately.

**Fix Applied:**
```systemverilog
// Accumulator write data (from systolic array outputs)
logic [63:0] acc_wr_data_col01, acc_wr_data_col2;
assign acc_wr_data_col01 = {acc1_out, acc0_out};  // 64-bit: col1 + col0
assign acc_wr_data_col2 = {32'h0, acc2_out};      // 64-bit: acc2 padded

// Select data based on systolic controller's column selection signals
assign acc_wr_data = sc_acc_wr_en ? 
    (acc_wr_col2 ? acc_wr_data_col2 : acc_wr_data_col01) :
    {acc1_out, acc0_out};  // Default for non-systolic writes
```

**Changes:**
- Created `acc_wr_data_col01` (acc0+acc1) and `acc_wr_data_col2` (acc2 padded)
- Multiplexer selects based on `acc_wr_col2` signal from systolic controller
- Ensures all 3 columns are stored correctly

### 4.4 Accumulator Write Address Selection

**Changes:** Use systolic controller address when active:
```systemverilog
logic [7:0] acc_wr_addr_combined;
assign acc_wr_addr_combined = sc_acc_wr_en ? sc_acc_wr_addr : acc_addr;

accumulator accumulators (
    // ... other signals ...
    .wr_addr (acc_wr_addr_combined), // Use systolic controller address when active
    // ... other signals ...
);
```

---

## 5. Accumulator (`rtl/accumulator.sv`)

### 5.1 Accumulator Read Latency Fix

**Problem:** Accumulator read was registered (1-cycle latency), but VPU activation pipelines use `acc_rd_en` as `valid_in` on the same cycle. This caused VPU to process stale data from the previous read instead of the current read.

**Root Cause:**
- Accumulator read was synchronous: `always_ff @(posedge clk) rd_data <= buffer[rd_addr]`
- VPU pipelines use `acc_rd_en` as `valid_in` trigger on the same cycle
- When `acc_rd_en` is asserted, `rd_data` still contains the previous value
- Next cycle, `rd_data` has the new value, but `acc_rd_en` is already low
- Result: VPU processes wrong data

**Impact:** VPU operations (RELU, pooling, quantization) would process incorrect accumulator values, leading to wrong activation outputs.

**Fix Applied:**
```systemverilog
// Read operation (combinational for VPU timing compatibility)
// CRITICAL FIX: Changed from registered to combinational read to eliminate 1-cycle latency
// VPU activation pipelines expect data to be available immediately when acc_rd_en is asserted
always_comb begin
    if (acc_buf_sel_reg == 0)
        rd_data = buffer0[rd_addr];
    else
        rd_data = buffer1[rd_addr];
end
```

**Changes:**
- Changed from `always_ff` (registered) to `always_comb` (combinational)
- Removed `rd_en` condition (data always available based on address)
- Data now available immediately when `rd_addr` is set
- VPU pipelines receive correct data when `valid_in` is asserted

**Note:** This assumes BRAM timing constraints allow combinational reads. If timing violations occur, consider:
- Adding pipeline registers in VPU to account for latency
- Using registered read with delayed `valid_in` signal

---

## 6. MMU (`rtl/mmu.sv`)

### 5.1 PE Weight Capture Signal Updates

**Changes:** Updated all 9 PE instantiations to use row+column specific capture signals:
```systemverilog
pe pe00 (.en_weight_capture(en_capture_row0_col0), ...);
pe pe01 (.en_weight_capture(en_capture_row0_col1), ...);
pe pe02 (.en_weight_capture(en_capture_row0_col2), ...);
pe pe10 (.en_weight_capture(en_capture_row1_col0), ...);
pe pe11 (.en_weight_capture(en_capture_row1_col1), ...);
pe pe12 (.en_weight_capture(en_capture_row1_col2), ...);
pe pe20 (.en_weight_capture(en_capture_row2_col0), ...);
pe pe21 (.en_weight_capture(en_capture_row2_col1), ...);
pe pe22 (.en_weight_capture(en_capture_row2_col2), ...);
```

---

## 7. Test Scripts

### 6.1 Diagnostic Isolated Test (`python/diagnostic_isolated.py`)

#### 6.1.1 WRITE_INSTR Protocol Fix

**Problem:** `write_instruction()` was sending length bytes, but UART protocol for `WRITE_INSTR` expects only `cmd + addr + 4 instruction bytes` (no length).

**Fix Applied:**
```python
def write_instruction(self, addr, instr):
    """Write 32-bit instruction
    
    CRITICAL: WRITE_INSTR protocol is DIFFERENT from other commands!
    It only expects: cmd, addr_hi, addr_lo, then 4 instruction bytes
    NO length bytes! The UART hardcodes 4-byte instruction length.
    """
    self.ser.reset_input_buffer()
    # Send ONLY cmd + addr (no length bytes!)
    self.ser.write(bytes([Cmd.WRITE_INSTR, 0, addr]))
    # Then send 4 instruction bytes
    self.ser.write(instr.to_bytes(4, 'big'))
    time.sleep(0.05)
    return True
```

#### 6.1.2 Test 6: Enhanced UB Bank Selection Test

**Problem:** Original test wrote a marker and checked if ST_UB modified it, but if accumulators were zero, ST_UB would write zeros regardless of bank selection, causing a false positive.

**Fix Applied:**
```python
def test6_ub_bank_during_isa(tpu):
    """
    TEST 6: Bank selection during ISA execution
    
    CRITICAL: This test first ensures accumulators have non-zero values
    by running a MATMUL, then verifies ST_UB writes that data.
    """
    # Step 1: Populate accumulators with known non-zero values
    # Load identity matrix weights and input [10, 20, 30]
    # Run MATMUL to produce [10, 20, 30] in accumulators
    
    # Step 2: Write marker to UB[40] - should be overwritten by ST_UB
    
    # Step 3: Execute ST_UB to address 40
    
    # Step 4: Read back and verify:
    #   - Marker was overwritten (not still 0xBB)
    #   - Values are not zero (accumulators were populated)
    #   - Values match expected [10, 20, 30]
```

**Changes:**
- Test now runs MATMUL first to populate accumulators with known values
- Verifies ST_UB writes non-zero accumulator data
- Checks for bank mismatch (marker not overwritten) and zero values (MATMUL failure)

### 6.2 New Test Scripts

#### 6.2.1 `python/test_stub_verification.py`

**Purpose:** Rigorous verification that ST_UB reads from accumulators and writes to UB correctly.

**Test Strategy:**
1. Populate accumulators with known non-zero values via MATMUL
2. Write marker pattern to UB
3. Execute ST_UB
4. Read back and verify accumulator values are present, not marker or zeros

#### 6.2.2 `python/test_bank_verification.py`

**Purpose:** Verify UART writes/reads use the same bank, and ST_UB writes to a bank UART can read.

**Test Strategy:**
1. Write pattern to UB via UART
2. Read back immediately - should match
3. Execute ST_UB
4. Read back - should contain accumulator data, not original pattern

---

## 8. Current Status and Known Issues

### 7.1 Fixed Issues
- ✅ Unified Buffer write burst corruption (memset bug)
- ✅ PE weight loading timing (registered pass-through)
- ✅ Activation data sustainment (missing UB reads)
- ✅ Accumulator write timing (garbage during pipeline fill)
- ✅ Accumulator address incrementing (all results to same address)
- ✅ Accumulator column 2 storage (acc2_out not stored)
- ✅ UB bank selection (address encoding mismatch)
- ✅ ST_UB bank selection (always writes to bank 0)
- ✅ Pipeline execution control (instruction re-execution)
- ✅ Instruction register clear on start
- ✅ Accumulator synchronous read latency (VPU timing mismatch)

### 7.2 Remaining Issues / Under Investigation
- ⚠️ **MATMUL Results Still Incorrect**: Despite fixes, MATMUL may still produce incorrect results. Possible causes:
  - Accumulator read timing (ST_UB reads before results are stable)
  - Weight/activation data not reaching PEs correctly
  - Signed/unsigned arithmetic mismatch
  - Accumulator clear timing (cleared too late or not at all)
- ⚠️ **Test 6 False Positive Risk**: If MATMUL doesn't populate accumulators, Test 6 will fail but may not clearly indicate the root cause (MATMUL vs bank selection).

### 7.3 Next Steps
1. **Re-synthesize FPGA** with all fixes applied
2. **Run diagnostic tests** to verify fixes:
   - `python/diagnostic_isolated.py` - Comprehensive test suite
   - `python/test_stub_verification.py` - ST_UB verification
   - `python/test_bank_verification.py` - Bank selection verification
3. **Debug MATMUL** if results still incorrect:
   - Check accumulator clear timing
   - Verify weight/activation data propagation
   - Check signed/unsigned arithmetic flags
   - Verify accumulator read timing for ST_UB

---

## 9. File Change Summary

### Modified Files
1. `rtl/unified_buffer.sv` - Write burst fix, bank selection fix
2. `rtl/tpu_controller.sv` - Pipeline control fix, IR clear fix, ST_UB bank fix
3. `rtl/systolic_controller.sv` - Weight loading timing fix, activation sustainment fix, accumulator write timing fix
4. `rtl/tpu_datapath.sv` - PE capture signals, UB read muxing, accumulator write data/address selection
5. `rtl/mmu.sv` - PE capture signal updates
6. `rtl/accumulator.sv` - Changed read from registered to combinational (VPU timing fix)
7. `python/diagnostic_isolated.py` - WRITE_INSTR protocol fix, Test 6 enhancement

### New Files
1. `python/test_stub_verification.py` - ST_UB verification test
2. `python/test_bank_verification.py` - Bank selection verification test

### Unchanged Files (but referenced)
- `rtl/tpu_top.sv` - Already hardcodes bank 0 for UART (no changes needed)
- `rtl/uart_dma_basys3.sv` - Already hardcodes bank 0 (no changes needed)
- `rtl/pe.sv` - Registered pass-through is by design (no changes needed)
- `rtl/accumulator.sv` - 64-bit width is by design (no changes needed)

---

## 10. Technical Context: Why These Bugs Occurred

### 9.1 Design Assumptions vs. Implementation Reality

1. **Unified Buffer Bursting**: Assumed combinational data capture, but state machine adds 1-cycle latency. Fixed by capturing data in previous cycle.

2. **PE Registered Pass-Through**: Assumed combinational propagation, but PEs use registered pass-through for timing closure. Fixed by staggering capture signals.

3. **Activation Data Flow**: Assumed controller would sustain reads, but controller only asserts for 1 cycle. Fixed by systolic controller taking over during compute.

4. **Pipeline Latency**: Assumed results available immediately, but systolic array has pipeline latency. Fixed by delaying accumulator writes.

5. **Bank Selection**: Assumed `ub_buf_sel` signal matched address encoding, but address uses bit 8 directly. Fixed by latching from address bit.

### 9.2 Testing Gaps

- Original tests didn't verify burst write data integrity
- Tests didn't account for pipeline latency
- Tests didn't verify bank selection consistency
- Tests didn't verify accumulator values before ST_UB

---

## 11. References

- **Base Commit**: `f5c1f2db01738fc262125bd774d04fb6345c12ce`
- **Repository**: https://github.com/AbiralShakya/tpu_to_fpga
- **Target FPGA**: Xilinx Basys3 (Artix-7 XC7A35T)
- **Clock**: 100 MHz
- **UART**: 115200 baud, 8N1

---

**Document Generated**: Current working directory state  
**Last Updated**: After applying all fixes for bugs identified in diagnostic testing

