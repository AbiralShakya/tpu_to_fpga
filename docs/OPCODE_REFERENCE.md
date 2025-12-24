# TPU v1 Opcode to Instruction Set Mapping

Complete reference table for all 20 ISA instructions with opcodes, arguments, and descriptions.

## Quick Reference Table

| Opcode | Instruction Name | Category | Description |
|--------|-----------------|----------|-------------|
| `0x00` | NOP | Control | No Operation |
| `0x01` | RD_HOST_MEM | Memory | Read from Host Memory (DMA) |
| `0x02` | WR_HOST_MEM | Memory | Write to Host Memory (DMA) |
| `0x03` | RD_WEIGHT | Memory | Read Weight Data from DRAM |
| `0x04` | LD_UB | Memory | Load from Unified Buffer |
| `0x05` | ST_UB | Memory | Store to Unified Buffer |
| `0x10` | MATMUL | Compute | Matrix Multiplication |
| `0x11` | CONV2D | Compute | 2D Convolution |
| `0x12` | MATMUL_ACC | Compute | Accumulate Matrix Multiply |
| `0x18` | RELU | Activation | Rectified Linear Unit |
| `0x19` | RELU6 | Activation | ReLU6 Activation |
| `0x1A` | SIGMOID | Activation | Sigmoid Activation |
| `0x1B` | TANH | Activation | Hyperbolic Tangent |
| `0x20` | MAXPOOL | Pooling | Max Pooling |
| `0x21` | AVGPOOL | Pooling | Average Pooling |
| `0x22` | ADD_BIAS | Compute | Add Bias Vector |
| `0x23` | BATCH_NORM | Compute | Batch Normalization |
| `0x30` | SYNC | Control | Synchronize Operations |
| `0x31` | CFG_REG | Control | Configure Register |
| `0x3F` | HALT | Control | Program Termination |

---

## Detailed Instruction Reference

### Control Instructions

#### `0x00: NOP` - No Operation
- **Arguments**: ARG1, ARG2, ARG3: Ignored
- **Flags**: Ignored
- **Timing**: 1 cycle
- **Use Case**: Padding, timing alignment
- **Example**: `NOP 0x00, 0x00, 0x00, 0b00`

#### `0x30: SYNC` - Synchronize Operations
- **Arguments**:
  - ARG1: Sync mask (bit field)
    - Bit 0: Systolic array completion
    - Bit 1: VPU completion
    - Bit 2: DMA completion
    - Bit 3: Weight FIFO load completion
  - ARG2: Timeout high byte
  - ARG3: Timeout low byte
- **Flags**: Reserved
- **Timing**: Variable (until completion or timeout)
- **Use Case**: Ensure data dependencies are met
- **Example**: `SYNC 0x01, 0x00, 0x10, 0b00` (wait systolic, timeout 4096 cycles)

#### `0x31: CFG_REG` - Configure Register
- **Arguments**:
  - ARG1: Register address (0-255)
  - ARG2: Data high byte
  - ARG3: Data low byte
- **Flags**: Reserved
- **Timing**: 1 cycle
- **Use Case**: Set activation parameters, pooling configurations
- **Example**: `CFG_REG 0x00, 0x06, 0x66, 0b00` (write 0x0666 to cfg[0])

#### `0x3F: HALT` - Program Termination
- **Arguments**: All ignored
- **Flags**: Ignored
- **Timing**: Program ends
- **Use Case**: End of neural network inference
- **Example**: `HALT 0x00, 0x00, 0x00, 0b00`

---

### Memory Instructions

#### `0x01: RD_HOST_MEM` - Read from Host Memory
- **Arguments**:
  - ARG1: UB destination address (0-255)
  - ARG2: Transfer length high byte
  - ARG3: Transfer length low byte
- **Flags**:
  - F[0]: Element size (0=8-bit, 1=16-bit)
  - F[1]: Burst mode (0=single, 1=burst)
- **Timing**: 1 cycle initiation + DMA transfer time
- **Use Case**: Loading input activations from host
- **Example**: `RD_HOST_MEM 0x00, 0x04, 0x00, 0b00` (read 1024 bytes to UB[0])

