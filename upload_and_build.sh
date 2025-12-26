#!/bin/bash
# Upload RTL and build with Vivado synthesis on Adroit

set -e

echo "========================================"
echo "TPU Vivado Synthesis Build"
echo "========================================"

# Upload files
echo -e "\n[1/3] Uploading RTL files to Adroit..."
scp -r rtl/ as0714@adroit.princeton.edu:~/tpu_build/
scp constraints/basys3_vivado.xdc as0714@adroit.princeton.edu:~/tpu_build/
scp vivado/build_with_vivado_synthesis.tcl as0714@adroit.princeton.edu:~/tpu_build/

# Build on Adroit
echo -e "\n[2/3] Running Vivado synthesis on Adroit..."
ssh as0714@adroit.princeton.edu << 'ENDSSH'
module load vivado/2023.1
cd ~/tpu_build
vivado -mode batch -source build_with_vivado_synthesis.tcl 2>&1 | tee vivado_build.log
ENDSSH

# Download bitstream
echo -e "\n[3/3] Downloading bitstream..."
scp as0714@adroit.princeton.edu:~/tpu_build/tpu_basys3_vivado_synth.bit build/

echo -e "\n========================================"
echo "✓ Build complete!"
echo "Bitstream: build/tpu_basys3_vivado_synth.bit"
echo "========================================"
echo ""
echo "To program FPGA:"
echo "  openFPGALoader -b basys3 build/tpu_basys3_vivado_synth.bit"
echo ""
echo "To test:"
echo "  python3 python/comprehensive_test.py"
echo "========================================"
