# TPU Coprocessor Driver System

Complete UART streaming driver system for using the TPU as a coprocessor on Basys 3 FPGA.

## Overview

This driver system provides:
- **32-bit instruction encoding/decoding** for all 20 ISA instructions
- **Double-buffered instruction streaming** via UART
- **Flow control and backpressure** handling
- **Status monitoring** and error handling
- **High-level instruction builders** for easy program creation

## Files

- **`tpu_coprocessor_driver.py`** - Main driver class with streaming support
- **`instruction_encoder.py`** - Instruction encoding/decoding utilities
- **`example_coprocessor_usage.py`** - Usage examples

## Quick Start

### Basic Usage

```python
from tpu_coprocessor_driver import TPU_Coprocessor, Opcode

# Connect to FPGA
tpu = TPU_Coprocessor(port='/dev/ttyUSB0')

try:
    # Enable streaming mode
    tpu.enable_stream_mode()
    
    # Stream instructions
    instructions = [
        (Opcode.NOP, 0, 0, 0, 0),
        (Opcode.MATMUL, 0, 0x20, 4, 0),
        (Opcode.HALT, 0, 0, 0, 0)
    ]
    
    tpu.stream_instructions(instructions)
    
finally:
    tpu.close()
```

## Instruction Format

All instructions are 32 bits:

```
[31:26] opcode  (6-bit)
[25:18] arg1    (8-bit)
[17:10] arg2    (8-bit)
[9:2]   arg3    (8-bit)
[1:0]   flags   (2-bit)
```

## All 20 Instructions

| Opcode | Instruction | Category |
|--------|-------------|----------|
| `0x00` | NOP | Control |
| `0x01` | RD_HOST_MEM | Memory |
| `0x02` | WR_HOST_MEM | Memory |
| `0x03` | RD_WEIGHT | Memory |
| `0x04` | LD_UB | Memory |
| `0x05` | ST_UB | Memory |
| `0x10` | MATMUL | Compute |
| `0x11` | CONV2D | Compute |
| `0x12` | MATMUL_ACC | Compute |
| `0x18` | RELU | Activation |
| `0x19` | RELU6 | Activation |
| `0x1A` | SIGMOID | Activation |
| `0x1B` | TANH | Activation |
| `0x20` | MAXPOOL | Pooling |
| `0x21` | AVGPOOL | Pooling |
| `0x22` | ADD_BIAS | Compute |
| `0x23` | BATCH_NORM | Compute |
| `0x30` | SYNC | Control |
| `0x31` | CFG_REG | Control |
| `0x3F` | HALT | Control |

## API Reference

### TPU_Coprocessor Class

#### Connection

```python
tpu = TPU_Coprocessor(port='/dev/ttyUSB0', baud=115200, timeout=2, verbose=True)
tpu.close()
```

#### Streaming Mode

```python
# Enable double-buffered streaming
tpu.enable_stream_mode()

# Stream single instruction
tpu.stream_instruction(Opcode.MATMUL, 0, 0x20, 4, 0)

# Stream multiple instructions
instructions = [(Opcode.NOP, 0, 0, 0, 0), ...]
tpu.stream_instructions(instructions, flow_control=True)

# Disable streaming
tpu.disable_stream_mode()
```

#### Memory Operations

```python
# Unified Buffer
tpu.write_ub(addr, data)  # data: numpy array or bytes
data = tpu.read_ub(addr, length)

# Weight Memory
tpu.write_weights(addr, data)
```

#### Status Monitoring

```python
status = tpu.read_status()
# Returns TPUStatus object with sys_busy, vpu_busy, etc.

tpu.wait_done(timeout=10.0)  # Wait for completion
```

#### Statistics

```python
stats = tpu.get_stats()
tpu.print_stats()
```

### Instruction Encoder

```python
from instruction_encoder import encode_instruction, decode_instruction

# Encode
word = encode_instruction(Opcode.MATMUL, 0, 0x20, 4, 0)

# Decode
opcode, arg1, arg2, arg3, flags = decode_instruction(word)

# Format
from instruction_encoder import format_instruction
print(format_instruction(word))
```

## Examples

### Example 1: Simple Streaming

