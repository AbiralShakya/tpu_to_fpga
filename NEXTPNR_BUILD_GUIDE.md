# Building nextpnr-xilinx for Basys3

This guide explains how to build `nextpnr-xilinx` and generate the chip database needed for Basys3 FPGA bitstream generation.

## Quick Start

```bash
# Build nextpnr-xilinx and generate chip database
./scripts/build_nextpnr_xilinx.sh
```

This script will:
1. ✓ Check prerequisites (cmake, python3)
2. ✓ Initialize git submodules
3. ✓ Build `bbasm` (needed for chipdb generation)
4. ✓ Generate chip database for Basys3 (xc7a35tcpg236-1)
5. ✓ Build `nextpnr-xilinx` binary

**Expected time:** 15-30 minutes (mostly chipdb generation)

## Manual Steps (if script fails)

### Step 1: Initialize Submodules

```bash
cd nextpnr-xilinx
git submodule update --init --recursive
```

### Step 2: Build bbasm

```bash
cd nextpnr-xilinx/bba
make
cd ..
```

### Step 3: Generate Chip Database

For Basys3 (xc7a35tcpg236-1):

```bash
cd nextpnr-xilinx
python3 xilinx/python/bbaexport.py \
    --device xc7a35tcpg236-1 \
    --bba xilinx/xc7a35t.bba

./bba/bbasm --l xilinx/xc7a35t.bba xilinx/xc7a35t.bin
```

**Note:** This step takes 5-10 minutes. The BBA export uses Project Xray database.

### Step 4: Build nextpnr-xilinx

```bash
cd nextpnr-xilinx
cmake -DARCH=xilinx -B build .
cmake --build build -j$(sysctl -n hw.ncpu)
```

The binary will be at: `nextpnr-xilinx/build/nextpnr-xilinx`

## Prerequisites

- **CMake** (3.10+): `brew install cmake`
- **Python 3**: Usually pre-installed on macOS
- **Git submodules**: Automatically initialized by the script
- **Project Xray database**: Included as submodule in `xilinx/external/prjxray-db`

## Verification

After building, verify:

```bash
# Check binary exists
ls -lh nextpnr-xilinx/build/nextpnr-xilinx

# Check chipdb exists
ls -lh nextpnr-xilinx/xilinx/xc7a35t.bin

# Test nextpnr-xilinx
./nextpnr-xilinx/build/nextpnr-xilinx --version
```

## Using in Build Script

The `build_tpu_bitstream.sh` script will automatically find:
- `nextpnr-xilinx` binary at: `nextpnr-xilinx/build/nextpnr-xilinx`
- Chip database at: `nextpnr-xilinx/xilinx/xc7a35t.bin`

Or you can set environment variables:
```bash
export NEXTPNR_XILINX=/path/to/nextpnr-xilinx
export CHIPDB_PATH=/path/to/xc7a35t.bin
```

## Troubleshooting

### Error: "bbasm: command not found"
- Build bbasm: `cd nextpnr-xilinx/bba && make`

### Error: "bbaexport.py: No module named 'prjxray'"
- Project Xray database is in submodule `xilinx/external/prjxray-db`
- Make sure submodules are initialized: `git submodule update --init --recursive`

### Error: "CMake Error: Boost not found"
- Install Boost: `brew install boost`

### Chipdb generation is slow
- This is normal - BBA export takes 5-10 minutes
- Using `pypy3` instead of `python3` can be faster (2-3x speedup)

### Build fails with "undefined reference"
- Make sure all submodules are up to date
- Try a clean build: `rm -rf build && cmake -DARCH=xilinx -B build . && cmake --build build`

## References

- [nextpnr-xilinx GitHub](https://github.com/gatecat/nextpnr-xilinx)
- [Project Xray](https://github.com/SymbiFlow/prjxray)
- [nextpnr Documentation](https://github.com/YosysHQ/nextpnr)

