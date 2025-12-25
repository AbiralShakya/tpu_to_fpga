# TPU ISA Opcode Reference Table

## Instruction Format (32-bit)

```
┌─────────┬─────────┬─────────┬─────────┬────────┐
│ 31..26  │ 25..18  │ 17..10  │  9..2   │  1..0  │
├─────────┼─────────┼─────────┼─────────┼────────┤
│ OPCODE  │  ARG1   │  ARG2   │  ARG3   │ FLAGS  │
│ (6-bit) │ (8-bit) │ (8-bit) │ (8-bit) │(2-bit) │
└─────────┴─────────┴─────────┴─────────┴────────┘
```

## Complete Opcode Table (20 Instructions)

### Memory & Data Movement (6 instructions)

| Opcode | Hex  | Mnemonic      | ARG1        | ARG2       | ARG3       | FLAGS          | Description                    |
|--------|------|---------------|-------------|------------|------------|----------------|--------------------------------|
| 0x00   | 00   | **NOP**       | —           | —          | —          | —              | No operation                   |
| 0x01   | 01   | **RD_HOST**   | UB addr     | Length[15:8]| Length[7:0]| Element size   | Read from host memory via DMA  |
| 0x02   | 02   | **WR_HOST**   | UB addr     | Length[15:8]| Length[7:0]| Element size   | Write to host memory via DMA   |
| 0x03   | 03   | **RD_WEIGHT** | Tile base   | Num tiles  | —          | Buffer select  | Read weight data into FIFO     |
| 0x04   | 04   | **LD_UB**     | Address     | Count      | —          | —              | Load from unified buffer       |
| 0x05   | 05   | **ST_UB**     | Address     | Count      | —          | —              | Store to unified buffer        |

### Matrix Operations (3 instructions)

| Opcode | Hex  | Mnemonic      | ARG1        | ARG2       | ARG3       | FLAGS          | Description                    |
|--------|------|---------------|-------------|------------|------------|----------------|--------------------------------|
| 0x10   | 10   | **MATMUL**    | UB addr     | Acc addr   | Num rows   | [1]=Signed     | Matrix multiplication          |
|        |      |               |             |            |            | [0]=Transpose  |                                |
| 0x11   | 11   | **CONV2D**    | UB addr     | Acc addr   | Num rows   | [1]=Signed     | 2D Convolution                 |
|        |      |               |             |            |            | [0]=Transpose  |                                |
| 0x12   | 12   | **MATMUL_ACC**| UB addr     | Acc addr   | Num rows   | [1]=Signed     | Matrix multiply + accumulate   |
|        |      |               |             |            |            | [0]=Transpose  |                                |

### Activation Functions (4 instructions)

| Opcode | Hex  | Mnemonic      | ARG1        | ARG2       | ARG3       | FLAGS          | Description                    |
|--------|------|---------------|-------------|------------|------------|----------------|--------------------------------|
| 0x18   | 18   | **RELU**      | Acc in addr | UB out addr| Length     | [0]=Use param  | ReLU activation                |
| 0x19   | 19   | **RELU6**     | Acc in addr | UB out addr| Length     | [0]=Use param  | ReLU6 activation (clamp @ 6)   |
| 0x1A   | 1A   | **SIGMOID**   | Acc in addr | UB out addr| Length     | [0]=Use param  | Sigmoid activation             |
| 0x1B   | 1B   | **TANH**      | Acc in addr | UB out addr| Length     | [0]=Use param  | Hyperbolic tangent             |

### Pooling & Normalization (4 instructions)

| Opcode | Hex  | Mnemonic      | ARG1        | ARG2       | ARG3       | FLAGS          | Description                    |
|--------|------|---------------|-------------|------------|------------|----------------|--------------------------------|
| 0x20   | 20   | **MAXPOOL**   | UB in addr  | UB out addr| —          | —              | Max pooling operation          |
| 0x21   | 21   | **AVGPOOL**   | UB in addr  | UB out addr| —          | —              | Average pooling operation      |
| 0x22   | 22   | **ADD_BIAS**  | Acc in addr | UB bias addr| Length    | —              | Add bias vector to data        |
| 0x23   | 23   | **BATCH_NORM**| UB in addr  | UB out addr| Config idx | —              | Batch normalization            |

### Control & Synchronization (3 instructions)

| Opcode | Hex  | Mnemonic      | ARG1        | ARG2       | ARG3       | FLAGS          | Description                    |
|--------|------|---------------|-------------|------------|------------|----------------|--------------------------------|
| 0x30   | 30   | **SYNC**      | Unit mask   | Timeout[15:8]| Timeout[7:0]| —            | Synchronize & toggle buffers   |
| 0x31   | 31   | **CFG_REG**   | Reg addr    | Data[15:8] | Data[7:0]  | —              | Configure register             |
| 0x3F   | 3F   | **HALT**      | —           | —          | —          | —              | Stop execution & interrupt     |

---

## Opcode Map (Visual Grid)

```
      x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xA   xB   xC   xD   xE   xF
    ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
0x  │NOP │ RDH│ WRH│ RDW│LDub│STub│    │    │    │    │    │    │    │    │    │    │
    ├────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
1x  │MMUL│CON2│MACC│    │    │    │    │    │RELU│RLU6│SIGM│TANH│    │    │    │    │
    ├────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
2x  │MAXP│AVGP│BIAS│BNRM│    │    │    │    │    │    │    │    │    │    │    │    │
    ├────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
3x  │SYNC│CFGR│    │    │    │    │    │    │    │    │    │    │    │    │    │HALT│
    └────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
```

