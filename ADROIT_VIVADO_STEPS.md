# Quick Steps to Run Vivado on Adroit

## Current Status
✅ Package extracted to: `/home/as0714/tpu_build/vivado_package/`
✅ All required files present:
   - `tpu.edif` (EDIF netlist)
   - `tpu_constraints.xdc` (constraints)
   - `vivado_import_edif.tcl` (build script)

## Next Steps

### Option 1: Batch Mode (Recommended - No GUI needed)

```bash
# Make sure you're in the right directory
cd /home/as0714/tpu_build/vivado_package

# Load Vivado module
module load vivado/2023.1

# Verify Vivado is loaded
which vivado
vivado -version

# Run the build script (takes 15-30 minutes)
vivado -mode batch -source vivado_import_edif.tcl

# This will:
# 1. Import EDIF netlist
# 2. Read constraints
# 3. Run opt_design (should pass now with the fix!)
# 4. Run place_design
# 5. Run route_design
# 6. Generate bitstream
```

### Option 2: Visual Vivado (GUI - requires X11)

```bash
# Make sure you're in the right directory
cd /home/as0714/tpu_build/vivado_package

# Load Vivado module
module load vivado/2023.1

# Launch Vivado GUI (requires X11 forwarding)
vivado &

# Wait 30-60 seconds for GUI to open
# Then in Vivado:
# 1. Tools → Run Tcl Script...
# 2. Select: vivado_import_edif.tcl
# 3. Click OK
```

## What to Expect

### During Build:
- **opt_design**: Should complete without the multiple driver error
- **place_design**: Takes 5-10 minutes
- **route_design**: Takes 5-10 minutes
- **write_bitstream**: Takes 1-2 minutes

### Success Indicators:
```
✓ Optimization complete
✓ Placement complete
✓ Routing complete
✓ Timing constraints met
✓ BUILD COMPLETE!
```

### Output Location:
```
/home/as0714/tpu_build/vivado_package/build/tpu_top.bit
```

## Check Build Results

```bash
# After build completes, check for bitstream
ls -lh build/tpu_top.bit

# Should show: ~2-3 MB file
# Example: -rw-r--r-- 1 as0714 users 2.1M Dec 23 20:15 build/tpu_top.bit
```

## If Build Fails

### Check Logs:
```bash
# Check Vivado log
cat build/vivado_project/vivado.log | tail -50

# Check for errors
grep -i error build/vivado_project/vivado.log | tail -20
```

### Common Issues:

1. **Still getting multiple driver error?**
   - The package might be from before the fix
   - Need to re-synthesize with fixed `instruction_fifo.sv`

2. **Timing violations?**
   - Check: `cat build/timing_final.rpt`
   - WNS (Worst Negative Slack) should be ≥ 0

3. **Resource overflow?**
   - Check: `cat build/utilization_final.rpt`
   - Design might be too large for Basys 3

## Download Bitstream to Mac

After successful build:

```bash
# From Mac terminal (new window)
scp as0714@adroit.princeton.edu:/home/as0714/tpu_build/vivado_package/build/tpu_top.bit \
    /Users/abiralshakya/Documents/tpu_to_fpga/build/
```

## Quick Reference

```bash
# Current directory
pwd
# Should show: /home/as0714/tpu_build/vivado_package

# Files present
ls -lh

# Run build
module load vivado/2023.1
vivado -mode batch -source vivado_import_edif.tcl

# Check result
ls -lh build/tpu_top.bit
```

---

**You're all set! Run the batch mode command above to start the build.**

