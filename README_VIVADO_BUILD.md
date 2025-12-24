# ✅ Vivado Build Setup Complete!

## Current Status

✅ **EDIF File Generated**: `build/tpu_basys3.edif` (1.4 MB)
✅ **Constraints Ready**: `constraints/basys3_nextpnr.xdc` (6 KB)
✅ **Vivado Script Ready**: `vivado/build_tpu_vivado.tcl` (2 KB)

## Next Step: Run Vivado

### Option 1: Automated Script (Easiest)

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga
./scripts/build_tpu_bitstream_vivado.sh
```

### Option 2: Direct Vivado Command

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga
vivado -mode batch -source vivado/build_tpu_vivado.tcl
```

**Expected Time**: 15-35 minutes

**Output**: `build/tpu_top_wrapper.bit`

## What Happens

1. **Vivado reads EDIF**: Imports `build/tpu_basys3.edif`
2. **Vivado reads constraints**: Applies `constraints/basys3_nextpnr.xdc`
3. **Vivado runs P&R**:
   - opt_design (optimization)
   - place_design (placement)
   - route_design (routing)
4. **Vivado generates bitstream**: `build/tpu_top_wrapper.bit`

## Files You Need (Already Ready!)

| File | Size | Purpose |
|------|------|---------|
| `build/tpu_basys3.edif` | 1.4 MB | Synthesized netlist (from Yosys) |
| `constraints/basys3_nextpnr.xdc` | 6 KB | Pin locations & IOSTANDARD |
| `vivado/build_tpu_vivado.tcl` | 2 KB | Vivado automation script |

**Total**: ~1.4 MB (very small!)

## If Vivado is Not in PATH

### macOS (if installed locally):
```bash
# Find Vivado installation
find /Applications -name "vivado" -type f 2>/dev/null

# Or set PATH manually
export PATH=/Applications/Xilinx/Vivado/2023.1/bin:$PATH
```

### Linux/Cluster:
```bash
module load vivado/2023.1
# Or your system's equivalent
```

## After Build Completes

```bash
# Program FPGA
openFPGALoader -b basys3 build/tpu_top_wrapper.bit

# Test UART
python3 python/drivers/tpu_driver.py /dev/tty.usbserial-*
```

## Troubleshooting

**"Vivado not found"**:
- Install Vivado or load module
- Check: `which vivado`

**"EDIF not found"**:
- Already generated! Check: `ls -lh build/tpu_basys3.edif`
- If missing, run: `yosys synthesis/yosys/synth_basys3_vivado.ys`

**Build takes long**:
- Normal! 15-35 minutes is expected
- Check progress in `vivado/build.log`

## Summary

**You're ready to build!** Just run:

```bash
vivado -mode batch -source vivado/build_tpu_vivado.tcl
```

Or use the convenience script:

```bash
./scripts/build_tpu_bitstream_vivado.sh
```

The EDIF file is already generated and ready to go! 🚀

