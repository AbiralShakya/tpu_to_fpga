#!/bin/bash
# Docker-based SymbiFlow Build Script

set -e

echo "Building TPU with Docker SymbiFlow..."

# Start Docker if not running
if (! docker stats --no-stream >/dev/null 2>&1); then
    echo "Starting Docker..."
    open -a Docker
    sleep 10  # Wait for Docker to start
fi

# Run SymbiFlow synthesis in Docker
echo "Running Yosys synthesis..."
docker run --rm -v $(pwd):/work ghcr.io/hdl/symbiflow/symbiflow-arch-defs:latest \
    bash -c "cd /work && yosys synth_basys3.ys"

echo "Running nextpnr-xilinx..."

# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Try to find chipdb file
CHIPDB_SRC=""
if [ -f "$PROJECT_ROOT/synthesis/nextpnr/xc7a35t.bin" ]; then
    CHIPDB_SRC="$PROJECT_ROOT/synthesis/nextpnr/xc7a35t.bin"
elif [ -f "/Users/abiralshakya/Downloads/diffTPU/xc7a35t.bin" ]; then
    CHIPDB_SRC="/Users/abiralshakya/Downloads/diffTPU/xc7a35t.bin"
else
    echo "✗ Error: xc7a35t.bin not found!"
    exit 1
fi

docker run --rm -v "$PROJECT_ROOT":/work -v "$(dirname "$CHIPDB_SRC")":/chipdb \
    ghcr.io/hdl/symbiflow/symbiflow-arch-defs:latest \
    bash -c "cd /work && nextpnr-xilinx \
        --chipdb /chipdb/xc7a35t.bin \
        --xdc tpu_constraints.xdc \
        --json build/tpu_basys3.json \
        --fasm build/tpu_basys3.fasm"

echo "Converting to bitstream..."
docker run --rm -v $(pwd):/work ghcr.io/hdl/symbiflow/symbiflow-arch-defs:latest \
    bash -c "cd /work && fasm2bels \
        --device xc7a35tcpg236-1 \
        --input build/tpu_basys3.fasm \
        --output build/tpu_basys3.bit"

echo "Programming FPGA..."
openFPGALoader -b basys3 build/tpu_basys3.bit

echo "Done! Your TPU is now on the Basys 3 FPGA!"