```python
tpu = TPU_Coprocessor()
tpu.enable_stream_mode()

instructions = [
    (Opcode.NOP, 0, 0, 0, 0),
    (Opcode.CFG_REG, 0, 0x06, 0x66, 0),
    (Opcode.HALT, 0, 0, 0, 0)
]

tpu.stream_instructions(instructions)
tpu.close()
```

### Example 2: Matrix Multiply

```python
tpu = TPU_Coprocessor()

# Load data
activations = np.ones((4, 256), dtype=np.uint8) * 2
weights = np.ones((256, 256), dtype=np.uint8) * 3
tpu.write_ub(0, activations)
tpu.write_weights(0, weights)

# Stream computation
tpu.enable_stream_mode()
instructions = [
    (Opcode.MATMUL, 0, 0x20, 4, 0),
    (Opcode.SYNC, 0x01, 0x00, 0x10, 0),
    (Opcode.RELU, 0x20, 0x40, 4, 0),
    (Opcode.HALT, 0, 0, 0, 0)
]
tpu.stream_instructions(instructions)

# Wait and read results
tpu.wait_done()
results = tpu.read_ub(0x40, 256)
tpu.close()
```

### Example 3: Using Instruction Builders

```python
from instruction_encoder import build_matmul, build_relu, build_sync

# Build instructions
instr1 = build_matmul(ub_addr=0, acc_addr=0x20, rows=4, transpose=False, signed=False)
instr2 = build_sync(mask=0x01, timeout=4096)
instr3 = build_relu(acc_addr=0x20, ub_out_addr=0x40, length=4)

# Encode to bytes
from instruction_encoder import instruction_to_bytes
bytes1 = instruction_to_bytes(instr1)
```

## UART Protocol

### Command Codes

- `0x01` - WRITE_UB: Write to Unified Buffer
- `0x02` - WRITE_WT: Write to Weight Memory
- `0x03` - WRITE_INSTR: Write instruction to buffer (legacy)
- `0x04` - READ_UB: Read from Unified Buffer
- `0x05` - EXECUTE: Start execution (legacy)
- `0x06` - READ_STATUS: Read TPU status
- `0x07` - STREAM_INSTR: Enter streaming mode (double-buffered)

### Streaming Protocol

1. **Enable Streaming:**
   ```
   Host → 0x07 (STREAM_INSTR)
   FPGA → 0x00 (ready) or 0xFF (not ready) + buffer select
   ```

2. **Stream Instruction:**
   ```
   Host → [32-bit instruction] (4 bytes, big-endian)
   FPGA → 0x00 (accepted) or 0xFF (buffer full) or 0x01 (buffer swapped)
   ```

3. **Flow Control:**
   - If FPGA responds `0xFF`, host should wait and retry
   - If FPGA responds `0x01`, buffer has swapped (continue streaming)

## Error Handling

```python
try:
    tpu = TPU_Coprocessor()
    tpu.enable_stream_mode()
    # ... operations ...
except serial.SerialException as e:
    print(f"Serial error: {e}")
except RuntimeError as e:
    print(f"Runtime error: {e}")
finally:
    tpu.close()
```

## Performance Tips

1. **Use streaming mode** for continuous instruction execution
2. **Enable flow control** to handle backpressure automatically
3. **Batch instructions** using `stream_instructions()` for better throughput
4. **Monitor status** with `wait_done()` for synchronization
5. **Check statistics** with `get_stats()` for debugging

## Troubleshooting

### "Streaming mode not enabled"
- Call `enable_stream_mode()` before streaming instructions

### "Buffer full" errors
- Enable flow control: `stream_instructions(..., flow_control=True)`
- Reduce instruction rate
- Check FPGA buffer status

### "No response from FPGA"
- Verify FPGA is programmed and powered on
- Check serial port: `ls /dev/tty*`
- Verify baud rate matches (115200)

### Connection issues
- Check USB cable connection
- Verify FTDI drivers installed
- Try different serial port

## See Also

- `OPCODE_REFERENCE.md` - Complete opcode reference
- `ISA_Reference.md` - Full ISA documentation
- `FPGA_PROGRAMMING_GUIDE.md` - FPGA setup guide

