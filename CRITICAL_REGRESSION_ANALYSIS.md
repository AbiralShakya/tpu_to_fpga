# CRITICAL REGRESSION ANALYSIS

## Timeline of Changes

### ✅ WORKING STATE: fef0361 (Dec 26, 22:56) - "cleanup" by abiral_chip

**uart_dma_basys3.sv Interface:**
```systemverilog
output reg [7:0]  ub_wr_addr,
output reg [7:0]  ub_rd_addr,
// NO ub_rd_count
// NO ub_rd_valid
```

**READ_UB Implementation:**
```systemverilog
if (!read_ub_initialized) begin
    // Initialize on first entry - load data from Unified Buffer
    read_buffer <= last_ub_write_data;  // ← USES LAST WRITTEN DATA!
    read_index <= 8'd0;
    byte_count <= 16'd0;
    tx_valid <= 1'b0;
    read_ub_initialized <= 1'b1;
}
```

**Why it "worked":**
- Writes stored data in `last_ub_write_data`
- Reads just echoed back `last_ub_write_data`
- **NEVER ACTUALLY READ FROM UNIFIED BUFFER MEMORY!**
- Write/read loopback appeared to work
- But partial sums (which require reading computed values) would FAIL

---

### ⚠️ BROKEN STATE: 7a7acba (Dec 26, 23:28) - "super temp" by abiral_chip

**uart_dma_basys3.sv Interface:**
```systemverilog
output logic [7:0]  ub_wr_addr,     // ← Still 8-bit!
output logic [8:0]  ub_rd_addr,     // ← Changed to 9-bit
output logic [8:0]  ub_rd_count,    // ← Added
input logic         ub_rd_valid,    // ← Added
```

**READ_UB Implementation:**
```systemverilog
if (read_ub_wait_valid) begin
    // Waiting for unified buffer to provide valid data
    if (ub_rd_valid) begin
        read_buffer <= ub_rd_data;  // ← NOW READS ACTUAL UB DATA!
        read_index <= 8'd0;
        byte_count <= 16'd0;
        tx_valid <= 1'b0;
        read_ub_wait_valid <= 1'b0;
        read_ub_initialized <= 1'b1;
        ub_rd_en <= 1'b0;
    end
}
```

**Problems:**
1. ✅ READ interface updated (9-bit, count, valid) - GOOD
2. ❌ WRITE interface still 8-bit - INCONSISTENT
3. ❌ Bank selection logic may not work correctly
4. ❌ Mixed 8-bit/9-bit addressing causes issues

---

### 🔧 ATTEMPTED FIX: fc9e33d (Dec 26, 21:12) - "debug" by Alan Ma

**uart_dma_basys3.sv Interface:**
```systemverilog
output logic [8:0]  ub_wr_addr,     // ← Fixed to 9-bit
output logic [8:0]  ub_wr_count,    // ← Added
output logic [8:0]  ub_rd_addr,     // ← Already 9-bit
output logic [8:0]  ub_rd_count,    // ← Already present
input logic         ub_rd_valid,    // ← Already present
```

**Changes:**
1. ✅ Made interface consistent (all 9-bit)
2. ✅ Added `ub_wr_count`
3. ✅ Updated all instantiations

**Remaining Issues:**
- Need to verify bank selection logic works correctly
- Need to verify READ_UB initialization sequence
- Need to check if READ_LENGTH_LO properly sets up ub_rd_en

---

## ROOT CAUSE ANALYSIS

### The False Positive

**In commit fef0361:**
- Matmuls "worked" because writes and reads were loopback only
- `read_buffer <= last_ub_write_data` made it look like reads worked
- But TPU computations (matmul, accumulate) store results in UB
- Those computed results were NEVER actually readable!

### The Real Problem

**Abiral's attempt (7a7acba) to fix it:**
- Added proper `ub_rd_valid` handshaking
- Changed to read actual `ub_rd_data` from memory
- BUT inconsistent interface (8-bit write, 9-bit read) broke everything

### What Actually Needs to Work

For **partial sums and matmul results** to work:

1. **Write path:** Host → UART DMA → Unified Buffer (input data)
2. **Compute:** TPU matmul → results stored in Unified Buffer
3. **Read path:** Unified Buffer → UART DMA → Host (computed results)

**Step 3 was BROKEN in fef0361!** It only worked for loopback testing.

---

## CRITICAL CHECKS NEEDED

### 1. Check READ_LENGTH_LO State

In the current code, when READ_UB command is received, check:

