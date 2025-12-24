# TPU Bitstream Generation on Adroit

## Files
- `tpu.edif`: Synthesized netlist from Yosys (open-source)
- `tpu_constraints.xdc`: FPGA constraints
- `import_edif.tcl`: Vivado script for automated bitstream generation

## Option 1: GUI Method (Since you have Vivado GUI)

### Step 1: Create New Project
1. Open Vivado GUI: `vivado &`
2. File → New Project
3. Project Name: `tpu_bitstream`
4. Project Location: `/scratch/network/as0714/bitstream_package`
5. Project Type: **RTL Project** (not "Do not specify sources")
6. Click Next

### Step 2: Select Device
1. Parts: Search for "xc7a35tcpg236-1" (Basys 3)
2. Select the part, click Next, then Finish

### Step 3: Add EDIF as Design Source
1. In **Sources** window (left panel), right-click **Design Sources**
2. Select **Add Sources**
3. Choose **Add or create design sources**
4. Click **Add Files**
5. Select `tpu.edif`
6. Click **OK** → **Finish**
7. When prompted, set **tpu_top** as the top module

### Step 4: Add Constraints
1. In Sources window, right-click **Constraints** → **Add Sources**
2. Add Files: Select `tpu_constraints.xdc`
3. Click OK

### Step 5: Generate Bitstream
1. In Flow Navigator: **PROGRAM AND DEBUG** → **Generate Bitstream**
2. Wait for completion (~5-10 minutes)
3. Bitstream location: `tpu_bitstream.runs/impl_1/tpu_top.bit`

## Option 2: TCL Script Method (Recommended)
This simplified script avoids Vivado project/run management issues:

```bash
# Load Vivado
module load vivado/2023.1

# Run bitstream generation
vivado -mode batch -source import_edif.tcl

# Check for output
ls -lh tpu_top.bit
```

The TCL script uses `read_edif` which properly imports the Yosys-generated EDIF file.

## Transfer Back to Mac
```bash
# From your Mac terminal:
scp as0714@adroit.princeton.edu:/scratch/network/as0714/bitstream_package/tpu_top.bit ./
```

## Result
- `tpu_top.bit`: Final bitstream file (~2-5 MB)
- Transfer back: `scp as0714@adroit.princeton.edu:/scratch/network/as0714/bitstream_package/tpu_top.bit ./`
- Program FPGA: `openFPGALoader -b basys3 tpu_top.bit`
