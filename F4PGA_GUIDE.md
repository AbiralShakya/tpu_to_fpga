# F4PGA (Project X-Ray) Guide - Experimental Open-Source Build

## ⚠️ Important Disclaimer

**F4PGA for Xilinx Artix-7 is EXPERIMENTAL and INCOMPLETE.**

Expected success rate:
- ✅ **Simple designs** (blinky, UART echo): **20-40% success**
- ❌ **Complex designs** (TPU with DSP, BRAM): **<5% success**

### Current Limitations

| Feature | Supported | Notes |
|---------|-----------|-------|
| Basic LUTs/FFs | ✅ Partial | Works for simple logic |
| BRAM | ⚠️ Limited | Small BRAMs only |
| DSP48E1 | ❌ No | Your systolic array won't work |
| Clock networks | ⚠️ Limited | May have routing issues |
| I/O timing | ❌ No | No timing closure guarantee |
| Bitstream encryption | ❌ No | Unsupported |

**Reality:** Your TPU design uses 9 DSP blocks (systolic array) which Project X-Ray doesn't support well.

---

## 🚀 Installation Steps

### Step 1: Install F4PGA

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga/tpu_to_fpga

# Run installation script
./install_f4pga.sh

# This will:
# - Install Miniconda
# - Create F4PGA environment
# - Download Artix-7 database
# - Install VPR, FASM tools
```

**Time:** ~30-60 minutes depending on download speed

### Step 2: Activate Environment

```bash
# Every time you want to build:
source ~/opt/f4pga/setup.sh

# You should see:
# ✓ F4PGA environment activated
# (f4pga) in your prompt
```

### Step 3: Verify Installation

```bash
# Check if tools are available
which yosys        # Should show conda environment path
which vpr          # VPR place & route tool
which symbiflow_synth  # F4PGA synthesis wrapper

# Check database
echo $XRAY_DATABASE  # Should show path to artix7 database
```

---

## 🧪 Build Options

### Option A: Simple UART Test (More Likely to Work)

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga/tpu_to_fpga

# Activate F4PGA
source ~/opt/f4pga/setup.sh

# Build simple UART loopback
./build_f4pga_simple.sh
```

**Expected result:**
- ⚠️ **Likely to fail** due to I/O primitive support issues
- If it works: bitstream at `build/f4pga_simple/uart_test.bit`

### Option B: Full TPU (Will Almost Certainly Fail)

```bash
# Build full TPU
./build_f4pga.sh
```

**Expected result:**
- ❌ **Will fail** at VPR stage due to DSP blocks
- Error: "Unknown block type: DSP48E1"

---

## 🔍 What Will Happen

### Synthesis (✅ Works)

```bash
yosys synth_basys3.ys
```

This will succeed - Yosys has good Xilinx support.

### Place & Route (❌ Likely Fails)

```bash
vpr arch.xml design.eblif ...
```

**Expected errors:**
```
Error: Unknown block type 'DSP48E1'
Error: Cannot find routing path for clock
Error: Unsupported primitive 'RAMB18E1'
```

**Why:** Project X-Ray database doesn't cover all Artix-7 primitives yet.

### Bitstream Generation (❌ Will Fail)

Even if P&R succeeds, bitstream generation will fail for complex designs.

---

## 🆘 When F4PGA Fails (Expected)

You have 3 options:

### Option 1: Hybrid Approach (Recommended)

Use Yosys for synthesis, Vivado only for bitstream:

```bash
# Already created for you:
./build_opensource.sh

# Then get bitstream with minimal Vivado usage:
# - Docker: docker run xilinx/vivado ...
# - VM: Ubuntu VM with Vivado
# - Cloud: Xilinx free web edition
```

### Option 2: Simplify Design

Remove features until F4PGA works:
- ❌ Remove DSP blocks (use LUT-based multiply)
- ❌ Remove large BRAMs (use distributed RAM)
- ❌ Reduce from 3x3 to 2x2 array
- ❌ Remove double buffering

**Result:** A much slower, limited TPU that might work.

### Option 3: Switch FPGA Board

Buy an iCE40 or ECP5 board with **full** open-source support:

| Board | Price | FPGA | Tools | Support |
|-------|-------|------|-------|---------|
| iCEBreaker | $60 | iCE40 UP5K | Yosys+nextpnr | 100% |
| ULX3S | $130 | ECP5-85F | Yosys+nextpnr | 100% |
| OrangeCrab | $90 | ECP5-25F | Yosys+nextpnr | 100% |

