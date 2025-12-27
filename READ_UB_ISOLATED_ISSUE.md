# READ_UB UART Command - Isolated Issue Analysis

## Status: Partial Sums Work, READ_UB Command Broken

### What Works ✅
- Partial sums test **PASSED**
- TPU matmul operations work correctly
- Writing to Unified Buffer via UART works
- TPU reading from UB (internal operations) works
- TPU writing results to UB works

### What's Broken ❌
- READ_UB UART command doesn't return correct data
- **Isolated issue** - doesn't affect TPU compute operations

---

## The "Working" Version (Shown by User)

### Interface (Correct)
```systemverilog
output logic [8:0]  ub_rd_addr,     // 9-bit ✓
output logic [8:0]  ub_rd_count,    // Present ✓
input logic         ub_rd_valid,    // Present ✓
```

### READ_LENGTH_LO (Doesn't use UB read interface!)
```systemverilog
8'h04: begin
    // Start read operation - use last_ub_write_data (working solution)
    byte_count <= 16'h0000;
    read_ub_initialized <= 1'b0;
    read_ub_wait_count <= 2'd0;   // Don't wait
    state <= READ_UB;
    // NOTE: No ub_rd_en, no ub_rd_addr, no ub_rd_count!
end
```

### READ_UB State (Uses cached data)
```systemverilog
if (!read_ub_initialized) begin
    // Use last written data (NOT actual UB memory!)
    read_buffer <= last_ub_write_data;
    read_index <= 8'd0;
    byte_count <= 16'd0;
    tx_valid <= 1'b0;
    read_ub_initialized <= 1'b1;
end
```

**Why partial sums work:**
- TPU operations don't use the READ_UB UART command
- TPU reads UB internally (through datapath, not UART)
- Partial sums are computed and stored in UB correctly
- But UART READ_UB command just echoes `last_ub_write_data`

---

## Current Code (After Today's Changes)

### READ_LENGTH_LO (Properly initializes UB read)
```systemverilog
8'h04: begin
    // Start read operation from Unified Buffer
    ub_rd_en <= 1'b1;                         // ✓ Enable read
    ub_rd_addr <= {1'b0, addr_lo};            // ✓ 9-bit address
    ub_rd_count <= 9'd1;                      // ✓ Count signal
    byte_count <= 16'h0000;
    read_ub_initialized <= 1'b0;
    read_ub_wait_valid <= 1'b1;              // ✓ Wait for valid
    state <= READ_UB;
end
```

### READ_UB State (Waits for actual UB data)
```systemverilog
if (read_ub_wait_valid) begin
    // Waiting for unified buffer to provide valid data
    if (ub_rd_valid) begin
        read_buffer <= ub_rd_data;  // ✓ Actual memory data
        read_index <= 8'd0;
        byte_count <= 16'd0;
        tx_valid <= 1'b0;
        read_ub_wait_valid <= 1'b0;
        read_ub_initialized <= 1'b1;
        ub_rd_en <= 1'b0;
    end
end
```

---

## Why Current Code Should Work

1. **Proper initialization**: Sets `ub_rd_en`, `ub_rd_addr`, `ub_rd_count`
2. **Proper handshaking**: Waits for `ub_rd_valid` before capturing data
3. **Reads actual memory**: Uses `ub_rd_data` not `last_ub_write_data`
4. **Bank selection**: Dynamic control ensures UART accesses same bank

---

## Possible Issues if Still Not Working

### 1. Bank Selection Timing
**Check in `tpu_top.sv` line 433:**
```systemverilog
assign ub_buf_sel = use_test_interface ? 
                    (test_ub_wr_en ? 1'b1 : 1'b0) : 
                    ctrl_ub_buf_sel;
```

**Issue**: When transitioning from write to read, `test_ub_wr_en` changes from 1→0, which changes `ub_buf_sel` from 1→0.

**Analysis:**
- Write: `ub_buf_sel=1` → `wr_bank_sel=~1=0` → writes to bank 0
- Read:  `ub_buf_sel=0` → `rd_bank_sel=0` → reads from bank 0
- **Both access bank 0 ✓**

### 2. Unified Buffer State Machine Timing
**Check `unified_buffer.sv` read latency:**
- Cycle 0: `ub_rd_en=1` asserted
- Cycle 1: UB enters RD_READ state, reads BRAM
- Cycle 2: `ub_rd_valid=1`, data available

**Current code waits for `ub_rd_valid` ✓**

### 3. Address Calculation
**Check if addresses match between write and read:**

Write uses: `{1'b0, addr_lo}`
Read uses: `{1'b0, addr_lo}`

Both use MSB=0, so both access lower half of address space ✓

---

## Debugging Steps

### 1. Check if ub_rd_valid ever goes high
Add to READ_UB state:
```systemverilog
if (ub_rd_valid) begin
    debug_last_tx_byte <= 8'hDD;  // Mark that valid was seen
end
```

### 2. Check what data is in ub_rd_data
Capture in debug signals:
```systemverilog
debug_read_buffer_bytes_0_3 <= ub_rd_data[31:0];
```

### 3. Compare with last_ub_write_data
```systemverilog
debug_last_write_bytes_0_3 <= last_ub_write_data[31:0];
```

If `ub_rd_data` differs from `last_ub_write_data`, then UB read is working but returning wrong address data.

If `ub_rd_data` equals `last_ub_write_data`, then UB read is working correctly (for loopback test).

If `ub_rd_valid` never goes high, then UB state machine issue.

---

## Recommendation

The current code is **architecturally correct**. If READ_UB still doesn't work:

1. **Test loopback first**: Write to addr 0, read from addr 0
   - Should work if bank selection is correct

2. **Test different addresses**: Write to addr 0, write to addr 1, read addr 0
   - Verifies it's not just echoing last write

3. **Check debug signals**: Use the debug instrumentation to see:
   - Does `ub_rd_valid` go high?
   - What's in `ub_rd_data`?
   - Does it match what was written?

4. **Timing issue**: If `ub_rd_valid` goes high but wrong data:
   - Check if waiting too many/too few cycles
   - Check if `ub_rd_data` needs extra cycle to stabilize

---

## Summary

- ✅ Partial sums work (TPU operations correct)
- ❌ READ_UB UART command broken (isolated issue)
- ✅ Current code has proper interface and handshaking
- ❓ Need to debug why `ub_rd_data` isn't returning correct values
- Likely issue: Bank selection edge case or timing
- Unlikely issue: Interface signals (all present and correct)

**The fix is NOT more interface changes** - it's debugging the READ_UB state machine timing or bank selection.

