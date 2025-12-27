#!/bin/bash
# Build UART Test Bitstream for Basys3
# This creates a simple UART loopback design that actually responds to UART commands

set -e

echo "============================================"
echo "Building UART Test Bitstream for Basys3"
echo "============================================"
echo ""

# Check for Vivado
if ! command -v vivado &> /dev/null; then
    echo "✗ Vivado not found!"
    echo "  This script requires Vivado to build the bitstream"
    echo "  Alternative: Use the full TPU bitstream which has UART support"
    exit 1
fi

# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"

cd "$PROJECT_ROOT"

# Check if we're in the right directory
if [ ! -f "rtl/uart_test_simple.sv" ] && [ ! -f "vivado/build_uart_test.tcl" ]; then
    echo "✗ Error: Cannot find UART test source files!"
    echo "  Looking for: rtl/uart_test_simple.sv or vivado/build_uart_test.tcl"
    echo "  Current directory: $PROJECT_ROOT"
    exit 1
fi

echo "Step 1: Creating Vivado project..."

# Try to find build script
BUILD_SCRIPT=""
if [ -f "vivado/build_uart_test.tcl" ]; then
    BUILD_SCRIPT="vivado/build_uart_test.tcl"
elif [ -f "build_uart_test.tcl" ]; then
    BUILD_SCRIPT="build_uart_test.tcl"
else
    echo "✗ Error: build_uart_test.tcl not found!"
    exit 1
fi

vivado -mode batch -source "$BUILD_SCRIPT"

if [ $? -eq 0 ]; then
    BITSTREAM="build/uart_test/uart_test_project.runs/impl_1/uart_test_simple.bit"
    if [ -f "$BITSTREAM" ]; then
        echo ""
        echo "✓ Bitstream created successfully!"
        echo "  Location: $BITSTREAM"
        echo ""
        echo "Next steps:"
        echo "  1. Program FPGA:"
        echo "     openFPGALoader -b basys3 $BITSTREAM"
        echo ""
        echo "  2. Test UART:"
        echo "     python3 ../test_uart_simple.py /dev/tty.usbserial-XXXX"
        echo ""
        echo "  This design will echo back any byte you send!"
    else
        echo "✗ Bitstream file not found at expected location"
        exit 1
    fi
else
    echo "✗ Build failed"
    exit 1
fi