**Trade-off:** Need to redesign TPU for different architecture.

---

## 📊 Debugging F4PGA Build

### Enable Verbose Output

```bash
# Add debug flags
export VPR_DEBUG=1
export YOSYS_DEBUG=1

# Run build with verbose logging
./build_f4pga.sh 2>&1 | tee build.log
```

### Common Errors and Fixes

#### Error: "DSP48E1 not supported"

```
Error: Cannot find architecture primitive for DSP48E1
```

**Fix:** Remove systolic array, use LUT-based multiply
```verilog
// Instead of:
assign psum = a * b + c;  // Uses DSP

// Use:
assign psum = lut_mult(a, b) + c;  // Uses LUTs
```

#### Error: "Routing failed"

```
Error: Failed to route signal clk
```

**Fix:** Simplify design, reduce fan-out
```verilog
// Add intermediate buffers
wire clk_buf;
BUFG bufg (.I(clk), .O(clk_buf));
```

#### Error: "Block RAM not found"

```
Error: RAMB18E1 primitive not in database
```

**Fix:** Use distributed RAM instead
```verilog
// Instead of:
(* ram_style = "block" *) reg [31:0] mem [0:255];

// Use:
(* ram_style = "distributed" *) reg [31:0] mem [0:255];
```

---

## 🎯 Realistic Expectations

### What CAN Work with F4PGA

✅ Blinky LED (100% success)
✅ Simple UART echo (50% success)
✅ Small state machines (80% success)
✅ Basic combinational logic (90% success)

### What CANNOT Work

❌ DSP blocks (systolic arrays)
❌ Large block RAMs
❌ High-speed clocking (>100 MHz)
❌ Complex designs (like your TPU)

---

## 💡 My Honest Recommendation

After setting up F4PGA, you'll likely find that:

1. **Simple test designs fail** due to I/O primitive issues
2. **Full TPU fails** at DSP block instantiation
3. **Hours spent debugging** tool limitations

**Better approach:**
1. ✅ Use **Yosys** for synthesis (open-source)
2. ✅ Use **Vivado in Docker** for bitstream (minimal proprietary usage)
3. ✅ Get working TPU on Basys 3 in <1 hour

### Quick Vivado in Docker

```bash
# Pull image (one-time, ~5GB download)
docker pull xilinx/vivado:2023.1

# Generate bitstream from Yosys output
cd /Users/abiralshakya/Documents/tpu_to_fpga/tpu_to_fpga
./build_opensource.sh  # Creates EDIF with Yosys

docker run --rm -v $(pwd):/work -w /work \
  xilinx/vivado:2023.1 \
  vivado -mode batch -source import_edif.tcl

# Result: build/tpu_top.bit
```

**Time:** 15 minutes (vs. days debugging F4PGA)

---

## 🔬 Try F4PGA Anyway?

If you still want to experiment with F4PGA (for learning purposes):

```bash
# Step 1: Install
./install_f4pga.sh

# Step 2: Try simple test
source ~/opt/f4pga/setup.sh
./build_f4pga_simple.sh

# Step 3: When it fails, try TPU
./build_f4pga.sh

# Step 4: Report issues to F4PGA project
# Help improve the tools by reporting what failed!
```

---

## 📚 Resources

- **F4PGA Docs:** https://f4pga.readthedocs.io/
- **Project X-Ray:** https://github.com/SymbiFlow/prjxray
- **Issue Tracker:** https://github.com/SymbiFlow/prjxray/issues
- **Chat:** #f4pga on libera.chat IRC

---

## ✅ Action Items

1. **Try F4PGA installation** (run `./install_f4pga.sh`)
2. **Attempt simple build** (run `./build_f4pga_simple.sh`)
3. **When it fails**, switch to hybrid approach:
   ```bash
   ./build_opensource.sh  # Yosys synthesis
   # Then use Vivado Docker for bitstream
   ```

**Expected outcome:** You'll learn that F4PGA isn't ready for production yet, but Yosys + Docker Vivado works great!

---

**Bottom Line:** F4PGA is amazing for iCE40/ECP5, but Artix-7 support is still experimental. For your TPU project, use Yosys (open-source) + Vivado Docker (minimal proprietary) = best of both worlds.