#### `0x02: WR_HOST_MEM` - Write to Host Memory
- **Arguments**:
  - ARG1: UB source address (0-255)
  - ARG2: Transfer length high byte
  - ARG3: Transfer length low byte
- **Flags**:
  - F[0]: Element size (0=8-bit, 1=16-bit)
  - F[1]: Reserved
- **Timing**: 1 cycle initiation + DMA transfer time
- **Use Case**: Sending results back to host
- **Example**: `WR_HOST_MEM 0x20, 0x01, 0x00, 0b00` (write 256 bytes from UB[0x20])

#### `0x03: RD_WEIGHT` - Read Weight Data
- **Arguments**:
  - ARG1: Starting tile number (0-255)
  - ARG2: Number of tiles to load (1-4)
  - ARG3: Reserved
- **Flags**:
  - F[0]: Buffer selection (0=buffer0, 1=buffer1)
  - F[1]: Reserved
- **Timing**: 1 cycle + DRAM access time + FIFO write time
- **Use Case**: Loading neural network weights
- **Example**: `RD_WEIGHT 0x00, 0x04, 0x00, 0b00` (load tiles 0-3 into buffer 0)

#### `0x04: LD_UB` - Load from Unified Buffer
- **Arguments**:
  - ARG1: UB address (0-255)
  - ARG2: Number of blocks to read (1-256)
  - ARG3: Reserved
- **Flags**: Reserved
- **Timing**: 1 cycle setup
- **Use Case**: Prepare activation data for matrix multiply
- **Example**: `LD_UB 0x00, 0x04, 0x00, 0b00` (load 4 blocks from UB[0])

#### `0x05: ST_UB` - Store to Unified Buffer
- **Arguments**:
  - ARG1: UB address (0-255)
  - ARG2: Number of blocks to write (1-256)
  - ARG3: Reserved
- **Flags**: Reserved
- **Timing**: 1 cycle setup
- **Use Case**: Prepare output buffer for activation results
- **Example**: `ST_UB 0x20, 0x04, 0x00, 0b00` (store 4 blocks to UB[0x20])

---

### Compute Instructions

#### `0x10: MATMUL` - Matrix Multiplication
- **Arguments**:
  - ARG1: UB input address (activations)
  - ARG2: Accumulator output address
  - ARG3: Number of input rows (batch size)
- **Flags**:
  - F[0]: Transpose input (0=no, 1=yes)
  - F[1]: Signed arithmetic (0=unsigned, 1=signed)
- **Timing**: 1 cycle initiation + ARG3 cycles compute + 2 cycles drain
- **Use Case**: Core neural network computation
- **Example**: `MATMUL 0x00, 0x20, 16, 0b00` (UB[0] × Weights → Acc[0x20], 16 rows)

#### `0x11: CONV2D` - 2D Convolution
- **Arguments**:
  - ARG1: UB input address
  - ARG2: Accumulator output address
  - ARG3: Configuration (kernel size, stride)
- **Flags**:
  - F[0]: Padding mode
  - F[1]: Signed arithmetic
- **Timing**: Variable based on kernel size and input dimensions
- **Use Case**: Convolutional neural networks
- **Example**: `CONV2D 0x00, 0x20, 0x03, 0b00` (3x3 convolution)

#### `0x12: MATMUL_ACC` - Accumulate Matrix Multiplication
- **Arguments**:
  - ARG1: UB input address
  - ARG2: Accumulator address (read/write)
  - ARG3: Number of input rows
- **Flags**:
  - F[0]: Transpose input
  - F[1]: Signed arithmetic
- **Timing**: Same as MATMUL but no clear overhead
- **Use Case**: Tiled matrix multiply, residual connections
- **Example**: `MATMUL_ACC 0x00, 0x20, 16, 0b00` (Acc[0x20] += UB[0] × Weights)

#### `0x22: ADD_BIAS` - Add Bias Vector
- **Arguments**:
  - ARG1: Accumulator input address
  - ARG2: UB output address (bias data location)
  - ARG3: Number of elements
- **Flags**: Reserved
- **Timing**: Variable
- **Use Case**: Add bias to accumulator results
- **Example**: `ADD_BIAS 0x20, 0x40, 64, 0b00` (add bias from UB[0x40] to Acc[0x20])

