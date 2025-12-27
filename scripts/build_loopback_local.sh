#!/bin/bash
# Build UART loopback test locally with nextpnr-xilinx

set -e

PROJECT_ROOT="/Users/abiralshakya/Documents/tpu_to_fpga"
cd "$PROJECT_ROOT"

echo "=========================================="
echo "Building UART Loopback Test Locally"
echo "=========================================="

# Step 1: Yosys synthesis
echo "Step 1: Running Yosys synthesis..."
yosys synthesis/yosys/synth_loopback_vivado.ys

if [ ! -f "build/uart_loopback.json" ]; then
    echo "ERROR: JSON not generated!"
    exit 1
fi
echo "✓ JSON generated: build/uart_loopback.json"

# Step 3: Find chipdb
CHIPDB=""
if [ -f "$PROJECT_ROOT/nextpnr-xilinx/xilinx/xc7a35t.bin" ]; then
    CHIPDB="$PROJECT_ROOT/nextpnr-xilinx/xilinx/xc7a35t.bin"
elif [ -f "/Users/abiralshakya/Downloads/diffTPU/xc7a35t.bin" ]; then
    CHIPDB="/Users/abiralshakya/Downloads/diffTPU/xc7a35t.bin"
else
    echo "ERROR: xc7a35t.bin chipdb not found!"
    echo "Please download it or specify path"
    exit 1
fi
echo "Using chipdb: $CHIPDB"

# Step 4: Find nextpnr-xilinx
NEXTPNR=""
if [ -f "$PROJECT_ROOT/nextpnr-xilinx/build/nextpnr-xilinx" ]; then
    NEXTPNR="$PROJECT_ROOT/nextpnr-xilinx/build/nextpnr-xilinx"
elif command -v nextpnr-xilinx &> /dev/null; then
    NEXTPNR="nextpnr-xilinx"
else
    echo "ERROR: nextpnr-xilinx not found!"
    exit 1
fi
echo "Using nextpnr: $NEXTPNR"

# Step 5: Place and route with nextpnr-xilinx
echo "Step 3: Running nextpnr-xilinx place & route..."
$NEXTPNR \
    --chipdb "$CHIPDB" \
    --xdc constraints/uart_loopback.xdc \
    --json build/uart_loopback.json \
    --fasm build/uart_loopback.fasm \
    --write build/uart_loopback_routed.json \
    --ignore-loops \
    --force

if [ ! -f "build/uart_loopback.fasm" ]; then
    echo "ERROR: FASM not generated!"
    exit 1
fi
echo "✓ FASM generated: build/uart_loopback.fasm"

# Step 6: Convert FASM to bitstream
echo "Step 4: Converting FASM to bitstream..."

# Check if we have pypy3 (faster) or python3
if command -v pypy3 &> /dev/null; then
    PYTHON=pypy3
else
    PYTHON=python3
fi

# Use prjxray-tools fasm2frames and xc7frames2bit
if command -v fasm2frames &> /dev/null; then
    echo "Using prjxray-tools..."
    fasm2frames build/uart_loopback.fasm > build/uart_loopback.frames
    xc7frames2bit \
        --part xc7a35tcpg236-1 \
        --frm_file build/uart_loopback.frames \
        --output_file build/uart_loopback.bit
else
    echo "ERROR: prjxray-tools not found!"
    echo "Install with: pip install prjxray-tools"
    exit 1
fi

if [ ! -f "build/uart_loopback.bit" ]; then
    echo "ERROR: Bitstream not generated!"
    exit 1
fi

echo ""
echo "=========================================="
echo "✓ BUILD SUCCESSFUL!"
echo "=========================================="
echo "Bitstream: build/uart_loopback.bit"
echo ""
echo "To program FPGA:"
echo "  1. Make sure SW15 (reset) is UP"
echo "  2. openFPGALoader -b basys3 build/uart_loopback.bit"
echo "  3. python3 -c \"import serial; s=serial.Serial('/dev/tty.usbserial-210183A27BE01',115200,timeout=1); s.write(b'\\x06'); print('Sent 0x06, got:', hex(ord(s.read(1))) if s.read(1) else 'NOTHING')\""
echo "=========================================="
