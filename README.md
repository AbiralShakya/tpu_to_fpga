# TPU FPGA Implementation

Tensor Processing Unit implementation for Xilinx Basys3 FPGA with UART host interface.

## Architecture

- 3x3 systolic array (9 MACs)
- 64 KiB unified buffer (double-buffered)
- Weight memory for model storage
- Vector processing unit (ReLU, pooling, quantization)
- 20-instruction ISA
- UART DMA at 115200 baud

```
Host (Python) <--UART--> FPGA TPU
                          |
          +---------------+---------------+
          |               |               |
     Controller      Datapath        UART DMA
          |               |
          v               v
   Instr Memory     Systolic Array (3x3)
                          |
                          v
                    Accumulators
                          |
                          v
                   Unified Buffer
```

## Current Status

### Working
- UART communication (read/write unified buffer)
- Weight memory loading
- Instruction memory loading
- Program execution (RD_WEIGHT, LD_UB, MATMUL, ST_UB, HALT)
- Basic data flow through systolic array

### Known Issue (Under Fix)
- ST_UB instruction returns incorrect values (-1 instead of computed results)
- Root cause: accumulator outputs not latched at end of MATMUL
- Fix applied to rtl/tpu_datapath.sv, awaiting FPGA re-synthesis

## Quick Start

### Build Bitstream
```bash
cd vivado
vivado -mode batch -source build_tpu_vivado.tcl
```

### Program FPGA
```bash
vivado -mode batch -source program_fpga.tcl
```

### Run Test
```bash
python3 python/tpu_coprocessor.py /dev/ttyUSB1
```

## Project Structure

```
rtl/                    SystemVerilog source
  tpu_top.sv            Top-level module
  tpu_controller.sv     ISA decoder and sequencer
  tpu_datapath.sv       Systolic array and accumulators
  mmu.sv                3x3 matrix multiply unit
  pe.sv                 Processing element
  unified_buffer.sv     Activation memory
  accumulator.sv        Partial sum storage
  uart_dma_basys3.sv    UART interface
  basys3_test_interface.sv  Board integration

python/                 Host software
  tpu_coprocessor.py    Main driver and demo
  drivers/              Low-level drivers

constraints/            Xilinx constraint files
vivado/                 Build scripts
docs/                   Documentation
```

## ISA Summary

| Opcode | Name      | Description                    |
|--------|-----------|--------------------------------|
| 0x00   | NOP       | No operation                   |
| 0x03   | RD_WEIGHT | Load weights to systolic array |
| 0x04   | LD_UB     | Load activations from buffer   |
| 0x05   | ST_UB     | Store results to buffer        |
| 0x10   | MATMUL    | Matrix multiply                |
| 0x18   | RELU      | ReLU activation                |
| 0x30   | SYNC      | Synchronization barrier        |
| 0x3F   | HALT      | Stop execution                 |

Instruction format: [31:26] opcode | [25:18] arg1 | [17:10] arg2 | [9:2] arg3 | [1:0] flags

## UART Protocol

| Command     | Code | Description              |
|-------------|------|--------------------------|
| WRITE_UB    | 0x01 | Write unified buffer     |
| WRITE_WT    | 0x02 | Write weight memory      |
| WRITE_INSTR | 0x03 | Write instruction        |
| READ_UB     | 0x04 | Read unified buffer      |
| EXECUTE     | 0x05 | Start program execution  |
| READ_STATUS | 0x06 | Read status register     |

## Hardware

- Target: Xilinx Basys3 (Artix-7 XC7A35T)
- Clock: 100 MHz
- UART: 115200 baud, 8N1
- Resources: ~8K LUTs, ~6K FFs, ~40 BRAM, 9 DSPs

## Recent Changes

### 2025-12-27: ST_UB Fix
- Added accumulator output latching in tpu_datapath.sv
- Latches capture PE outputs during systolic_active
- Packed 8-bit outputs in consecutive bytes for Python compatibility
- Requires FPGA re-synthesis to take effect

### 2025-12-26: UART Interface Fixes
- Fixed unified buffer read handshaking (ub_rd_valid)
- Fixed bank selection for double-buffered memory
- Extended address width to 9 bits
- Added burst count signals

## Build Requirements

- Vivado 2023.x or later
- Python 3.8+ with pyserial, numpy
- Basys3 FPGA board

## License

See LICENSE file.
