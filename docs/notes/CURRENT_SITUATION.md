# TPU Bank Selection Bug - Current Situation

## The Problem
The unified buffer has a bank selection bug where writes go to bank1 but reads come from bank0, causing all reads to return zeros.

**Fix needed:** Change line 70 in `rtl/unified_buffer.sv` from:
```systemverilog
wr_bank_sel = ~ub_buf_sel;  // BUGGY: opposite bank
```
to:
```systemverilog
wr_bank_sel = ub_buf_sel;  // FIXED: same bank
```

## The Catch-22

### Working Bitstreams (720KB) - Have Bank Bug
- `build/tpu_basys3-3.bit` - UART works perfectly ✅
- Returns status 0x20, reads return zeros ❌ (bank bug)
- Built with: **Yosys EDIF + Vivado P&R**

### Fixed Bitstreams (2.1MB) - Break UART Completely
- `build/tpu_basys3_fixed.bit` - NO UART response at all ❌
- Built with: **Vivado full synthesis**
- Something in Vivado synthesis breaks UART entirely

### Can't Build Fixed Bitstream with Yosys
- Yosys 0.60 fails with memory inference error:
  ```
  ERROR: Assert `port.en == State::S1' failed in kernel/mem.cc:492.
  ```
- This happens in the MEMORY_DFF pass during synth_xilinx
- Affects both old code (with bug) and new code (fixed)

## Why This is Happening

**Yosys Memory Inference:** Yosys expects memory write enables to be constant/unconditional. The unified_buffer has conditional writes inside a state machine:
```systemverilog
if (wr_bank_sel) begin
    memory_bank1[addr] <= data;  // Conditional write
end else begin
    memory_bank0[addr] <= data;  // Conditional write
end
```

**Vivado Synthesis:** When synthesizing the full RTL, Vivado appears to:
- Map memories to flip-flops instead of BRAM (2.1MB bitstream vs 720KB)
- Break UART functionality completely (no responses)
- Possibly optimize away critical logic or create timing violations

## Options

### Option 1: Use Old EDIF + Vivado P&R (Doesn't Fix Bug)
The old EDIF in `adroit_upload/tpu_basys3.edif` was built somehow (different Yosys version? different code?) and works. You could:
1. Upload old EDIF to Adroit
2. Run Vivado P&R only (no synthesis)
3. Get working bitstream BUT still has bank bug

### Option 2: Try Older Yosys Version
The old EDIF header shows it was built with Yosys 0.60 (same as current). But maybe:
- A previous Yosys version didn't have this strict assertion
- Try Yosys 0.40 or earlier

### Option 3: Investigate Vivado Synthesis Failure
Debug why Vivado synthesis breaks UART:
- Check synthesis warnings/errors in build log
- Look for optimized-away modules
- Check timing constraints
- Verify clock/reset signals

### Option 4: Restructure Memory Design (Complex)
Rewrite unified_buffer to avoid conditional memory writes:
- Use separate always blocks for each bank
- Use explicit write enables
- This is what I tried but it still fails

### Option 5: Manual EDIF Patching (Advanced)
Edit the EDIF netlist directly to fix the bank selection logic:
- Find the NOT gate inverting wr_bank_sel
- Remove it
- Extremely error-prone

## Recommendation

Can you check if there's a **Vivado build log** from when the working bitstreams were created?
Look for:
- `*.log` files in build/
- Synthesis reports showing how memories were mapped
- Any differences in Vivado version or settings

The fact that `tpu_basys3-3.bit` (720KB) works means there IS a way to build this design correctly. We just need to figure out what workflow was used.

## Quick Test
Load the old working bitstream to confirm hardware is OK:
```bash
openFPGALoader -b basys3 build/tpu_basys3-3.bit
python3 python/comprehensive_test.py
```
Should show: Status=0x20, reads return zeros (expected with bank bug)