#### `0x23: BATCH_NORM` - Batch Normalization
- **Arguments**:
  - ARG1: UB input address
  - ARG2: UB output address
  - ARG3: Config register base index
- **Flags**: Reserved
- **Timing**: Variable
- **Use Case**: Batch normalization layer
- **Example**: `BATCH_NORM 0x00, 0x20, 0x00, 0b00` (batch norm using cfg[0])

---

### Activation Instructions

#### `0x18: RELU` - Rectified Linear Unit
- **Arguments**:
  - ARG1: Accumulator input address
  - ARG2: UB output address
  - ARG3: Number of elements to process
- **Flags**:
  - F[0]: Leak factor select (0=default, 1=config register)
  - F[1]: Reserved
- **Timing**: 1 cycle + ceil(ARG3/256) cycles + 1 cycle drain
- **Use Case**: Standard ReLU activation
- **Example**: `RELU 0x20, 0x40, 64, 0b00` (ReLU on Acc[0x20] → UB[0x40], 64 elements)

#### `0x19: RELU6` - ReLU6 Activation
- **Arguments**: Same as RELU
- **Flags**: Same as RELU
- **Timing**: Same as RELU
- **Use Case**: MobileNet-style activations
- **Example**: `RELU6 0x20, 0x40, 64, 0b00` (ReLU6: min(max(0, x), 6))

#### `0x1A: SIGMOID` - Sigmoid Activation
- **Arguments**: Same as RELU
- **Flags**: Same as RELU
- **Timing**: Same as RELU
- **Use Case**: Binary classification outputs
- **Example**: `SIGMOID 0x20, 0x40, 64, 0b00` (Sigmoid: 1/(1+e^(-x)))

#### `0x1B: TANH` - Hyperbolic Tangent
- **Arguments**: Same as RELU
- **Flags**: Same as RELU
- **Timing**: Same as RELU
- **Use Case**: LSTM/GRU gates, normalization
- **Example**: `TANH 0x20, 0x40, 64, 0b00` (Tanh: tanh(x))

---

### Pooling Instructions

#### `0x20: MAXPOOL` - Max Pooling
- **Arguments**:
  - ARG1: UB input address
  - ARG2: UB output address
  - ARG3: Reserved
- **Flags**: F[0:1]: Window size and stride (from config registers)
- **Timing**: Variable
- **Use Case**: Spatial downsampling
- **Example**: `MAXPOOL 0x00, 0x20, 0x00, 0b00` (max pooling)

#### `0x21: AVGPOOL` - Average Pooling
- **Arguments**: Same as MAXPOOL
- **Flags**: Same as MAXPOOL
- **Timing**: Same as MAXPOOL
- **Use Case**: Spatial downsampling
- **Example**: `AVGPOOL 0x00, 0x20, 0x00, 0b00` (average pooling)

---

## Instruction Format

All instructions are 32 bits:

```
┌─────────────────────────────────────────────────────────────┐
│ [31:26] │ [25:18] │ [17:10] │ [9:2] │ [1:0] │
│ opcode  │  arg1   │  arg2   │ arg3  │ flags │
│ (6-bit) │ (8-bit) │ (8-bit) │(8-bit)│(2-bit)│
└─────────────────────────────────────────────────────────────┘
```

### Encoding Example

```python
# Python encoding helper
def encode_instruction(opcode, arg1, arg2, arg3, flags=0):
    """Encode instruction into 32-bit word"""
    return (opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags

# Example: MATMUL instruction
# MATMUL 0x00, 0x20, 16, 0b00
instr = encode_instruction(0x10, 0x00, 0x20, 16, 0b00)
# Result: 0x40002040
```

---

## Opcode Ranges

| Range | Category | Instructions |
|-------|----------|--------------|
| `0x00-0x0F` | Control/Memory | NOP, RD_HOST_MEM, WR_HOST_MEM, RD_WEIGHT, LD_UB, ST_UB |
| `0x10-0x17` | Compute | MATMUL, CONV2D, MATMUL_ACC |
| `0x18-0x1F` | Activation | RELU, RELU6, SIGMOID, TANH |
| `0x20-0x2F` | Pooling/Compute | MAXPOOL, AVGPOOL, ADD_BIAS, BATCH_NORM |
| `0x30-0x3E` | Control | SYNC, CFG_REG |
| `0x3F` | Control | HALT |

