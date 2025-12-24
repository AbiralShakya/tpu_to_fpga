# 3x3 TPU-to-FPGA: Extended tinytinyTPU for Basys 3 FPGA

A minimal 3x3 systolic-array TPU-style matrix-multiply unit, implemented in SystemVerilog and optimized for the Digilent Basys 3 FPGA board.

The design models the full post-MAC pipeline:
MMU -> Accumulator (alignment + double buffering) -> Activation + Normalization + Loss -> Quantization -> Unified Buffer

Based on [tinytinyTPU](https://github.com/Alanma23/tinytinyTPU) but extended for 3x3 systolic array and FPGA deployment.

--------------------------------------------------------------------------------

## Project Structure

```
tpu_to_fpga/
|
|-- rtl/                          # SystemVerilog RTL source files
|   |-- pe.sv                     # Processing Element
|   |-- mmu.sv                    # 3x3 Matrix Multiply Unit (extended)
|   |-- weight_fifo.sv            # Single-column weight FIFO
|   |-- dual_weight_fifo.sv       # Dual-column weight FIFO with skew
|   |-- accumulator.sv            # Top-level accumulator (modular)
|   |-- accumulator_align.sv      # Column alignment logic
|   |-- accumulator_mem.sv        # Double-buffered accumulator memory
|   |-- activation_func.sv        # ReLU/ReLU6 activation
|   |-- normalizer.sv             # Gain/bias/shift normalization
|   |-- loss_block.sv             # L1 loss computation
|   |-- activation_pipeline.sv    # Full post-accumulator pipeline
|   |-- unified_buffer.sv         # Ready/valid output FIFO
|   |-- mlp_top.sv                # Top-level MLP integration wrapper
|   |-- tpu_datapath.sv           # FPGA datapath integration
|   |-- tpu_controller.sv         # FPGA control logic
|   |-- tpu_top.sv                # FPGA top-level with UART
|   |-- uart_rx.sv                # UART receiver
|   |-- uart_tx.sv                # UART transmitter
|   `-- uart_dma_basys3.sv        # UART DMA controller
|
|-- sim/                          # Simulation environment
|   |-- Makefile                  # Build and test automation
|   |-- tests/                    # cocotb Python testbenches
|   |   |-- test_pe.py
|   |   `-- test_mmu.py
|   `-- waves/                    # Generated VCD waveforms
|
|-- archive/                      # Original project files (preserved)
|   |-- tpu_constraints.xdc       # FPGA timing constraints
|   |-- tpu_driver.py             # Python host interface
|   `-- ISA_Reference.md          # Instruction documentation
|
`-- README.md
```

--------------------------------------------------------------------------------

## Quick Start

### Prerequisites

- Verilator 5.022 or later (for cocotb tests)
- Python 3.8+
- cocotb and cocotb-test: `pip3 install cocotb cocotb-test pytest`
- GTKWave or Surfer for waveform viewing (optional)

### Running Tests

All simulation commands must be run from the `sim/` directory:

```bash
cd sim

# Run all tests
make test

# Run all tests with waveform generation
make test WAVES=1

# Run specific module test
make test_pe
make test_mmu
make test_mlp

# Run with waveforms
make test_pe WAVES=1
```

### Viewing Waveforms

```bash
# List available waveforms
make waves

# Open specific waveform
make waves MODULE=pe
make waves MODULE=mmu
make waves MODULE=mlp_top
```

### Linting

```bash
make lint
```

--------------------------------------------------------------------------------

## Core RTL Modules

### pe.sv - Processing Element

The PE is the fundamental compute block.

- Multiply-Accumulate (MAC): `psum_out = psum_in + (in_act * weight)`
- Data forwarding: activation flows right, partial sum flows down
- Weight loading: separate `en_weight_pass` and `en_weight_capture` signals

Design Notes:
- Single-cycle registered outputs
- `en_weight_pass` controls psum passthrough during load phase
- `en_weight_capture` triggers weight register latch
- Systolic-friendly timing for TPU-style arrays

### mmu.sv - 3x3 Systolic Array (Extended)

```
PE00 -> PE01 -> PE02    Activations flow horizontally (right)
  |       |       |     Row 0: direct input
  v       v       v     Row 1: 1-cycle delay
PE10 -> PE11 -> PE12    Row 2: 2-cycle delay
  |       |       |
  v       v       v     Partial sums flow vertically (down)
acc0    acc1    acc2    Outputs to accumulator
```

**Extensions from tinytinyTPU:**
- Third column of PEs (PE02, PE12)
- Extended weight loading for diagonal wavefront (3 cycles)
- Additional column capture signal (`en_capture_col2`)

Responsibilities:
- Feeds activations into rows with proper skewing (row0 direct, row1+row2 delayed)
- Loads weights via vertical psum path with per-column capture timing
- Emits three partial-sum columns to the accumulator

Control Signals:
- `en_weight_pass` - broadcast to all PEs during weight load phase
- `en_capture_col0` - capture enable for column 0 PEs
- `en_capture_col1` - capture enable for column 1 PEs (staggered)
- `en_capture_col2` - capture enable for column 2 PEs (staggered)

### dual_weight_fifo.sv - Staggered Column Weight FIFO

Three independent 4-entry queues share one data bus to fill all MMU columns.

- Column 0: Combinational read output (no latency)
- Column 1: Registered output with 1-cycle skew
- Column 2: Registered output with 2-cycle skew (our extension)
- Single `pop` signal advances both read pointers

### accumulator.sv - Modular Accumulator

**Architecture Change:** Split into separate align and memory modules (tinytinyTPU compatible)

Components:
- `accumulator_align.sv` - Deskews column outputs for proper timing
- `accumulator_mem.sv` - Double-buffered 32-bit storage with accumulate/overwrite modes

### activation_pipeline.sv

Top-level post-accumulator stage:

1. Activation (ReLU/ReLU6/passthrough)
2. Normalization (gain/bias/shift)
3. Parallel loss computation
4. Affine int8 quantization with saturation

### unified_buffer.sv

Byte-wide synchronous FIFO with ready/valid backpressure.

--------------------------------------------------------------------------------

## Diagonal Wavefront Weight Loading (Extended)

The 4-cycle staggered weight loading scheme ensures weights propagate through the 3x3 systolic array in a proper diagonal wavefront pattern.

For weight matrix W = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]:

| Cycle | col0_out | col1_out | col2_out | Column 0 Captures | Column 1 Captures | Column 2 Captures |
|-------|----------|----------|----------|-------------------|-------------------|-------------------|
| 0     | 7        | 0 (skew) | 0 (skew) | No                | No                | No                |
| 1     | 4        | 8        | 0 (skew) | Yes               | No                | No                |
| 2     | 1        | 5        | 9        | No                | Yes               | No                |
| 3     | (hold)   | 2        | 6        | No                | No                | Yes               |

Final Weight Distribution:
```
PE00: W[0,0]=1    PE01: W[0,1]=2    PE02: W[0,2]=3
PE10: W[1,0]=4    PE11: W[1,1]=5    PE12: W[1,2]=6
PE20: W[2,0]=7    PE21: W[2,1]=8    PE22: W[2,2]=9
```

--------------------------------------------------------------------------------

## FPGA Integration Features

### UART DMA Communication
- Host interface for programming weights and activations
- Real-time result streaming
- DMA controller for efficient data transfer

### Resource Optimization
- **DSP Slices**: 9/9 (100% utilization on Basys 3)
- **LUTs**: ~27,000 (~52% utilization)
- **Power**: ~350mW estimated
- **Clock**: 100MHz target frequency

### Host Python Interface
```python
from tpu_driver import TPUDriver

tpu = TPUDriver(port='/dev/ttyUSB0')
tpu.load_weights(weights_3x9)  # 3x3 array × 3 layers = 27 weights
result = tpu.forward_pass(input_3x_batch)
```

--------------------------------------------------------------------------------

## Multi-Layer MLP Inference

The `mlp_top.sv` demonstrates multi-layer neural network inference:

```
Weight FIFO -> MMU (3x3 systolic) -> Accumulator -> Activation Pipeline -> UB
                 ^                                                      |
                 +------------------ feedback (next layer) ------------+
```

### 3-Layer MLP Example

```
Input:  A  = [[5, 6, 7], [8, 9, 10]]

Layer 1: H = ReLU(A * W1)    where W1 = 3x3 weight matrix
         H = [[23, 34, 45], [31, 46, 61]]

Layer 2: Y = ReLU(H * W2)    where W2 = 3x3 weight matrix
         Y = [[57, 57, 57], [77, 77, 77]]
```

--------------------------------------------------------------------------------

## Makefile Reference

```
make help                  Show all available commands

Test Commands:
  make test                Run all tests
  make test WAVES=1        Run all tests with waveform generation
  make test_pe             Run PE tests only
  make test_mmu            Run MMU tests only
  make test_mlp            Run MLP integration tests

Waveform Commands:
  make waves               List available waveforms
  make waves MODULE=pe     Open specific waveform in viewer

Other:
  make lint                Run Verilator lint check
  make clean               Remove build artifacts
  make clean_waves         Remove generated waveforms

Environment Variables:
  WAVES=1                  Enable waveform generation
  WAVE_VIEWER=gtkwave      Use GTKWave instead of Surfer
  MODULE=<name>            Specify module for waveform viewing
```

--------------------------------------------------------------------------------

## Pipeline Timing Summary (3x3 Array)

| Phase        | Duration | Description                                    |
|--------------|----------|------------------------------------------------|
| Weight Load  | 4 cycles | Staggered column capture with diagonal wavefront |
| Compute      | 4 cycles | Activation streaming with row skew             |
| First Result | 6 cycles | From compute start to first accumulator output |
| Result Spacing| 1 cycle | Between consecutive valid accumulator outputs  |

--------------------------------------------------------------------------------

## Test Coverage

| Test File                  | Module              | Tests                           |
|---------------------------|---------------------|--------------------------------|
| test_pe.py                | pe                  | Reset, MAC, weight capture      |
| test_mmu.py               | mmu                 | Weight loading, 3x3 matrix multiply |
| test_weight_fifo.py       | weight_fifo         | Push/pop, wraparound           |
| test_dual_weight_fifo.py  | dual_weight_fifo    | Column independence, skew      |
| test_accumulator.py       | accumulator         | Alignment, buffering, modes    |
| test_activation_func.py   | activation_func     | ReLU positive/negative/zero    |
| test_normalizer.py        | normalizer          | Gain, bias, scaling            |
|test_activation_pipeline.py| activation_pipeline| Full pipeline, saturation     |
| test_mlp_integration.py   | mlp_top             | Multi-layer MLP inference      |

All 9 test suites pass with Verilator 5.042.

--------------------------------------------------------------------------------

## SystemVerilog Migration

This project was migrated from Verilog to SystemVerilog for improved synthesis compatibility and modern language features.

### Key Changes

1. File extension: `.v` -> `.sv`

2. Type declarations:
   - `input wire` / `output reg` -> `input logic` / `output logic`
   - `reg` / `wire` -> `logic`

3. Always blocks:
   - `always @(posedge clk)` -> `always_ff @(posedge clk)`
   - `always @(*)` -> `always_comb`

4. Array syntax:
   - `[0:DEPTH-1]` -> `[DEPTH]`

5. Case statements:
   - `case` -> `unique case` where appropriate

6. Width handling:
   - Explicit bit-width casts to avoid Verilator warnings
   - Example: `count == DEPTH` -> `count == (ADDR_W+1)'(DEPTH)`

### Testbench Migration

The Verilog testbenches were replaced with cocotb Python testbenches:

- More maintainable and readable test code
- Easier debugging with Python tools
- VCD waveform generation via Verilator
- Pytest integration for test discovery and reporting

--------------------------------------------------------------------------------

## FPGA Deployment

### Synthesis with Vivado
1. Create new Vivado project
2. Add all `.sv` files from `rtl/` directory
3. Add `archive/tpu_constraints.xdc`
4. Run synthesis and implementation
5. Generate bitstream

### Resource Utilization (Basys 3 - XC7A35T)

| Component | Used | Available | Utilization |
|-----------|------|-----------|-------------|
| DSP Slices | 9 | 90 | 100% |
| LUTs | ~27,000 | 52,160 | ~52% |
| FFs | ~15,000 | 104,320 | ~14% |
| BRAM | 4 | 60 | ~7% |

### Power Analysis

Estimated power consumption:
- **Dynamic Power**: ~320mW
- **Static Power**: ~30mW
- **Total**: ~350mW

Power distribution:
- DSP slices: ~60%
- Clock management: ~15%
- Memory interfaces: ~15%
- Logic: ~10%
