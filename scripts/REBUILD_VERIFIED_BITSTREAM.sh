#!/bin/bash
# Rebuild bitstream with full verification to ensure UART DMA is included

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_ROOT"

echo "=================================================================================="
echo "Rebuilding TPU Bitstream with UART DMA Verification"
echo "=================================================================================="
echo ""

# Step 1: Generate EDIF
echo "Step 1: Generating EDIF with Yosys..."
yosys synthesis/yosys/synth_basys3_vivado.ys

if [ ! -f "build/tpu_basys3.edif" ]; then
    echo "✗ Error: EDIF generation failed!"
    exit 1
fi

echo "✓ EDIF generated: build/tpu_basys3.edif"
echo ""

# Step 2: Verify UART DMA is in EDIF
echo "Step 2: Verifying UART DMA module is in EDIF..."
if grep -qi "uart_dma\|uart_rx\|uart_tx" build/tpu_basys3.edif; then
    echo "✓ UART-related modules found in EDIF"
    grep -i "uart" build/tpu_basys3.edif | head -5
else
    echo "⚠ Warning: No UART modules found in EDIF!"
    echo "  This suggests UART DMA might not be included"
fi
echo ""

# Step 3: Verify constraints file exists
echo "Step 3: Verifying constraints file..."
if [ ! -f "constraints/basys3_nextpnr.xdc" ]; then
    echo "✗ Error: constraints/basys3_nextpnr.xdc not found!"
    exit 1
fi

if grep -q "uart_tx\|uart_rx\|B18\|A18" constraints/basys3_nextpnr.xdc; then
    echo "✓ UART pin constraints found in XDC file"
    grep -E "uart_tx|uart_rx|B18|A18" constraints/basys3_nextpnr.xdc
else
    echo "✗ Error: UART pin constraints NOT found in XDC file!"
    exit 1
fi
echo ""

# Step 4: Instructions for Adroit
echo "=================================================================================="
echo "Step 4: Upload to Adroit and Build with Vivado"
echo "=================================================================================="
echo ""
echo "Upload files:"
echo "  scp build/tpu_basys3.edif constraints/basys3_nextpnr.xdc as0714@adroit.princeton.edu:~/tpu_build/"
echo ""
echo "On Adroit, in Vivado TCL Console:"
echo "  cd ~/tpu_build"
echo "  read_edif tpu_basys3.edif"
echo "  set_property top tpu_top_wrapper [current_fileset]"
echo "  read_xdc basys3_nextpnr.xdc"
echo "  link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper"
echo ""
echo "  # Verify UART pins are assigned"
echo "  report_io | grep -i uart"
echo ""
echo "  # Run implementation"
echo "  opt_design"
echo "  place_design"
echo "  route_design"
echo ""
echo "  # Verify routing"
echo "  report_route_status"
echo ""
echo "  # Generate bitstream"
echo "  write_bitstream -force tpu_top_wrapper_verified.bit"
echo ""
echo "Download:"
echo "  scp as0714@adroit.princeton.edu:~/tpu_build/tpu_top_wrapper_verified.bit build/"
echo ""

echo "=================================================================================="
echo "✓ EDIF ready for upload with verification"
echo "=================================================================================="

