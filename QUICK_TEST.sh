#!/bin/bash
# Quick script to download bitstream and test ISA

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_ROOT"

echo "=========================================="
echo "TPU ISA Testing - Quick Start"
echo "=========================================="
echo ""

# Step 1: Download bitstream
echo "Step 1: Downloading bitstream from Adroit..."
if [ ! -f "build/tpu_top_wrapper.bit" ]; then
    echo "Downloading from Adroit..."
    scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper.bit build/
    echo "✓ Downloaded"
else
    echo "✓ Bitstream already exists: build/tpu_top_wrapper.bit"
fi

# Step 2: Program FPGA
echo ""
echo "Step 2: Programming FPGA..."
if command -v openFPGALoader &> /dev/null; then
    openFPGALoader -b basys3 build/tpu_top_wrapper.bit
    echo "✓ FPGA programmed"
else
    echo "⚠ openFPGALoader not found - skipping programming"
    echo "  Install with: brew install openfpgaloader"
fi

# Step 3: Find UART port
echo ""
echo "Step 3: Finding UART port..."
UART_PORTS=($(ls /dev/tty.usbserial-* 2>/dev/null || true))

if [ ${#UART_PORTS[@]} -eq 0 ]; then
    echo "✗ No UART devices found!"
    echo "  Make sure Basys3 is connected via USB"
    exit 1
elif [ ${#UART_PORTS[@]} -eq 1 ]; then
    UART_PORT="${UART_PORTS[0]}"
    echo "✓ Found UART: $UART_PORT"
else
    # Use the last one (usually UART, first is JTAG)
    UART_PORT="${UART_PORTS[-1]}"
    echo "✓ Found ${#UART_PORTS[@]} devices, using UART: $UART_PORT"
    echo "  (Other devices: ${UART_PORTS[@]})"
fi

# Step 4: Test ISA
echo ""
echo "Step 4: Testing ISA instructions..."
echo "=========================================="
echo ""

if [ -f "python/test_all_instructions.py" ]; then
    python3 python/test_all_instructions.py "$UART_PORT"
else
    echo "✗ Test script not found: python/test_all_instructions.py"
    echo ""
    echo "Manual test:"
    echo "  python3 python/drivers/tpu_driver.py $UART_PORT"
fi

echo ""
echo "=========================================="
echo "Done!"
echo "=========================================="

