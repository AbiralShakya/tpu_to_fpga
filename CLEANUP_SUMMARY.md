# Repository Cleanup Summary

## Date: 2024-12-26

### Files Removed

1. **Backup Files (.bak)**
   - `constraints/basys3_nextpnr.xdc.bak`
   - `constraints/basys3_nextpnr.xdc.bak2`
   - `scripts/build_tpu_bitstream.sh.bak`
   - `nextpnr-xilinx/CMakeLists.txt.bak`

2. **Old Bitstream Files**
   - Removed old numbered bitstreams (kept only latest 5 versions: 51-55)
   - Removed duplicate bitstreams: `tpu_basys3_vivado_synth (1).bit`, `tpu_basys3-*.bit`, `tpu_top_wrapper*.bit`
   - Removed `simple_test.bit` from root

3. **Unused Files**
   - `simple_test.xdc` (moved to constraints/ if needed)

### Files Organized

1. **Documentation**
   - Moved all `.md` files from root to `docs/notes/`
   - Moved all `.txt` files from root to `docs/reference/`
   - Main `README.md` remains in root

2. **Scripts**
   - Moved shell scripts from root to `scripts/` directory:
     - `PROGRAM_AND_TEST.sh`
     - `QUICK_*.sh` scripts
     - `REBUILD_*.sh` scripts
     - `SIMPLE_*.sh` scripts
     - `USE_*.sh` scripts
     - `build_*.sh` scripts
     - `test_*.sh` scripts
   - `upload_and_build.sh` remains in root (primary build script)

3. **Python Test Files**
   - Moved `test_uart_simple.py` and `test_tpu_instructions.py` from root to `python/`

### .gitignore Updates

Added exclusions for large third-party directories:
- `nextpnr-xilinx/` (should be a git submodule if needed)
- `openXC7-TetriSaraj/` (third-party example)
- `squashfs-root/` (extracted archive)
- `toolchain-installer/` (installer files)
- `tpu_vivado_package.tar.gz` (large archive)
- `yosys_0.38_amd64.snap` (snap package)
- Virtual environments: `venv/`, `tpu_env/`, `env/`

### Files Kept (Still in Use)

1. **Constraint Files** (all are used for different purposes):
   - `constraints/basys3_vivado.xdc` - Main Vivado constraints
   - `constraints/basys3_nextpnr.xdc` - Nextpnr constraints
   - `constraints/basys3_with_timing.xdc` - Timing constraints
   - `constraints/basys3.xdc` - Generic constraints
   - `constraints/uart_loopback.xdc` - Loopback test constraints
   - `constraints/simple_test.xdc` - Simple test constraints

2. **RTL Files**:
   - `rtl/uart_dma_basys3.sv` - Currently used
   - `rtl/uart_dma_basys3_improved.sv` - Kept for reference (not currently used)

3. **Build Artifacts**:
   - Kept latest 5 bitstream versions (51-55) for testing/rollback

### Repository Structure After Cleanup

```
tpu_to_fpga/
├── README.md                    # Main readme
├── upload_and_build.sh          # Primary build script
├── .gitignore                   # Updated with exclusions
├── rtl/                         # RTL source files
├── constraints/                 # XDC constraint files
├── python/                      # Python test scripts
├── scripts/                     # Build and utility scripts
├── docs/                        # Documentation
│   ├── notes/                   # Markdown documentation
│   ├── reference/               # Reference text files
│   ├── guides/                  # Existing guides
│   └── archived/                # Archived docs
├── build/                       # Build outputs (gitignored)
├── sim/                         # Simulation files
├── vivado/                      # Vivado TCL scripts
└── archive/                     # Old archived files
```

### Recommendations

1. **Consider removing** `rtl/uart_dma_basys3_improved.sv` if not planning to use it
2. **Archive old bitstreams** to external storage if needed for historical reference
3. **Convert large directories** (`nextpnr-xilinx/`, etc.) to git submodules if they need to be tracked
4. **Review documentation** in `docs/notes/` and consolidate/remove outdated files

