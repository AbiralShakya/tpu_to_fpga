#!/bin/bash
# SymbiFlow Installation Script for macOS

set -e

echo "Installing SymbiFlow tools for Yosys + nextpnr-xilinx flow..."

# Install dependencies
brew install cmake boost eigen python3 git boost-python3

# Clone and build nextpnr with Xilinx support
echo "Building nextpnr-xilinx..."
git clone https://github.com/SymbiFlow/nextpnr.git
cd nextpnr
mkdir build
cd build
cmake .. -DARCH=xilinx -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
sudo make install
cd ../..

# Clone and build fasm2bels
echo "Building fasm2bels..."
git clone https://github.com/SymbiFlow/fasm2bels.git
cd fasm2bels
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
sudo make install
cd ../..

echo "SymbiFlow tools installed successfully!"
echo "You can now use: yosys + nextpnr-xilinx + fasm2bels"
