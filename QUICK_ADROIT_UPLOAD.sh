#!/bin/bash
# Quick script to prepare and upload TPU build files to Adroit

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
UPLOAD_DIR="$PROJECT_ROOT/adroit_upload"
PACKAGE_FILE="$PROJECT_ROOT/tpu_vivado_package.tar.gz"

echo "Preparing TPU build package for Adroit..."

# Create upload directory
mkdir -p "$UPLOAD_DIR"

# Copy essential files
echo "Copying files..."
cp "$PROJECT_ROOT/build/tpu_basys3.edif" "$UPLOAD_DIR/"
cp "$PROJECT_ROOT/constraints/basys3_nextpnr.xdc" "$UPLOAD_DIR/"
cp "$PROJECT_ROOT/vivado/build_tpu_vivado.tcl" "$UPLOAD_DIR/"

# Create README
cat > "$UPLOAD_DIR/README.txt" << 'EOF'
TPU Bitstream Build Package for Adroit
=======================================

Files:
- tpu_basys3.edif: Synthesized netlist from Yosys
- basys3_nextpnr.xdc: FPGA constraints
- build_tpu_vivado.tcl: Automated build script

See ADROIT_VIVADO_GUI_GUIDE.md for instructions.
EOF

# Create tarball
echo "Creating tarball..."
cd "$PROJECT_ROOT"
tar -czf "$PACKAGE_FILE" adroit_upload/

echo ""
echo "✓ Package created: $PACKAGE_FILE"
echo "  Size: $(ls -lh "$PACKAGE_FILE" | awk '{print $5}')"
echo ""
echo "Upload to Adroit:"
echo "  scp $PACKAGE_FILE YOUR_NETID@adroit.princeton.edu:~/tpu_build/"
echo ""
echo "Or to scratch (more space):"
echo "  scp $PACKAGE_FILE YOUR_NETID@adroit.princeton.edu:/scratch/network/YOUR_NETID/tpu_build/"
echo ""
echo "Replace YOUR_NETID with your Princeton NetID!"

