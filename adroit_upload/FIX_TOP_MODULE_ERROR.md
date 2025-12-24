# Fix: "No files found to match top module 'tpu_top_wrapper'"

## Problem

Vivado's `read_edif` command expects the EDIF filename to match the top module name, OR you need to specify it differently.

**Error**: `ERROR: [Project 1-68] No files found to match top module 'tpu_top_wrapper'`

**Cause**: EDIF file is named `tpu_basys3.edif` but top module is `tpu_top_wrapper`

## Solution 1: Rename EDIF File (Easiest)

On Adroit:
```bash
cd /home/as0714/tpu_build/adroit_upload
cp tpu_basys3.edif tpu_top_wrapper.edif
```

Then in Vivado TCL Console:
```tcl
cd /home/as0714/tpu_build/adroit_upload
read_edif tpu_top_wrapper.edif
set_property top tpu_top_wrapper [current_fileset]
read_xdc basys3_nextpnr.xdc
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper
opt_design
place_design
route_design
write_bitstream -force tpu_top_wrapper.bit
```

## Solution 2: Use read_edif with -top (Alternative)

Try specifying the top module explicitly:
```tcl
cd /home/as0714/tpu_build/adroit_upload
read_edif -top tpu_top_wrapper tpu_basys3.edif
read_xdc basys3_nextpnr.xdc
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper
opt_design
place_design
route_design
write_bitstream -force tpu_top_wrapper.bit
```

## Solution 3: Check Actual Top Module Name

First, check what module name is actually in the EDIF:
```bash
# On Adroit
grep -i "cell\|library" tpu_basys3.edif | head -20
```

Then use that name instead of `tpu_top_wrapper`.

## Quick Fix (Copy-Paste)

**On Adroit terminal**:
```bash
cd /home/as0714/tpu_build/adroit_upload
cp tpu_basys3.edif tpu_top_wrapper.edif
```

**In Vivado TCL Console**:
```tcl
cd /home/as0714/tpu_build/adroit_upload
read_edif tpu_top_wrapper.edif
set_property top tpu_top_wrapper [current_fileset]
read_xdc basys3_nextpnr.xdc
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper
opt_design
place_design
route_design
write_bitstream -force tpu_top_wrapper.bit
```

This should work!

