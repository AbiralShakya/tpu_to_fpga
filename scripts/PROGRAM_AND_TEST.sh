#!/bin/bash
# Quick script to program FPGA and test with tpu_top_wrapper_verified_rev3.bit

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_ROOT"

BITSTREAM="build/tpu_top_wrapper_verified_rev3.bit"

echo "=================================================================================="
echo "Program FPGA and Test - tpu_top_wrapper_verified_rev3.bit"
echo "=================================================================================="
echo ""

# Check bitstream exists
if [ ! -f "$BITSTREAM" ]; then
    echo "✗ Error: Bitstream not found: $BITSTREAM"
    echo ""
    echo "Available bitstreams:"
    ls -lh build/*.bit 2>/dev/null || echo "  (none found)"
    exit 1
fi

echo "✓ Bitstream found: $BITSTREAM"
ls -lh "$BITSTREAM"
echo ""

# Program FPGA
echo "Programming FPGA..."
if command -v openFPGALoader &> /dev/null; then
    openFPGALoader -b basys3 "$BITSTREAM"
    echo "✓ FPGA programmed"
else
    echo "✗ Error: openFPGALoader not found"
    echo "  Install with: brew install openfpgaloader"
    exit 1
fi

echo ""

# Find UART port
echo "Finding UART port..."
UART_PORTS=$(ls /dev/tty.usbserial-* 2>/dev/null || true)

if [ -z "$UART_PORTS" ]; then
    echo "✗ No UART devices found!"
    echo "  Make sure Basys3 is connected via USB"
    exit 1
fi

# Count ports
PORT_COUNT=$(echo "$UART_PORTS" | wc -l | tr -d ' ')

if [ "$PORT_COUNT" -eq 1 ]; then
    UART_PORT="$UART_PORTS"
    echo "✓ Found UART: $UART_PORT"
else
    # Use the last one (usually UART, first is JTAG)
    UART_PORT=$(echo "$UART_PORTS" | tail -1)
    echo "✓ Found $PORT_COUNT devices, using UART: $UART_PORT"
    echo "  (All devices: $UART_PORTS)"
fi

echo ""
echo "=================================================================================="
echo "Testing Options:"
echo "=================================================================================="
echo ""
echo "1. Basic UART connectivity test:"
echo "   python3 python/test_uart_basic.py $UART_PORT"
echo ""
echo "2. Legacy mode ISA test:"
echo "   python3 python/test_isa_legacy.py $UART_PORT"
echo ""
echo "3. Legacy mode with verification:"
echo "   python3 python/test_isa_legacy_verified.py $UART_PORT"
echo ""
echo "4. Streaming mode (if supported):"
echo "   python3 python/test_all_instructions.py $UART_PORT"
echo ""
echo "=================================================================================="
echo "Quick test (basic connectivity):"
echo "=================================================================================="
echo ""

if [ -f "python/test_uart_basic.py" ]; then
    python3 python/test_uart_basic.py "$UART_PORT"
else
    echo "⚠ Test script not found: python/test_uart_basic.py"
fi

echo ""
echo "=================================================================================="
echo "Done! Use the commands above to run more tests."
echo "=================================================================================="

