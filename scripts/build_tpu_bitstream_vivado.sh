#!/bin/bash
# Build TPU bitstream using Yosys + Vivado (instead of nextpnr-xilinx)
# This avoids VCC_WIRE/GND_WIRE errors

set -e

# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "Building TPU bitstream for Basys 3 using Yosys + Vivado..."
echo "This avoids nextpnr-xilinx VCC_WIRE/GND_WIRE errors"

# Change to project root
cd "$PROJECT_ROOT"

# Step 1: Synthesis with Yosys (generate EDIF)
echo "Step 1: Running Yosys synthesis (generating EDIF)..."
if [ -f "synthesis/yosys/synth_basys3_vivado.ys" ]; then
    yosys synthesis/yosys/synth_basys3_vivado.ys
elif [ -f "synth_basys3_vivado.ys" ]; then
    yosys synth_basys3_vivado.ys
else
    echo "✗ Error: synth_basys3_vivado.ys not found!"
    exit 1
fi

# Check if EDIF was generated
if [ ! -f "build/tpu_basys3.edif" ]; then
    echo "✗ Error: EDIF file not generated!"
    exit 1
fi

echo "✓ EDIF generated: build/tpu_basys3.edif"

# Step 2: Place and Route with Vivado
echo "Step 2: Running Vivado place & route..."

# Check if Vivado is available
if ! command -v vivado &> /dev/null; then
    echo "✗ Error: Vivado not found!"
    echo "  Please ensure Vivado is installed and in your PATH"
    echo "  Or load Vivado module: module load vivado/2023.1"
    exit 1
fi

# Check constraints file
XDC_FILE=""
if [ -f "$PROJECT_ROOT/constraints/basys3_nextpnr.xdc" ]; then
    XDC_FILE="$PROJECT_ROOT/constraints/basys3_nextpnr.xdc"
    echo "  Using constraints: $XDC_FILE"
elif [ -f "$PROJECT_ROOT/constraints/basys3.xdc" ]; then
    XDC_FILE="$PROJECT_ROOT/constraints/basys3.xdc"
    echo "  Using constraints: $XDC_FILE"
elif [ -f "$PROJECT_ROOT/simple_test.xdc" ]; then
    XDC_FILE="$PROJECT_ROOT/simple_test.xdc"
    echo "  Using constraints: $XDC_FILE"
else
    echo "✗ Error: Constraints file not found!"
    exit 1
fi

# Run Vivado in batch mode
vivado -mode batch -source "$PROJECT_ROOT/vivado/build_tpu_vivado.tcl" -log vivado/build.log

# Check if bitstream was generated
BITSTREAM="build/tpu_top_wrapper.bit"
if [ -f "$BITSTREAM" ]; then
    echo ""
    echo "✓ Bitstream generated: $BITSTREAM"
    echo ""
    echo "To program the FPGA:"
    echo "  openFPGALoader -b basys3 $BITSTREAM"
else
    echo "✗ Error: Bitstream not found!"
    echo "  Expected location: $BITSTREAM"
    echo "  Check vivado/build.log for errors"
    exit 1
fi

echo ""
echo "TPU successfully built using Vivado!"

