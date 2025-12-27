#!/bin/bash
# Quick script to rebuild bitstream with streaming mode support

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_ROOT"

echo "=========================================="
echo "Rebuilding TPU with Streaming Mode"
echo "=========================================="
echo ""

# Step 1: Regenerate EDIF
echo "Step 1: Regenerating EDIF with improved UART DMA..."
yosys synthesis/yosys/synth_basys3_vivado.ys

if [ ! -f "build/tpu_basys3.edif" ]; then
    echo "✗ Error: EDIF generation failed!"
    exit 1
fi

echo "✓ EDIF generated: build/tpu_basys3.edif"
echo ""

# Step 2: Upload to Adroit
echo "Step 2: Uploading to Adroit..."
echo "Run this command:"
echo "  scp build/tpu_basys3.edif as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/"
echo ""

# Step 3: Instructions for Adroit
echo "Step 3: On Adroit, run in Vivado TCL Console:"
echo "  cd /home/as0714/tpu_build/adroit_upload"
echo "  cp tpu_basys3.edif tpu_top_wrapper.edif"
echo "  read_edif tpu_top_wrapper.edif"
echo "  set_property top tpu_top_wrapper [current_fileset]"
echo "  read_xdc basys3_nextpnr.xdc"
echo "  link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper"
echo "  opt_design"
echo "  place_design"
echo "  route_design"
echo "  write_bitstream -force tpu_top_wrapper.bit"
echo ""

# Step 4: Download
echo "Step 4: Download bitstream:"
echo "  scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper.bit build/"
echo ""

# Step 5: Program and test
echo "Step 5: Program FPGA and test:"
echo "  openFPGALoader -b basys3 build/tpu_top_wrapper.bit"
echo "  python3 python/test_all_instructions.py /dev/tty.usbserial-XXXXX"
echo ""

echo "=========================================="
echo "✓ EDIF ready for upload!"
echo "=========================================="

