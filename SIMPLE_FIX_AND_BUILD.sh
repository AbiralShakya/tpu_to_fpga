#!/bin/bash
# Simple script to fix bank bug and build on Adroit

set -e

echo "========================================"
echo "TPU Bank Selection Fix - Vivado Build"
echo "========================================"
echo ""
echo "Uploading RTL with bank selection fix..."
scp -r rtl/ as0714@adroit.princeton.edu:~/tpu_build/
scp constraints/basys3_nextpnr.xdc as0714@adroit.princeton.edu:~/tpu_build/
scp vivado/build_rtl_vivado.tcl as0714@adroit.princeton.edu:~/tpu_build/

echo ""
echo "Building on Adroit with Vivado (this takes ~15 min)..."
ssh as0714@adroit.princeton.edu 'module load vivado/2023.1 && cd ~/tpu_build && vivado -mode batch -source build_rtl_vivado.tcl'

echo ""
echo "Downloading bitstream..."
scp as0714@adroit.princeton.edu:~/tpu_build/tpu_basys3_fixed.bit build/

echo ""
echo "========================================"
echo "✓ BUILD COMPLETE!"
echo "========================================"
echo ""
echo "Program FPGA:"
echo "  openFPGALoader -b basys3 build/tpu_basys3_fixed.bit"
echo ""
echo "Test:"
echo "  python3 python/comprehensive_test.py"
echo "========================================"
