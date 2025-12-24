#!/bin/bash
# Script to upload TPU project files to Princeton Adroit cluster
# Usage: ./upload_to_adroit.sh

set -e

NETID="as0714"
ADROIT_HOST="adroit.princeton.edu"
REMOTE_DIR="/scratch/network/as0714/tpu_to_fpga"
# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LOCAL_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "=========================================="
echo "Upload TPU Project to Princeton Adroit"
echo "=========================================="
echo ""

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}Step 1: Preparing files...${NC}"

# Create temporary directory for packaging
TEMP_DIR=$(mktemp -d)
echo "Using temp directory: $TEMP_DIR"

# Copy RTL files
echo "Copying RTL files..."
mkdir -p "$TEMP_DIR/rtl"
cp -r "$LOCAL_DIR/rtl"/*.sv "$TEMP_DIR/rtl/" 2>/dev/null || true
cp -r "$LOCAL_DIR/rtl"/*.v "$TEMP_DIR/rtl/" 2>/dev/null || true

# Copy constraints
echo "Copying constraints..."
cp "$LOCAL_DIR/tpu_constraints.xdc" "$TEMP_DIR/" 2>/dev/null || true

# Copy primitives if exists
if [ -f "$LOCAL_DIR/xilinx_primitives.v" ]; then
    echo "Copying Xilinx primitives..."
    cp "$LOCAL_DIR/xilinx_primitives.v" "$TEMP_DIR/" 2>/dev/null || true
fi

# Copy project creation script
if [ -f "$LOCAL_DIR/create_vivado_project.tcl" ]; then
    echo "Copying project creation script..."
    cp "$LOCAL_DIR/create_vivado_project.tcl" "$TEMP_DIR/" 2>/dev/null || true
fi

# Create package
echo -e "${BLUE}Step 2: Creating package...${NC}"
cd "$TEMP_DIR"
tar -czf tpu_vivado_project.tar.gz *
PACKAGE_SIZE=$(du -h tpu_vivado_project.tar.gz | cut -f1)
echo -e "${GREEN}✓ Package created: $PACKAGE_SIZE${NC}"

# Upload
echo -e "${BLUE}Step 3: Uploading to Adroit...${NC}"
echo "Remote: $NETID@$ADROIT_HOST:$REMOTE_DIR"
echo ""

# Create remote directory if needed
ssh "$NETID@$ADROIT_HOST" "mkdir -p $REMOTE_DIR"

# Upload package
scp tpu_vivado_project.tar.gz "$NETID@$ADROIT_HOST:$REMOTE_DIR/"

echo -e "${GREEN}✓ Upload complete!${NC}"
echo ""

# Cleanup
rm -rf "$TEMP_DIR"

echo -e "${BLUE}Step 4: Extracting on Adroit...${NC}"
echo "Run these commands on Adroit:"
echo ""
echo "  cd $REMOTE_DIR"
echo "  tar -xzf tpu_vivado_project.tar.gz"
echo "  ls -lh rtl/*.sv | head -5"
echo ""

echo -e "${BLUE}Step 5: Launch Visual Vivado${NC}"
echo "On Adroit, run:"
echo ""
echo "  module load vivado/2023.1"
echo "  cd $REMOTE_DIR"
echo "  vivado &"
echo ""

echo "=========================================="
echo -e "${GREEN}Upload Complete!${NC}"
echo "=========================================="
echo ""
echo "Next steps:"
echo "  1. SSH to Adroit: ssh -X $NETID@$ADROIT_HOST"
echo "  2. Extract package: cd $REMOTE_DIR && tar -xzf tpu_vivado_project.tar.gz"
echo "  3. Load Vivado: module load vivado/2023.1"
echo "  4. Launch Vivado: vivado &"
echo "  5. Follow VISUAL_VIVADO_ADROIT_GUIDE.md"
echo ""

