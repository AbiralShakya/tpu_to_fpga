# TPU to FPGA Codebase Cleanup Plan

## Current Issues
- 69 files at root level (too cluttered)
- Multiple outdated test files and binaries
- Duplicate/obsolete documentation
- Build artifacts mixed with source
- Multiple synthesis scripts for different purposes

## Proposed Structure

```
tpu_to_fpga/
├── README.md                      # Main project README
├── docs/                          # All documentation
│   ├── IMPLEMENTATION_SUMMARY.md
│   ├── ISA_Reference.md
│   ├── OPCODE_REFERENCE.md
│   ├── guides/
│   │   ├── FPGA_PROGRAMMING_GUIDE.md
│   │   ├── UART_SETUP_GUIDE.md
│   │   ├── COMPREHENSIVE_TESTING_GUIDE.md
│   │   └── vivado/
│   │       ├── ADROIT_QUICK_START.md
│   │       ├── ADROIT_VIVADO_STEPS.md
│   │       ├── VISUAL_VIVADO_ADROIT_GUIDE.md
│   │       └── VIVADO_OPT_DESIGN_FIX.md
│   └── archived/
│       ├── DOCKER_BUILD_GUIDE.md
│       ├── F4PGA_GUIDE.md
│       └── OPEN_SOURCE_FPGA_SETUP.md
├── rtl/                           # RTL source files (keep as is)
├── constraints/                   # Constraint files
│   ├── basys3.xdc
│   └── simple_test.xdc
├── synthesis/                     # Synthesis scripts
│   ├── yosys/
│   │   ├── synth_basys3.ys
│   │   ├── synth_simple_test.ys
│   │   └── synth_simple.ys
│   └── nextpnr/
│       └── xc7a35t.bin
├── python/                        # Python scripts and drivers
│   ├── drivers/
│   │   ├── tpu_coprocessor_driver.py
│   │   └── tpu_driver.py
│   ├── instruction_encoder.py
│   ├── test_all_instructions.py
│   ├── test_isa_fpga.py
│   ├── demo_tpu_complete.py
│   └── example_coprocessor_usage.py
├── sim/                           # Simulation files (keep structure)
├── scripts/                       # Build and utility scripts
│   ├── build_tpu_bitstream.sh
│   ├── check_vivado.sh
│   ├── complete_workflow.sh
│   ├── setup_env.sh
│   └── upload_to_adroit.sh
├── build/                         # Build outputs (keep, add .gitignore)
├── vivado/                        # Vivado-specific files
│   ├── bitstream_package/
│   ├── create_vivado_project.tcl
│   └── extract_vivado_package.sh
├── assets/                        # Images and diagrams
│   ├── TPU_Datapath_Diagram.svg
│   └── noyceimg.png.jpg
└── archive/                       # Obsolete files to remove or archive
    ├── minimal_controller*.v
    ├── test_*.v (old test files)
    ├── *.vvp (compiled simulation)
    ├── obj_dir/ (build artifacts)
    └── tpu_to_fpga/ (duplicate?)
```

## Files to Delete (Obsolete/Generated)
- *.vvp (compiled Verilog)
- test_ctrl, test_integration, test_pipeline, test_signals, test_ub (binaries)
- simple_test, simple_test.v, simple_test.vcd (old test artifacts)
- minimal_controller*.v (4 versions, likely obsolete)
- obj_dir/ (Verilator build artifacts)
- __pycache__/
- bitstream_package.tar.gz (can regenerate)
- openXC7-TetriSaraj/ (external project?)
- tpu_to_fpga/ (subdirectory - duplicate?)

## Files to Keep at Root
- README.md (main)
- .gitignore
- xilinx_primitives.v (needed for synthesis)
- opcodes.csv (data file)
- controlsignaltable.txt (data file)

## Action Items
1. Create new directory structure
2. Move documentation files
3. Move Python scripts
4. Move synthesis files
5. Move constraint files
6. Move shell scripts
7. Move Vivado files
8. Move assets
9. Delete obsolete files
10. Update paths in scripts
11. Create comprehensive .gitignore
12. Update main README

