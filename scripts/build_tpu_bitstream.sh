#!/bin/bash
# Complete Yosys + SymbiFlow Build Script for TPU on Basys 3

set -e

echo "Building TPU bitstream for Basys 3 using Yosys + SymbiFlow..."

# Step 1: Synthesis with Yosys
echo "Step 1: Running Yosys synthesis..."
cd /Users/abiralshakya/Documents/tpu_to_fpga
yosys synth_basys3.ys

# Step 2: Place and Route with nextpnr-xilinx
echo "Step 2: Running nextpnr-xilinx place & route..."
nextpnr-xilinx \
    --chipdb /Users/abiralshakya/Downloads/diffTPU/xc7a35t.bin \
    --xdc tpu_constraints.xdc \
    --json build/tpu_basys3.json \
    --fasm build/tpu_basys3.fasm \
    --verbose

# Step 3: Convert FASM to bitstream
echo "Step 3: Converting FASM to bitstream..."
fasm2bels \
    --device xc7a35tcpg236-1 \
    --input build/tpu_basys3.fasm \
    --output build/tpu_basys3.bit

echo "Bitstream generated: build/tpu_basys3.bit"

# Step 4: Program the FPGA
echo "Step 4: Programming Basys 3 FPGA..."
openFPGALoader -b basys3 build/tpu_basys3.bit

echo "TPU successfully programmed to Basys 3 FPGA!"
echo "Use switches and buttons to test your 3x3 systolic array TPU!"