**Legend:**
- **RDH** = RD_HOST_MEM, **WRH** = WR_HOST_MEM, **RDW** = RD_WEIGHT
- **LDub** = LD_UB, **STub** = ST_UB
- **MMUL** = MATMUL, **CON2** = CONV2D, **MACC** = MATMUL_ACC
- **RLU6** = RELU6, **SIGM** = SIGMOID
- **MAXP** = MAXPOOL, **AVGP** = AVGPOOL, **BIAS** = ADD_BIAS, **BNRM** = BATCH_NORM
- **CFGR** = CFG_REG

---

## FLAGS Field Encoding

### Memory Operations (RD_HOST, WR_HOST)
```
FLAGS[1:0] = Element Size
  00 = 8-bit  (byte)
  01 = 16-bit (half-word)
  10 = 32-bit (word)
  11 = Reserved
```

### Matrix Operations (MATMUL, CONV2D, MATMUL_ACC)
```
FLAGS[1] = Signed arithmetic (1=signed, 0=unsigned)
FLAGS[0] = Transpose input  (1=transpose, 0=normal)
```

### Activation Functions (RELU, RELU6, SIGMOID, TANH)
```
FLAGS[0] = Use parameter from config register
  0 = Use default parameter
  1 = Use cfg_registers[FLAGS[1:0]]
```

### Weight Loading (RD_WEIGHT)
```
FLAGS[0] = Weight buffer select
  0 = Buffer A
  1 = Buffer B
```

---

## Instruction Categories by Function

### Category 1: Data Movement (35% of instructions)
- **Host Interface**: RD_HOST_MEM, WR_HOST_MEM
- **Weight Loading**: RD_WEIGHT
- **Buffer Operations**: LD_UB, ST_UB
- **Control**: NOP, HALT

### Category 2: Compute (25% of instructions)
- **Matrix/Tensor**: MATMUL, CONV2D, MATMUL_ACC
- **Normalization**: ADD_BIAS, BATCH_NORM

### Category 3: Activation (20% of instructions)
- **Non-linear**: RELU, RELU6, SIGMOID, TANH

### Category 4: Pooling (10% of instructions)
- **Spatial Reduction**: MAXPOOL, AVGPOOL

### Category 5: Synchronization (10% of instructions)
- **Control Flow**: SYNC, CFG_REG

---

## Control Signal Mapping

Each instruction generates **up to 46 control signals**:

| Category               | Signals | Controlled By                                      |
|------------------------|---------|-----------------------------------------------------|
| Pipeline/Program       | 4       | All instructions (pc_cnt, pc_ld, ir_ld, flush)     |
| Systolic Array         | 7       | MATMUL, CONV2D, MATMUL_ACC                         |
| Unified Buffer         | 7       | LD_UB, ST_UB, all compute/activation ops           |
| Weight FIFO            | 5       | RD_WEIGHT                                          |
| Accumulator            | 4       | MATMUL*, all activation ops                        |
| VPU (Vector)           | 6       | RELU, RELU6, SIGMOID, TANH, MAXPOOL, AVGPOOL      |
| DMA                    | 5       | RD_HOST_MEM, WR_HOST_MEM                           |
| Sync/Control           | 3       | SYNC                                               |
| Configuration          | 3       | CFG_REG                                            |
| Halt/Interrupt         | 2       | HALT                                               |

**Total control outputs**: 46 signals (lines 18-86 in tpu_controller.sv)

---

## Typical Instruction Sequences

### Example 1: Matrix Multiplication Workload
```
RD_HOST_MEM   UB[0],  host_addr, 256, 32-bit    # Load input matrix
RD_WEIGHT     tile_0, 4_tiles                   # Load weight tiles
MATMUL        UB[0],  ACC[0], 16, signed        # Compute: ACC = A × W
RELU          ACC[0], UB[1],  256               # Activation
WR_HOST_MEM   UB[1],  result_addr, 256, 32-bit  # Write back results
HALT                                            # Done
```

### Example 2: Convolution + BatchNorm + Pooling
```
RD_HOST_MEM   UB[0],  input, 1024, 8-bit        # Load image
RD_WEIGHT     tile_0, 16_tiles                  # Load conv kernels
CONV2D        UB[0],  ACC[0], 32, unsigned      # Convolution
BATCH_NORM    UB[0],  UB[1],  cfg_idx_0         # Normalize
RELU6         ACC[0], UB[2],  512               # ReLU6 activation
MAXPOOL       UB[2],  UB[3]                     # 2×2 max pooling
SYNC          0xF,    timeout_1000              # Wait for completion
WR_HOST_MEM   UB[3],  output, 256, 8-bit        # Write results
HALT
```

---

## Implementation Notes

1. **Pipeline Depth**: 2-stage pipeline (Fetch/Decode + Execute)
2. **Hazard Handling**: Automatic stall on unit busy (sys_busy, vpu_busy, dma_busy, wt_busy)
3. **Buffer Double-Buffering**: SYNC instruction toggles all buffer select signals
4. **Instruction Width**: Fixed 32-bit for all instructions (orthogonal ISA)
5. **Address Space**:
   - Unified Buffer: 512 bytes × 2 banks (9-bit address)
   - Accumulator: 256 entries × 2 banks (8-bit address)
   - Instruction Memory: 256 instructions (8-bit PC)
   - Config Registers: 256 × 16-bit registers

6. **VPU Modes** (4-bit encoding):
   - 0x1: ReLU
   - 0x2: ReLU6
   - 0x3: Sigmoid
   - 0x4: Tanh
   - 0x5: Bias Add
   - 0x6: Max Pool
   - 0x7: Avg Pool
   - 0x8: Batch Norm

---

## RTL Files
- **Opcode Definitions**: `rtl/tpu_controller.sv` (lines 98-117)
- **Decode Logic**: `rtl/tpu_controller.sv` (lines 403-817)
- **Pipeline Registers**: `rtl/tpu_controller.sv` (lines 146-163)
