#!/bin/bash
# Quick script to extract and use Vivado package on Adroit

PACKAGE_NAME="vivado_package_v2.tar.gz"
EXTRACT_DIR="vivado_package_v2"

echo "=========================================="
echo "Extract and Use Vivado Package"
echo "=========================================="
echo ""

# Check if package exists
if [ ! -f "$PACKAGE_NAME" ]; then
    echo "Error: $PACKAGE_NAME not found!"
    echo "Available packages:"
    ls -lh *.tar.gz 2>/dev/null
    exit 1
fi

# Extract package
echo "Step 1: Extracting $PACKAGE_NAME..."
tar -xzf "$PACKAGE_NAME"

if [ $? -ne 0 ]; then
    echo "Error: Extraction failed!"
    exit 1
fi

echo "✓ Extraction complete"
echo ""

# Verify contents
echo "Step 2: Verifying contents..."
cd "$EXTRACT_DIR" 2>/dev/null || cd vivado_package 2>/dev/null || {
    echo "Error: Extracted directory not found!"
    exit 1
}

if [ -f "tpu.edif" ] && [ -f "tpu_constraints.xdc" ]; then
    echo "✓ All required files found:"
    ls -lh tpu.edif tpu_constraints.xdc vivado_import_edif.tcl 2>/dev/null
else
    echo "Warning: Some files may be missing"
    ls -lh
fi

echo ""
echo "=========================================="
echo "Next Steps:"
echo "=========================================="
echo ""
echo "1. Load Vivado:"
echo "   module load vivado/2023.1"
echo ""
echo "2. Run batch mode:"
echo "   vivado -mode batch -source vivado_import_edif.tcl"
echo ""
echo "   OR launch GUI:"
echo "   vivado &"
echo ""
echo "3. After build, download bitstream:"
echo "   scp as0714@adroit.princeton.edu:$(pwd)/build/tpu_top.bit ~/tpu_to_fpga/build/"
echo ""