```systemverilog
READ_LENGTH_LO: begin
    if (rx_valid && !rx_framing_error) begin
        length[7:0] <= rx_data;
        
        case (command)
            8'h04: begin
                // Start read operation from Unified Buffer
                ub_rd_en <= 1'b1;                         // ← IS THIS SET?
                ub_rd_addr <= {1'b0, addr_lo};            // ← 9-bit address
                ub_rd_count <= 9'd1;                      // ← Count signal
                byte_count <= 16'h0000;
                read_ub_initialized <= 1'b0;
                read_ub_wait_valid <= 1'b1;              // ← Wait for valid
                state <= READ_UB;
            end
        endcase
    end
end
```

### 2. Check Bank Selection in tpu_top.sv

Current logic:
```systemverilog
assign ub_buf_sel = use_test_interface ? (test_ub_wr_en ? 1'b1 : 1'b0) : ctrl_ub_buf_sel;
```

With unified_buffer.sv logic:
```systemverilog
rd_bank_sel = ub_buf_sel;      // Read from selected bank
wr_bank_sel = ~ub_buf_sel;     // Write to opposite bank
```

**For UART writes:**
- `test_ub_wr_en = 1` → `ub_buf_sel = 1`
- `wr_bank_sel = ~1 = 0` → Writes to **bank 0**

**For UART reads:**
- `test_ub_wr_en = 0` → `ub_buf_sel = 0`
- `rd_bank_sel = 0` → Reads from **bank 0**

**✅ Both access bank 0 - CORRECT!**

### 3. Check unified_buffer Module Instantiation

Verify in tpu_top.sv or basys3_test_interface.sv:
```systemverilog
unified_buffer ub_inst (
    .clk(clk),
    .rst_n(rst_n),
    .ub_buf_sel(ub_buf_sel),       // ← Dynamic control
    .ub_rd_en(ub_rd_en),
    .ub_rd_addr(ub_rd_addr),       // ← Must be 9-bit
    .ub_rd_count(ub_rd_count),     // ← Must be present
    .ub_rd_data(ub_rd_data),
    .ub_rd_valid(ub_rd_valid),     // ← Must be connected
    .ub_wr_en(ub_wr_en),
    .ub_wr_addr(ub_wr_addr),       // ← Must be 9-bit  
    .ub_wr_count(ub_wr_count),     // ← Must be present
    .ub_wr_data(ub_wr_data),
    .ub_wr_ready(ub_wr_ready),
    .ub_busy(ub_busy),
    .ub_done(ub_done)
);
```

---

## WHAT SHOULD BE TESTED

### Test 1: Loopback (Should work even in "broken" fef0361)
```python
tpu.write_ub(addr=0, data=bytes(range(32)))
result = tpu.read_ub(addr=0, length=32)
assert result == bytes(range(32))
```

### Test 2: Actual Memory Read (Fails in fef0361, should work now)
```python
# Write via UART
tpu.write_ub(addr=0, data=bytes(range(32)))

# Simulate TPU operation that modifies UB (or write to different address)
tpu.write_ub(addr=1, data=bytes(range(32, 64)))

# Read back the FIRST write
result = tpu.read_ub(addr=0, length=32)
# In fef0361: This returns bytes(32, 64) - the LAST write!
# After fix: Should return bytes(0, 32) - the ACTUAL data at addr 0
```

### Test 3: Partial Sum Read (The real test)
```python
# Load input data
tpu.write_ub(addr=0, data=input_matrix)
tpu.write_weights(addr=0, data=weights)

# Execute matmul
tpu.load_instruction(0, matmul_instr)
tpu.execute()
wait_for_done()

# Read partial sums from UB
result = tpu.read_ub(addr=result_addr, length=256)
# In fef0361: Returns garbage or last written data
# After fix: Should return actual computed matmul results
```

---

## RECOMMENDATION

The current code (after your fixes today) should be CORRECT. The issue is:

1. **fef0361 was a FALSE POSITIVE** - it never actually read from UB memory
2. **7a7acba tried to fix it** but had inconsistent 8/9-bit interface
3. **Your fixes (fc9e33d, dd48e12)** made the interface consistent

**To verify the fix works:**
1. Rebuild bitstream
2. Test loopback (should work)
3. Test multi-address read/write (proves it's not just loopback)
4. Test actual matmul with partial sum readback

If it still doesn't work, the issue is likely in:
- Bank selection timing
- READ_UB state machine initialization
- Unified buffer state machine (check unified_buffer.sv)

---

## KEY INSIGHT

**You were right:** Only minor tweaks to `ub_read` were needed for psum to work.

**But:** The "working" version (fef0361) wasn't actually reading from UB - it was just echoing back the last write! So it looked like everything worked, but partial sums (which require reading computed values) were never tested.

**The fix:** Add proper `ub_rd_valid` handshaking AND make interface consistent (all 9-bit).

Your current code should be correct now!