---

## Python Constants

```python
# Opcode constants (for use in Python drivers)
OP_NOP          = 0x00
OP_RD_HOST_MEM  = 0x01
OP_WR_HOST_MEM  = 0x02
OP_RD_WEIGHT    = 0x03
OP_LD_UB        = 0x04
OP_ST_UB        = 0x05
OP_MATMUL       = 0x10
OP_CONV2D       = 0x11
OP_MATMUL_ACC   = 0x12
OP_RELU         = 0x18
OP_RELU6        = 0x19
OP_SIGMOID      = 0x1A
OP_TANH         = 0x1B
OP_MAXPOOL      = 0x20
OP_AVGPOOL      = 0x21
OP_ADD_BIAS     = 0x22
OP_BATCH_NORM   = 0x23
OP_SYNC         = 0x30
OP_CFG_REG      = 0x31
OP_HALT         = 0x3F
```

---

## SystemVerilog Constants

```systemverilog
// Opcode definitions (6-bit)
localparam [5:0] NOP_OP         = 6'h00;
localparam [5:0] RD_HOST_MEM_OP = 6'h01;
localparam [5:0] WR_HOST_MEM_OP = 6'h02;
localparam [5:0] RD_WEIGHT_OP   = 6'h03;
localparam [5:0] LD_UB_OP       = 6'h04;
localparam [5:0] ST_UB_OP       = 6'h05;
localparam [5:0] MATMUL_OP      = 6'h10;
localparam [5:0] CONV2D_OP      = 6'h11;
localparam [5:0] MATMUL_ACC_OP  = 6'h12;
localparam [5:0] RELU_OP        = 6'h18;
localparam [5:0] RELU6_OP       = 6'h19;
localparam [5:0] SIGMOID_OP     = 6'h1A;
localparam [5:0] TANH_OP        = 6'h1B;
localparam [5:0] MAXPOOL_OP     = 6'h20;
localparam [5:0] AVGPOOL_OP     = 6'h21;
localparam [5:0] ADD_BIAS_OP    = 6'h22;
localparam [5:0] BATCH_NORM_OP  = 6'h23;
localparam [5:0] SYNC_OP        = 6'h30;
localparam [5:0] CFG_REG_OP     = 6'h31;
localparam [5:0] HALT_OP        = 6'h3F;
```

---

## Quick Lookup by Category

### Memory Operations
- `0x01` RD_HOST_MEM - DMA read from host
- `0x02` WR_HOST_MEM - DMA write to host
- `0x03` RD_WEIGHT - Load weights from DRAM
- `0x04` LD_UB - Load from Unified Buffer
- `0x05` ST_UB - Store to Unified Buffer

### Matrix Operations
- `0x10` MATMUL - Matrix multiply
- `0x11` CONV2D - 2D convolution
- `0x12` MATMUL_ACC - Accumulate matrix multiply

### Activation Functions
- `0x18` RELU - ReLU activation
- `0x19` RELU6 - ReLU6 activation
- `0x1A` SIGMOID - Sigmoid activation
- `0x1B` TANH - Tanh activation

### Pooling Operations
- `0x20` MAXPOOL - Max pooling
- `0x21` AVGPOOL - Average pooling

### Other Operations
- `0x22` ADD_BIAS - Add bias vector
- `0x23` BATCH_NORM - Batch normalization

### Control Operations
- `0x00` NOP - No operation
- `0x30` SYNC - Synchronize operations
- `0x31` CFG_REG - Configure register
- `0x3F` HALT - Program termination

---

## Notes

- All opcodes are 6-bit (values 0x00-0x3F)
- Total of 20 implemented instructions
- Remaining opcodes (0x06-0x0F, 0x13-0x17, 0x1C-0x1F, 0x24-0x2F, 0x32-0x3E) are reserved for future use
- Instruction format is consistent across all opcodes: `[opcode:6][arg1:8][arg2:8][arg3:8][flags:2]`

