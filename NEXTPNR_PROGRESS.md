# nextpnr-xilinx Build Progress

## ✅ Current Status

**Good News**: 
- ✅ **No VCC/GND cells** in the JSON (verified)
- ✅ **Placement progressing** - simulated annealing running
- ✅ **No VCC_WIRE errors** so far!

## What We Fixed

1. **Removed `vcc` signal** from `tpu_top_wrapper.sv` - it wasn't used and was causing VCC_WIRE routing issues
2. **Simplified Yosys script** - removed unnecessary `chtype` commands
3. **Fixed chipdb path** - using `nextpnr-xilinx/xilinx/xc7a35t.bin`

## Current Build

The build is currently running placement with simulated annealing. This can take 10-30 minutes.

**To check progress**:
```bash
tail -f build/nextpnr_retry.log
```

**To check if complete**:
```bash
ls -lh build/tpu_basys3.fasm
```

## If Build Completes Successfully

Once `build/tpu_basys3.fasm` is generated:

1. **Convert to bitstream** (if `fasm2bels` is available):
   ```bash
   fasm2bels --device xc7a35tcpg236-1 \
       --input build/tpu_basys3.fasm \
       --output build/tpu_basys3.bit
   ```

2. **Or use Vivado to convert FASM to bitstream**:
   ```bash
   vivado -mode batch -source scripts/fasm_to_bitstream.tcl
   ```

3. **Program FPGA**:
   ```bash
   openFPGALoader -b basys3 build/tpu_basys3.bit
   ```

## If Build Still Fails

If you still get VCC_WIRE errors or segfaults:

1. **Try with `--placer heap`** (if available):
   ```bash
   nextpnr-xilinx --chipdb ... --placer heap ...
   ```

2. **Use Vivado for P&R** (guaranteed to work):
   ```bash
   ./scripts/build_tpu_bitstream_vivado.sh
   ```

## Key Insight

The VCC_WIRE error was likely caused by the unused `vcc = 1'b1` signal in the wrapper. Removing it eliminated the VCC constant that nextpnr-xilinx was trying to route.

