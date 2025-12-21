# TPU v1 Instruction Set Architecture (ISA) - Complete Reference

## Table of Contents
1. [Instruction Format](#instruction-format)
2. [Control Signal Architecture](#control-signal-architecture)
3. [Instruction Set Details](#instruction-set-details)
4. [Pipeline Behavior](#pipeline-behavior)
5. [Programming Examples](#programming-examples)
6. [Timing and Performance](#timing-and-performance)

---

## Instruction Format

### 32-Bit Instruction Word Layout

```
┌────────────────────────────────────────────────────────────────────────────────┐
│ [31:26] │ [25:18] │ [17:10] │ [9:2] │ [1:0] │
│ opcode  │  arg1   │  arg2   │ arg3  │ flags │  ← 32-bit instruction word
│ (6-bit) │ (8-bit) │ (8-bit) │(8-bit)│(2-bit)│
│ 0-63    │ 0-255   │ 0-255   │ 0-255 │ 0-3   │
└────────────────────────────────────────────────────────────────────────────────┘
```

### Field Definitions

#### **Opcode (6-bit, bits 31:26)**
- **Range**: 0x00 to 0x3F (64 possible instructions)
- **Implemented**: 20 instructions (0x00-0x3F range)
- **Encoding**: Direct binary encoding

#### **Arguments (8-bit each, bits 25:18, 17:10, 9:2)**
- **ARG1**: Primary operand/address (memory addresses, accumulator indices)
- **ARG2**: Secondary operand/address (destination addresses, counts)
- **ARG3**: Tertiary operand (lengths, counts, configuration values)

#### **Flags (2-bit, bits 1:0)**
- **F[0] (bit 0)**: Mode selection, buffer control, arithmetic options
- **F[1] (bit 1)**: Precision control, special modes, configuration selects

### Flag Bit Usage Table

| Instruction Type | F[0] Usage | F[1] Usage | Combined Usage |
|------------------|------------|------------|----------------|
| **Memory Ops** | Direction/Mode | Element Size | Transfer config |
| **Compute Ops** | Signed/Unsigned | Transpose | Precision + Mode |
| **Activation** | Leak Factor Select | Reserved | Config register index |
| **Control** | Buffer Select | Timeout Enable | Mask configuration |

---

## Control Signal Architecture

### Controller Output Signals (44 total)

#### **Program Counter Control (1 signal)**
- `pc_cnt`: Increment PC (1) or stall (0)

#### **Unified Buffer Control (6 signals)**
- `ub_port_a_rd_en`: Port A read enable
- `ub_port_a_addr[7:0]`: Port A read address
- `ub_port_b_wr_en`: Port B write enable
- `ub_port_b_addr[7:0]`: Port B write address
- `ub_rd_count[7:0]`: Read burst count
- `ub_wr_count[7:0]`: Write burst count

#### **DMA Control (5 signals)**
- `dma_start`: Initiate DMA transfer
- `dma_dir`: Direction (0=host→TPU, 1=TPU→host)
- `dma_ub_addr[7:0]`: UB block address
- `dma_length[15:0]`: Transfer length in bytes
- `dma_elem_sz[1:0]`: Element size (00=8b, 01=16b, 10=32b)

#### **Weight FIFO Control (4 signals)**
- `wt_mem_rd_en`: Read from weight DRAM
- `wt_mem_addr[23:0]`: DRAM address
- `wt_fifo_wr`: Write to weight FIFO
- `wt_num_tiles[7:0]`: Number of tiles to load
- `wt_buf_sel`: Buffer selection (0 or 1)

#### **Systolic Array Control (7 signals)**
- `sys_start`: Begin matrix operation
- `sys_mode[1:0]`: Operation mode (00=MatMul, 01=Conv2D, 10=Accumulate)
- `sys_rows[7:0]`: Batch size (input rows)
- `sys_signed`: Signed/unsigned arithmetic
- `sys_transpose`: Transpose input matrix
- `sys_acc_addr[7:0]`: Accumulator write address
- `sys_acc_clear`: Clear accumulator before write

#### **Accumulator Control (4 signals)**
- `acc_wr_en`: Write enable
- `acc_rd_en`: Read enable
- `acc_addr[7:0]`: Read/write address
- `acc_buf_sel`: Buffer selection (0 or 1)

#### **VPU Control (6 signals)**
- `vpu_start`: Begin VPU operation
- `vpu_mode[3:0]`: Function selection
- `vpu_in_addr[7:0]`: Input address (accumulator)
- `vpu_out_addr[7:0]`: Output address (UB)
- `vpu_length[7:0]`: Number of elements to process
- `vpu_param[15:0]`: Operation parameter

#### **Sync Control (3 signals)**
- `sync_wait`: Stall until completion
- `sync_mask[3:0]`: Units to wait for (bit mask)
- `sync_timeout[15:0]`: Maximum wait cycles

#### **Configuration Control (3 signals)**
- `cfg_wr_en`: Write enable
- `cfg_addr[7:0]`: Register address
- `cfg_data[15:0]`: Data to write

---

## Instruction Set Details

### **0x00: NOP - No Operation**

**Description**: Performs no operation, increments PC.

**Arguments**:
- ARG1, ARG2, ARG3: Ignored
- FLAGS: Ignored

**Control Signals**:
```
pc_cnt = 1
(All other signals = 0)
```

**Timing**: 1 cycle
**Pipeline**: No hazards
**Use Case**: Padding, timing alignment

---

### **0x01: RD_HOST_MEM - Read from Host Memory**

**Description**: Initiates DMA transfer from host memory to Unified Buffer.

**Arguments**:
- ARG1: UB destination address (0-255)
- ARG2: Transfer length high byte
- ARG3: Transfer length low byte
- FLAGS[F[0]]: Element size (0=8-bit, 1=16-bit)
- FLAGS[F[1]]: Burst mode (0=single, 1=burst)

**Control Signals**:
```
pc_cnt = 1
dma_start = 1
dma_dir = 0 (host→TPU)
dma_ub_addr[7:0] = ARG1
dma_length[15:0] = {ARG2, ARG3}
dma_elem_sz[1:0] = FLAGS[1:0]
```

**Timing**: 1 cycle initiation + DMA transfer time
**Pipeline**: May cause DMA hazard
**Use Case**: Loading input activations from host

**Example**:
```systemverilog
RD_HOST_MEM 0x00, 0x04, 0x00, 0b00  // Read 1024 bytes to UB[0], 8-bit elements
```

---

### **0x02: WR_HOST_MEM - Write to Host Memory**

**Description**: Initiates DMA transfer from Unified Buffer to host memory.

**Arguments**:
- ARG1: UB source address (0-255)
- ARG2: Transfer length high byte
- ARG3: Transfer length low byte
- FLAGS[F[0]]: Element size (0=8-bit, 1=16-bit)
- FLAGS[F[1]]: Reserved

**Control Signals**:
```
pc_cnt = 1
dma_start = 1
dma_dir = 1 (TPU→host)
dma_ub_addr[7:0] = ARG1
dma_length[15:0] = {ARG2, ARG3}
dma_elem_sz[1:0] = FLAGS[1:0]
```

**Timing**: 1 cycle initiation + DMA transfer time
**Pipeline**: May cause DMA hazard
**Use Case**: Sending results back to host

---

### **0x03: RD_WEIGHT - Read Weight Data**

**Description**: Loads weight tiles from off-chip DRAM into weight FIFO buffer.

**Arguments**:
- ARG1: Starting tile number (0-255)
- ARG2: Number of tiles to load (1-4)
- ARG3: Reserved
- FLAGS[F[0]]: Buffer selection (0=buffer0, 1=buffer1)
- FLAGS[F[1]]: Reserved

**Control Signals**:
```
pc_cnt = 1
wt_mem_rd_en = 1
wt_mem_addr[23:0] = ARG1 * 65536  // Tile base address
wt_fifo_wr = 1
wt_num_tiles[7:0] = ARG2
wt_buf_sel = FLAGS[0]
```

**Timing**: 1 cycle + DRAM access time + FIFO write time
**Pipeline**: May cause weight FIFO hazard
**Use Case**: Loading neural network weights

**Example**:
```systemverilog
RD_WEIGHT 0x00, 0x04, 0x00, 0b00  // Load tiles 0-3 into buffer 0
```

---

### **0x04: LD_UB - Load from Unified Buffer**

**Description**: Sets up Unified Buffer for systolic array read operations.

**Arguments**:
- ARG1: UB address (0-255)
- ARG2: Number of blocks to read (1-256)
- ARG3: Reserved
- FLAGS: Reserved

**Control Signals**:
```
pc_cnt = 1
ub_port_a_rd_en = 1
ub_port_a_addr[7:0] = ARG1
ub_rd_count[7:0] = ARG2
```

**Timing**: 1 cycle setup
**Pipeline**: Prepares for systolic read
**Use Case**: Prepare activation data for matrix multiply

---

### **0x05: ST_UB - Store to Unified Buffer**

**Description**: Sets up Unified Buffer for VPU write operations.

**Arguments**:
- ARG1: UB address (0-255)
- ARG2: Number of blocks to write (1-256)
- ARG3: Reserved
- FLAGS: Reserved

**Control Signals**:
```
pc_cnt = 1
ub_port_b_wr_en = 1
ub_port_b_addr[7:0] = ARG1
ub_wr_count[7:0] = ARG2
```

**Timing**: 1 cycle setup
**Pipeline**: Prepares for VPU write
**Use Case**: Prepare output buffer for activation results

---

### **0x10: MATMUL - Matrix Multiplication**

**Description**: Performs matrix multiplication: `Acc[ARG2] = UB[ARG1] × Weights`

**Arguments**:
- ARG1: UB input address (activations)
- ARG2: Accumulator output address
- ARG3: Number of input rows (batch size)
- FLAGS[F[0]]: Transpose input (0=no, 1=yes)
- FLAGS[F[1]]: Signed arithmetic (0=unsigned, 1=signed)

**Control Signals**:
```
pc_cnt = 1
ub_port_a_rd_en = 1
ub_port_a_addr[7:0] = ARG1
acc_wr_en = 1
acc_addr[7:0] = ARG2
sys_start = 1
sys_mode[1:0] = 00 (matrix multiply)
sys_rows[7:0] = ARG3
sys_signed = FLAGS[1]
sys_transpose = FLAGS[0]
sys_acc_addr[7:0] = ARG2
sys_acc_clear = 1
wt_fifo_rd_en = 1
acc_buf_sel = current_buffer
```

**Timing**: 1 cycle initiation + ARG3 cycles compute + 2 cycles drain
**Pipeline**: Causes systolic hazard
**Use Case**: Core neural network computation

**Example**:
```systemverilog
MATMUL 0x00, 0x20, 16, 0b00  // Acc[0x20] = UB[0] × Weights, 16 rows, unsigned
```

---

### **0x11: CONV2D - 2D Convolution**

**Description**: Performs 2D convolution using im2col transformation.

**Arguments**:
- ARG1: UB input address
- ARG2: Accumulator output address
- ARG3: Configuration (kernel size, stride)
- FLAGS[F[0]]: Padding mode
- FLAGS[F[1]]: Signed arithmetic

**Control Signals**:
```
pc_cnt = 1
ub_port_a_rd_en = 1
ub_port_a_addr[7:0] = ARG1
acc_wr_en = 1
sys_start = 1
sys_mode[1:0] = 01 (convolution)
sys_rows[7:0] = derived from ARG3
sys_signed = FLAGS[1]
sys_acc_addr[7:0] = ARG2
sys_acc_clear = 1
wt_fifo_rd_en = 1
acc_buf_sel = current_buffer
```

**Timing**: Variable based on kernel size and input dimensions
**Use Case**: Convolutional neural networks

---

### **0x12: MATMUL_ACC - Accumulate Matrix Multiplication**

**Description**: Matrix multiply with accumulation: `Acc[ARG2] += UB[ARG1] × Weights`

**Arguments**:
- ARG1: UB input address
- ARG2: Accumulator address (read/write)
- ARG3: Number of input rows
- FLAGS[F[0]]: Transpose input
- FLAGS[F[1]]: Signed arithmetic

**Control Signals**:
```
pc_cnt = 1
ub_port_a_rd_en = 1
ub_port_a_addr[7:0] = ARG1
acc_rd_en = 1
acc_wr_en = 1
acc_addr[7:0] = ARG2
sys_start = 1
sys_mode[1:0] = 10 (accumulate mode)
sys_rows[7:0] = ARG3
sys_signed = FLAGS[1]
sys_transpose = FLAGS[0]
sys_acc_addr[7:0] = ARG2
sys_acc_clear = 0  // Don't clear - accumulate!
wt_fifo_rd_en = 1
acc_buf_sel = current_buffer
```

**Timing**: Same as MATMUL but no clear overhead
**Use Case**: Tiled matrix multiply, residual connections

---

### **0x18: RELU - Rectified Linear Unit**

**Description**: Applies ReLU activation: `out = max(0, in)`

**Arguments**:
- ARG1: Accumulator input address
- ARG2: UB output address
- ARG3: Number of elements to process
- FLAGS[F[0]]: Leak factor select (0=default, 1=config register)
- FLAGS[F[1]]: Reserved

**Control Signals**:
```
pc_cnt = 1
ub_port_b_wr_en = 1
acc_rd_en = 1
vpu_start = 1
vpu_mode[3:0] = 0001 (ReLU)
vpu_in_addr[7:0] = ARG1
vpu_out_addr[7:0] = ARG2
vpu_length[7:0] = ARG3
vpu_param[15:0] = FLAGS[0] ? cfg_reg[FLAGS[1:0]] : 0
ub_port_b_addr[7:0] = ARG2
acc_addr[7:0] = ARG1
acc_buf_sel = !current_buffer  // Read from OTHER buffer!
```

**Timing**: 1 cycle + ceil(ARG3/256) cycles + 1 cycle drain
**Pipeline**: Causes VPU hazard
**Use Case**: Standard ReLU activation

**Example**:
```systemverilog
RELU 0x20, 0x40, 64, 0b00  // ReLU on Acc[0x20] → UB[0x40], 64 elements
```

---

### **0x19: RELU6 - ReLU6 Activation**

**Description**: Applies ReLU6: `out = min(max(0, in), 6.0)`

**Arguments**: Same as RELU
**Control Signals**: Same as RELU but `vpu_mode[3:0] = 0010`
**Use Case**: MobileNet-style activations

---

### **0x1A: SIGMOID - Sigmoid Activation**

**Description**: Applies sigmoid: `out = 1 / (1 + e^(-x))`

**Arguments**: Same as RELU
**Control Signals**: Same as RELU but `vpu_mode[3:0] = 0011`
**Use Case**: Binary classification outputs

---

### **0x1B: TANH - Hyperbolic Tangent**

**Description**: Applies tanh: `out = tanh(x)`

**Arguments**: Same as RELU
**Control Signals**: Same as RELU but `vpu_mode[3:0] = 0100`
**Use Case**: LSTM/GRU gates, normalization

---

### **0x20: MAXPOOL - Max Pooling**

**Description**: Applies max pooling operation.

**Arguments**:
- ARG1: UB input address
- ARG2: UB output address
- ARG3: Reserved
- FLAGS[F[0:1]]: Window size and stride (from config registers)

**Control Signals**:
```
pc_cnt = 1
ub_port_a_rd_en = 1
ub_port_b_wr_en = 1
vpu_start = 1
vpu_mode[3:0] = 0110 (Max Pool)
vpu_in_addr[7:0] = ARG1
vpu_out_addr[7:0] = ARG2
ub_port_a_addr[7:0] = ARG1
ub_port_b_addr[7:0] = ARG2
```

**Use Case**: Spatial downsampling

---

### **0x21: AVGPOOL - Average Pooling**

**Description**: Applies average pooling operation.

**Arguments**: Same as MAXPOOL
**Control Signals**: Same as MAXPOOL but `vpu_mode[3:0] = 0111`

---

### **0x22: ADD_BIAS - Add Bias Vector**

**Description**: Adds bias vector to each row: `out[i] = in[i] + bias`

**Arguments**:
- ARG1: Accumulator input address
- ARG2: UB output address (bias data location)
- ARG3: Number of elements
- FLAGS: Reserved

**Control Signals**:
```
pc_cnt = 1
ub_port_a_rd_en = 1  // Read bias from UB
ub_port_b_wr_en = 1  // Write result to UB
acc_rd_en = 1
vpu_start = 1
vpu_mode[3:0] = 0101 (Bias Add)
vpu_in_addr[7:0] = ARG1
vpu_out_addr[7:0] = ARG2
vpu_length[7:0] = ARG3
acc_addr[7:0] = ARG1
acc_buf_sel = !current_buffer
```

---

### **0x23: BATCH_NORM - Batch Normalization**

**Description**: Applies batch normalization: `out = (in - mean) / sqrt(var + eps) * scale + shift`

**Arguments**:
- ARG1: UB input address
- ARG2: UB output address
- ARG3: Config register base index
- FLAGS: Reserved

**Control Signals**:
```
pc_cnt = 1
ub_port_a_rd_en = 1
ub_port_b_wr_en = 1
vpu_start = 1
vpu_mode[3:0] = 1000 (Batch Norm)
vpu_in_addr[7:0] = ARG1
vpu_out_addr[7:0] = ARG2
ub_port_a_addr[7:0] = ARG1
ub_port_b_addr[7:0] = ARG2
vpu_param[15:0] = ARG3  // Config base index
```

---

### **0x30: SYNC - Synchronize Operations**

**Description**: Stall pipeline until specified units complete their operations.

**Arguments**:
- ARG1: Sync mask (bit field)
- ARG2: Timeout high byte
- ARG3: Timeout low byte
- FLAGS: Reserved

**Sync Mask (ARG1) bits**:
- Bit 0: Systolic array completion
- Bit 1: VPU completion
- Bit 2: DMA completion
- Bit 3: Weight FIFO load completion

**Control Signals**:
```
pc_cnt = 0  // STALL PC!
sync_wait = 1
sync_mask[3:0] = ARG1[3:0]
sync_timeout[15:0] = {ARG2, ARG3}
```

**Timing**: Variable - until all masked units complete or timeout
**Pipeline**: Causes pipeline stall (hazard)
**Use Case**: Ensure data dependencies are met

**Example**:
```systemverilog
SYNC 0x01, 0x00, 0x10, 0b00  // Wait for systolic only, timeout 4096 cycles
SYNC 0x03, 0x10, 0x00, 0b00  // Wait for systolic + VPU, timeout 4096 cycles
```

---

### **0x31: CFG_REG - Configure Register**

**Description**: Write value to configuration register.

**Arguments**:
- ARG1: Register address (0-255)
- ARG2: Data high byte
- ARG3: Data low byte
- FLAGS: Reserved

**Control Signals**:
```
pc_cnt = 1
cfg_wr_en = 1
cfg_addr[7:0] = ARG1
cfg_data[15:0] = {ARG2, ARG3}
```

**Timing**: 1 cycle
**Use Case**: Set activation parameters, pooling configurations

**Example**:
```systemverilog
CFG_REG 0x00, 0x06, 0x66, 0b00  // Write 0x0666 (0.1) to cfg[0] for ReLU leak
```

---

### **0x3F: HALT - Program Termination**

**Description**: Stop program execution and signal completion to host.

**Arguments**: All ignored
**Control Signals**:
```
pc_cnt = 0  // Stop PC increment
halt_req = 1  // Signal completion
interrupt_en = 1  // Notify host
```

**Timing**: Program ends
**Use Case**: End of neural network inference

---

## Pipeline Behavior

### **2-Stage Pipeline Operation**

```
Cycle N:     Fetch/Decode Stage
             - Read instruction from memory
             - Decode opcode, args, flags
             - Check for hazards
             - IF/ID register update

Cycle N+1:   Execute Stage
             - Generate control signals
             - Drive datapath
             - Update PC (unless stalled)
```

### **Hazard Detection**

**Systolic Hazard**: Pipeline stalls when `sys_busy = 1`
**VPU Hazard**: Pipeline stalls when `vpu_busy = 1`
**DMA Hazard**: Pipeline stalls when `dma_busy = 1`
**Weight FIFO Hazard**: Pipeline stalls when weight loading active

### **Stall Conditions**

| Condition | Stall Duration | Resolution |
|-----------|----------------|------------|
| SYNC instruction | Variable | Units complete or timeout |
| Systolic busy | 1+ cycles | MATMUL/CONV complete |
| VPU busy | 1+ cycles | Activation complete |
| DMA active | 1+ cycles | Transfer complete |

---

## Programming Examples

### **Example 1: Simple Matrix Multiply**

```python
program = [
    # Load activations to UB[0]
    (0x01, 0x00, 0x04, 0x00, 0b00),  # RD_HOST_MEM: 1024 bytes to UB[0]
    # Load weights to FIFO buffer 0
    (0x03, 0x00, 0x04, 0x00, 0b00),  # RD_WEIGHT: tiles 0-3 to buffer 0
    # Wait for data transfer
    (0x30, 0x0C, 0x00, 0x10, 0b00),  # SYNC: wait DMA + weight load
    # Perform matrix multiply
    (0x10, 0x00, 0x20, 0x04, 0b00),  # MATMUL: 4×256 × 256×256 → Acc[0x20]
    # Wait for completion
    (0x30, 0x01, 0x00, 0x10, 0b00),  # SYNC: wait systolic
    # Send results back
    (0x02, 0x20, 0x01, 0x00, 0b00),  # WR_HOST_MEM: Acc[0x20] → host
    # Done
    (0x3F, 0x00, 0x00, 0x00, 0b00),  # HALT
]
```

### **Example 2: 2-Layer Neural Network**

```python
program = [
    # Layer 1: Input(9×256) → Hidden(9×256)
    (0x01, 0x00, 0x09, 0x00, 0b00),  # Load input activations (9 rows)
    (0x03, 0x00, 0x09, 0x00, 0b00),  # Load layer 1 weights (9 tiles)
    (0x30, 0x0C, 0x00, 0x10, 0b00),  # Wait for data
    (0x10, 0x00, 0x20, 0x09, 0b00),  # MATMUL
    (0x18, 0x20, 0x40, 0x09, 0b00),  # RELU → UB[0x40]
    (0x30, 0x02, 0x00, 0x10, 0b00),  # Wait VPU

    # Layer 2: Hidden(9×256) → Output(9×256)
    (0x03, 0x09, 0x09, 0x00, 0b01),  # Load layer 2 weights (9 tiles, parallel!)
    (0x10, 0x40, 0x60, 0x09, 0b00),  # MATMUL on ReLU outputs
    (0x18, 0x60, 0x80, 0x09, 0b00),  # RELU → final output
    (0x30, 0x02, 0x00, 0x10, 0b00),  # Wait VPU
    (0x02, 0x80, 0x15, 0x00, 0b00),  # Send results to host (9×256 bytes)
    (0x3F, 0x00, 0x00, 0x00, 0b00),  # HALT
]
```

### **Example 3: Advanced Configuration**

```python
program = [
    # Configure ReLU leak factor
    (0x31, 0x00, 0x06, 0x66, 0b00),  # CFG_REG[0] = 0x0666 (0.1)
    # Configure pooling parameters
    (0x31, 0x01, 0x00, 0x02, 0b00),  # CFG_REG[1] = 2 (2×2 pooling)
    # Load and process data
    (0x01, 0x00, 0x18, 0x00, 0b00),  # Load 6144 bytes (9×256×3 bytes approx)
    (0x03, 0x00, 0x09, 0x00, 0b00),  # Load 9 weight tiles
    (0x30, 0x0C, 0x00, 0x20, 0b00),  # Wait with longer timeout
    (0x10, 0x00, 0x20, 0x09, 0b00),  # Matrix: 9×256
    (0x18, 0x20, 0x40, 0x09, 0b01),  # Leaky ReLU using cfg[1]
    (0x20, 0x40, 0x50, 0x00, 0b00),  # Max pool using cfg[1] params
    (0x02, 0x50, 0x15, 0x00, 0b00),  # Output results (9×256 bytes)
    (0x3F, 0x00, 0x00, 0x00, 0b00),  # HALT
]
```

---

## Timing and Performance

### **Instruction Latencies**

| Instruction | Initiation | Execution | Total Cycles |
|-------------|------------|-----------|--------------|
| NOP | 1 | 0 | 1 |
| Memory Ops | 1 | 1-1000+ | Variable |
| MATMUL | 1 | N+2 | N+3 (N = rows) |
| Activations | 1 | ceil(L/256)+1 | Variable |
| SYNC | 1 | Variable | Until complete |
| CFG_REG | 1 | 0 | 1 |
| HALT | 1 | ∞ | Program end |

### **Throughput Characteristics**

- **Peak MAC Throughput**: 65,536 operations per cycle
- **Memory Bandwidth**: 256 bits/cycle (UB), 16 bits/cycle (weights)
- **Pipeline Efficiency**: ~90% with proper synchronization
- **Double-Buffer Benefit**: Zero stall time between layers

### **Resource Conflicts**

- **UB Conflicts**: Port A (systolic) vs Port B (VPU) arbitration
- **Accumulator Conflicts**: Read/write buffer selection
- **Weight FIFO**: Buffer swapping for continuous operation

This ISA provides a complete programming interface for neural network acceleration on the TPU v1 architecture! 🚀
