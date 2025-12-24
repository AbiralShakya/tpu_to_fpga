# Open-Source FPGA Setup for TPU

## Completed Setup

### 1. Toolchain Verification
- Yosys: Already installed (`/opt/homebrew/bin/yosys`)
- Docker: Available for containerized tools
- openFPGALoader: Available for FPGA programming

### 2. Python Environment Setup
- Created virtual environment: `tpu_env/`
- Installed dependencies: `pyserial`, `numpy`
- Verified TPU driver import: `tpu_coprocessor_driver.py`

### 3. Synthesis Script Fixes
- Located main synthesis script: `tpu_to_fpga/synth_basys3.ys`
- Added missing modules:
  - `mmu.sv`
  - `activation_func.sv`
- Synthesis now includes all RTL files

### 4. Yosys Synthesis
- Command: `yosys synth_basys3.ys`
- Input: All SystemVerilog files in `rtl/`
- Output files:
  - `build/tpu_basys3.json` (12.5 MB)
  - `build/tpu.edif` (2.8 MB)
- Status: Successful compilation

### 5. UART Setup
- Detected UART devices:
  - `/dev/tty.usbserial-210183A27BE00` (JTAG)
  - `/dev/tty.usbserial-210183A27BE01` (UART)
- Verified Python serial communication
- Instruction encoding tested: NOP, MATMUL, HALT

## Remaining Steps

### Bitstream Generation
The design cannot be fully processed with open-source tools due to Xilinx DSP block support limitations.

#### Option 1: Adroit Cluster with TCL Script (Recommended)
The GUI EDIF import isn't working properly. Use the TCL script instead:

1. Transfer bitstream package to Adroit:
   ```
   scp bitstream_package.tar.gz as0714@adroit.princeton.edu:/scratch/network/as0714/
   ```

2. On Adroit, extract and run TCL script:
   ```bash
   cd /scratch/network/as0714
   tar -xzf bitstream_package.tar.gz
   cd bitstream_package
   module load vivado/2023.1
   vivado -mode batch -source import_edif.tcl
   ```

3. Check for the generated bitstream:
   ```bash
   ls -lh tpu_top.bit
   ```

4. Transfer back bitstream:
   ```
   scp as0714@adroit.princeton.edu:/scratch/network/as0714/bitstream_package/tpu_top.bit ./
   ```

#### Alternative: Vivado GUI (If TCL doesn't work)
If you want to try GUI anyway:
- Create RTL project
- Add EDIF as design source (it may not expand properly)
- Add constraints
- Generate bitstream
- Look for bitstream in project directory

#### Option 2: Docker Vivado
```
docker run -v $(pwd):/work xilinx/vivado:2023.1 \
  vivado -mode batch -source import_edif.tcl
```

### FPGA Programming
1. Connect Basys 3 board via USB
2. Program bitstream:
   ```
   openFPGALoader -b basys3 build/tpu_top.bit
   ```

### UART Testing
1. Activate virtual environment:
   ```
   source tpu_env/bin/activate
   ```

2. Run instruction tests:
   ```
   python3 test_all_instructions.py /dev/tty.usbserial-210183A27BE01
   ```

3. Run matrix multiplication test:
   ```
   python3 demo_tpu_complete.py /dev/tty.usbserial-210183A27BE01
   ```

## File Locations

### Synthesis Outputs
- `tpu_to_fpga/build/tpu_basys3.json`: Yosys JSON netlist
- `tpu_to_fpga/build/tpu.edif`: EDIF netlist for Vivado

### Scripts
- `tpu_to_fpga/synth_basys3.ys`: Yosys synthesis script
- `tpu_to_fpga/import_edif.tcl`: Vivado bitstream generation
- `upload_to_adroit.sh`: Transfer to Princeton Adroit

### Python Drivers
- `tpu_coprocessor_driver.py`: UART communication driver
- `instruction_encoder.py`: Instruction encoding
- `test_all_instructions.py`: Complete test suite

## Current Status

- Synthesis: Complete (100% open-source)
- Place & Route: Requires Vivado
- Bitstream Generation: Requires Vivado
- UART Testing: Ready
- FPGA Programming: Ready (openFPGALoader available)

## Notes

- Yosys synthesis successful with all modules included
- UART communication stack fully functional
- F4PGA tools not installed (would likely fail for DSP-intensive design)
- Minimal Vivado usage required for final bitstream generation
