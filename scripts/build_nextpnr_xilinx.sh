#!/bin/bash
# Build nextpnr-xilinx and generate chip database for Basys3

set -e

# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
NEXTPNR_DIR="$PROJECT_ROOT/nextpnr-xilinx"

echo "============================================"
echo "Building nextpnr-xilinx for Basys3"
echo "============================================"
echo ""

cd "$NEXTPNR_DIR"

# Step 1: Check prerequisites
echo "Step 1: Checking prerequisites..."

if ! command -v cmake &> /dev/null; then
    echo "✗ Error: cmake not found!"
    echo "  Install with: brew install cmake"
    exit 1
fi

if ! command -v python3 &> /dev/null; then
    echo "✗ Error: python3 not found!"
    exit 1
fi

echo "✓ Prerequisites OK"
echo ""

# Step 2: Initialize submodules (if needed)
echo "Step 2: Checking submodules..."
if [ ! -d "xilinx/external/prjxray-db" ] || [ ! -d "xilinx/external/nextpnr-xilinx-meta" ]; then
    echo "  Initializing submodules..."
    git submodule update --init --recursive
else
    echo "✓ Submodules already initialized"
fi
echo ""

# Step 3: Configure CMake and build bbasm (needed for chipdb generation)
echo "Step 3: Configuring CMake and building bbasm..."

# Set Eigen3 include directory for macOS Homebrew
# Note: Disabling BUILD_HEAP to avoid Eigen dependency issues on macOS
CMAKE_ARGS="-DARCH=xilinx -DBUILD_PYTHON=OFF -DUSE_OPENMP=OFF -DBUILD_HEAP=OFF"

if [ ! -f "build/CMakeCache.txt" ]; then
    echo "  Configuring CMake..."
    echo "  Note: Disabling Python, OpenMP, and HeAP placer for macOS compatibility..."
    cmake $CMAKE_ARGS -S . -B build
else
    echo "✓ CMake already configured"
fi

# Build bbasm target
if [ ! -f "build/bbasm" ]; then
    echo "  Building bbasm..."
    cmake --build build --target bbasm -j$(sysctl -n hw.ncpu)
    echo "✓ bbasm built"
else
    echo "✓ bbasm already exists"
fi
echo ""

# Step 4: Generate chip database for Basys3 (xc7a35tcpg236-1)
echo "Step 4: Generating chip database for Basys3..."
echo "  Device: xc7a35tcpg236-1 (Basys3)"
echo "  Note: This may take several minutes..."

CHIPDB_DIR="$NEXTPNR_DIR/xilinx"
CHIPDB_BBA="$CHIPDB_DIR/xc7a35t.bba"
CHIPDB_BIN="$CHIPDB_DIR/xc7a35t.bin"

# Check if chipdb already exists
if [ -f "$CHIPDB_BIN" ]; then
    echo "✓ Chip database already exists: $CHIPDB_BIN"
else
    # Generate BBA file
    if [ ! -f "$CHIPDB_BBA" ]; then
        echo "  Generating BBA file (this may take 5-10 minutes)..."
        echo "  Using Project Xray database from submodule..."
        
        # Set XRAY_DIR if not set (point to submodule)
        if [ -z "$XRAY_DIR" ] && [ -d "$NEXTPNR_DIR/xilinx/external/prjxray-db" ]; then
            export XRAY_DIR="$NEXTPNR_DIR/xilinx/external/prjxray-db"
            echo "  XRAY_DIR set to: $XRAY_DIR"
        fi
        
        python3 "$CHIPDB_DIR/python/bbaexport.py" \
            --device xc7a35tcpg236-1 \
            --bba "$CHIPDB_BBA"
        echo "✓ BBA file generated"
    else
        echo "✓ BBA file already exists"
    fi
    
    # Convert BBA to binary
    echo "  Converting BBA to binary..."
    ./build/bbasm --l "$CHIPDB_BBA" "$CHIPDB_BIN"
    echo "✓ Chip database generated: $CHIPDB_BIN"
fi
echo ""

# Step 5: Build nextpnr-xilinx
echo "Step 5: Building nextpnr-xilinx..."
if [ ! -f "build/nextpnr-xilinx" ]; then
    # Build (CMake already configured in Step 3)
    echo "  Building (this may take 10-20 minutes)..."
    cmake --build build --target nextpnr-xilinx -j$(sysctl -n hw.ncpu)
    echo "✓ nextpnr-xilinx built"
else
    echo "✓ nextpnr-xilinx already exists"
fi
echo ""

# Step 6: Verify build
echo "Step 6: Verifying build..."
if [ -f "build/nextpnr-xilinx" ]; then
    echo "✓ nextpnr-xilinx binary: build/nextpnr-xilinx"
    ./build/nextpnr-xilinx --version 2>&1 | head -3 || true
else
    echo "✗ Error: nextpnr-xilinx binary not found after build!"
    exit 1
fi

if [ -f "$CHIPDB_BIN" ]; then
    echo "✓ Chip database: $CHIPDB_BIN"
    ls -lh "$CHIPDB_BIN"
else
    echo "✗ Error: Chip database not found!"
    exit 1
fi

echo ""
echo "============================================"
echo "Build Complete!"
echo "============================================"
echo ""
echo "nextpnr-xilinx location: $NEXTPNR_DIR/build/nextpnr-xilinx"
echo "Chip database location: $CHIPDB_BIN"
echo ""
echo "To use in build script, update build_tpu_bitstream.sh to use:"
echo "  $NEXTPNR_DIR/build/nextpnr-xilinx"
echo ""

