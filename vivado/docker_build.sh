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
docker run --rm -v $(pwd):/work -v /Users/abiralshakya/Downloads/diffTPU:/diffTPU \
    ghcr.io/hdl/symbiflow/symbiflow-arch-defs:latest \
    bash -c "cd /work && nextpnr-xilinx \
        --chipdb /diffTPU/xc7a35t.bin \
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
