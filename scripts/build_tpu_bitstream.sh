#!/bin/bash
# Complete Yosys + SymbiFlow Build Script for TPU on Basys 3

set -e

# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "Building TPU bitstream for Basys 3 using Yosys + SymbiFlow..."

# Change to project root
cd "$PROJECT_ROOT"

# Step 1: Synthesis with Yosys
echo "Step 1: Running Yosys synthesis..."
if [ -f "synthesis/yosys/synth_basys3.ys" ]; then
    yosys synthesis/yosys/synth_basys3.ys
elif [ -f "synth_basys3.ys" ]; then
    yosys synth_basys3.ys
else
    echo "✗ Error: synth_basys3.ys not found!"
    exit 1
fi

# Step 2: Place and Route with nextpnr-xilinx
echo "Step 2: Running nextpnr-xilinx place & route..."

# Try to find chipdb file in multiple locations
CHIPDB=""
if [ -f "$PROJECT_ROOT/nextpnr-xilinx/xilinx/xc7a35t.bin" ]; then
    CHIPDB="$PROJECT_ROOT/nextpnr-xilinx/xilinx/xc7a35t.bin"
elif [ -f "$PROJECT_ROOT/synthesis/nextpnr/xc7a35t.bin" ]; then
    CHIPDB="$PROJECT_ROOT/synthesis/nextpnr/xc7a35t.bin"
elif [ -f "/Users/abiralshakya/Downloads/diffTPU/xc7a35t.bin" ]; then
    CHIPDB="/Users/abiralshakya/Downloads/diffTPU/xc7a35t.bin"
elif [ -n "$CHIPDB_PATH" ]; then
    CHIPDB="$CHIPDB_PATH"
else
    echo "✗ Error: xc7a35t.bin chipdb file not found!"
    echo "  Please build it with: ./scripts/build_nextpnr_xilinx.sh"
    echo "  Or set CHIPDB_PATH environment variable"
    echo "  Or place xc7a35t.bin in one of:"
    echo "    - $PROJECT_ROOT/nextpnr-xilinx/xilinx/xc7a35t.bin"
    echo "    - $PROJECT_ROOT/synthesis/nextpnr/xc7a35t.bin"
    echo "    - /Users/abiralshakya/Downloads/diffTPU/xc7a35t.bin"
    exit 1
fi

echo "  Using chipdb: $CHIPDB"

# Find constraints file (prefer nextpnr-compatible version)
XDC_FILE=""
if [ -f "$PROJECT_ROOT/constraints/basys3_nextpnr.xdc" ]; then
    XDC_FILE="$PROJECT_ROOT/constraints/basys3_nextpnr.xdc"
    echo "  Using nextpnr-compatible constraints: $XDC_FILE"
elif [ -f "$PROJECT_ROOT/constraints/basys3.xdc" ]; then
    XDC_FILE="$PROJECT_ROOT/constraints/basys3.xdc"
    echo "  Warning: Using full XDC file (some constraints will be ignored by nextpnr-xilinx)"
elif [ -f "$PROJECT_ROOT/tpu_constraints.xdc" ]; then
    XDC_FILE="$PROJECT_ROOT/tpu_constraints.xdc"
else
    echo "✗ Error: Constraints file not found!"
    exit 1
fi

mkdir -p build

# Find nextpnr-xilinx binary
NEXTPNR_BIN=""
if [ -f "$PROJECT_ROOT/nextpnr-xilinx/build/nextpnr-xilinx" ]; then
    NEXTPNR_BIN="$PROJECT_ROOT/nextpnr-xilinx/build/nextpnr-xilinx"
elif command -v nextpnr-xilinx &> /dev/null; then
    NEXTPNR_BIN="nextpnr-xilinx"
else
    echo "✗ Error: nextpnr-xilinx not found!"
    echo "  Build it with: ./scripts/build_nextpnr_xilinx.sh"
    exit 1
fi

echo "  Using nextpnr-xilinx: $NEXTPNR_BIN"

$NEXTPNR_BIN \
    --chipdb "$CHIPDB" \
    --xdc "$XDC_FILE" \
    --json build/tpu_basys3.json \
    --fasm build/tpu_basys3.fasm \
    --force \
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
