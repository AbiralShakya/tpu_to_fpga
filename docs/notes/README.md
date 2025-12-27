# TPU to FPGA Project

A Tensor Processing Unit (TPU) implementation for Xilinx Artix-7 FPGAs with full open-source toolchain support using Yosys + nextpnr-xilinx.

## 🎯 Project Overview

This project implements a custom TPU architecture targeting the Basys3 FPGA development board (Xilinx Artix-7 xc7a35tcpg236-1). The design includes:

- **3x3 Systolic Array** for matrix multiplication
- **Unified Buffer** with dual-bank architecture
- **Custom ISA** for TPU operations
- **UART Interface** for host communication
- **Pipelined Datapath** with double buffering

## 📁 Project Structure

```
tpu_to_fpga/
├── rtl/                    # RTL source files (SystemVerilog)
│   ├── tpu_top.sv         # Top-level TPU module
│   ├── tpu_controller.sv  # Instruction decoder and control
│   ├── tpu_datapath.sv    # Main datapath
│   ├── systolic_controller.sv
│   ├── unified_buffer.sv
│   ├── pe_dsp.sv          # Processing element with DSP48E1
│   └── ...
├── constraints/            # FPGA constraint files
│   ├── basys3.xdc        # Full TPU constraints for Basys3
│   └── simple_test.xdc   # Simple test design constraints
├── synthesis/              # Synthesis scripts and databases
│   ├── yosys/            # Yosys synthesis scripts
│   │   ├── synth_basys3.ys
│   │   └── synth_simple_test.ys
│   └── nextpnr/          # nextpnr-xilinx database
│       └── xc7a35t.bin   # Artix-7 chipdb (88 MB)
├── python/                 # Python scripts and drivers
│   ├── drivers/          # UART communication drivers
│   │   ├── tpu_coprocessor_driver.py
│   │   └── tpu_driver.py
│   ├── instruction_encoder.py
│   ├── test_all_instructions.py
│   └── demo_tpu_complete.py
├── sim/                    # Simulation testbenches
│   ├── tests/            # Cocotb Python tests
│   └── Makefile          # Verilator/Icarus simulation
├── scripts/                # Build and utility scripts
│   ├── complete_workflow.sh
│   ├── setup_env.sh
│   └── upload_to_adroit.sh
├── vivado/                 # Vivado-specific files (optional)
│   ├── bitstream_package/
│   └── create_vivado_project.tcl
├── docs/                   # Documentation
│   ├── ISA_Reference.md
│   ├── OPCODE_REFERENCE.md
│   ├── IMPLEMENTATION_SUMMARY.md
│   └── guides/
│       ├── FPGA_PROGRAMMING_GUIDE.md
│       ├── UART_SETUP_GUIDE.md
│       └── vivado/
├── build/                  # Build outputs (gitignored)
│   ├── simple_test.json  # Yosys synthesis output
│   ├── simple_test.fasm  # FASM netlist
│   └── simple_test.bit   # Final bitstream
├── assets/                 # Images and diagrams
├── archive/                # Archived/obsolete files
├── xilinx_primitives.v    # Xilinx primitive definitions
├── opcodes.csv            # Opcode reference table
└── controlsignaltable.txt # Control signal documentation
```

## 🚀 Quick Start

### Prerequisites

- **Yosys** (synthesis)
- **nextpnr-xilinx** (place & route)
- **Project X-Ray** (bitstream generation)
- **Python 3.9+** with pyserial, numpy
- **Basys3 FPGA board** (or compatible Artix-7)

### Build Flow (Open-Source)

1. **Synthesize with Yosys:**
```bash
cd synthesis/yosys
yosys synth_simple_test.ys
# Output: ../../build/simple_test.json
```

2. **Place & Route with nextpnr-xilinx:**
```bash
nextpnr-xilinx \
  --chipdb synthesis/nextpnr/xc7a35t.bin \
  --json build/simple_test.json \
  --xdc constraints/simple_test.xdc \
  --fasm build/simple_test.fasm \
  --write build/simple_test_routed.json
```

3. **Generate Bitstream:**
```bash
# Convert FASM to frames
python3 $XRAY_UTILS_DIR/fasm2frames.py \
  --db-root $XRAY_DATABASE_DIR/artix7 \
  --part xc7a35tcpg236-1 \
  build/simple_test.fasm > build/simple_test.frames

# Convert frames to bitstream
$XRAY_TOOLS_DIR/xc7frames2bit \
  --part_file $XRAY_DATABASE_DIR/artix7/xc7a35tcpg236-1/part.yaml \
  --part_name xc7a35tcpg236-1 \
  --frm_file build/simple_test.frames \
  --output_file build/simple_test.bit
```

4. **Program FPGA:**
```bash
openFPGALoader -b basys3 build/simple_test.bit
```

### Alternative: Vivado Flow

For the full TPU design with DSP blocks, you may need Vivado:

```bash
cd scripts
./complete_workflow.sh
```

See `docs/guides/vivado/` for detailed Vivado instructions.

## 🧪 Testing

### Simulation

```bash
cd sim
make                    # Run Verilator simulation
make cocotb            # Run Cocotb Python tests
```

### Hardware Testing

```bash
cd python
python3 test_all_instructions.py /dev/ttyUSB0
python3 demo_tpu_complete.py /dev/ttyUSB0
```

## 📖 Documentation

- **[ISA Reference](docs/ISA_Reference.md)** - Instruction set architecture
- **[Opcode Reference](docs/OPCODE_REFERENCE.md)** - Detailed opcode specifications
- **[Implementation Summary](docs/IMPLEMENTATION_SUMMARY.md)** - Architecture overview
- **[FPGA Programming Guide](docs/guides/FPGA_PROGRAMMING_GUIDE.md)** - How to program the board
- **[UART Setup Guide](docs/guides/UART_SETUP_GUIDE.md)** - Serial communication setup

## 🔧 Design Specifications

- **Target Device:** Xilinx Artix-7 xc7a35tcpg236-1 (Basys3)
- **System Clock:** 100 MHz
- **Systolic Array:** 3x3 PEs with DSP48E1 blocks
- **Data Width:** 8-bit activations, 8-bit weights, 32-bit accumulators
- **Unified Buffer:** 512 bytes (dual-bank)
- **Instruction Memory:** 256 entries
- **UART:** 115200 baud, 8N1

## 📊 Resource Utilization

### Simple Test Design
- LUTs: 204 / 65,200 (0.3%)
- FFs: 99 / 65,200 (0.2%)
- Max Frequency: 283.69 MHz

### Full TPU Design
- LUTs: ~15,000 / 65,200 (23%)
- FFs: ~8,000 / 65,200 (12%)
- DSP48E1: 9 / 120 (7.5%)
- BRAM: 8 / 150 (5.3%)

## 🤝 Contributing

This is an academic/research project. Feel free to fork and experiment!

## 📝 License

[Add your license here]

## 🙏 Acknowledgments

- **Yosys** - Open-source synthesis
- **nextpnr** - Open-source place & route
- **Project X-Ray** - Xilinx bitstream documentation
- **Verilator** - Fast simulation
- **Cocotb** - Python-based verification

## 📧 Contact

[Add contact information]

---

**Note:** The open-source toolchain (Yosys + nextpnr-xilinx + Project X-Ray) provides a fully functional alternative to Vivado for Artix-7 FPGAs, though some advanced features may require proprietary tools.
