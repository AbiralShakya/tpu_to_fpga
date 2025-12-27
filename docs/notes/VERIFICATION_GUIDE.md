# ISA Test Verification Guide

## Problem with Current Tests

The original `test_isa_legacy.py` only checks that instructions **complete**, not that they produce **correct results**. For example:

```python
# ❌ OLD: Only checks completion
tpu.write_instruction(0, Opcode.MATMUL, ...)
tpu.start_execution()
status = tpu.read_status()  # Just checks if done
print("✓ MATMUL test passed")  # But we don't know if result is correct!
```

## Solution: Result Verification

### New Test Script: `test_isa_legacy_verified.py`

This script actually **verifies results**:

1. **Write/Read UB Test**: Writes known data, reads it back, verifies match
2. **MATMUL Test**: 
   - Writes known input matrices (e.g., 2×2 identity test)
   - Executes MATMUL
   - Uses ST_UB to move results from accumulator to UB
   - Reads results and compares against expected values

### Example: Verified MATMUL Test

```python
# 1. Prepare known input
input_matrix = np.array([[1, 2], [3, 4]], dtype=np.uint8)
weight_matrix = np.array([[1, 0], [0, 1]], dtype=np.uint8)  # Identity

# 2. Write to TPU
tpu.write_ub(0, input_matrix)
tpu.write_weights(0, weight_matrix)

# 3. Execute: MATMUL → ST_UB (move results to UB)
tpu.write_instruction(0, Opcode.MATMUL, 0, 0x00, 2, 0)      # Compute
tpu.write_instruction(1, Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0)  # Wait
tpu.write_instruction(2, Opcode.ST_UB, 0x00, 0x20, 0x02, 0)  # Store results
tpu.write_instruction(3, Opcode.HALT, 0, 0, 0, 0)
tpu.start_execution()

# 4. Read and verify results
result_data = tpu.read_ub(0x20, 64)
result_matrix = extract_2x2(result_data)
expected_matrix = np.array([[1, 2], [3, 4]])  # Identity × input = input

if np.array_equal(expected_matrix, result_matrix):
    print("✓ MATMUL result is CORRECT!")
else:
    print("✗ MATMUL result is WRONG!")
```

## Running Verified Tests

```bash
# Basic tests (completion only)
python3 python/test_isa_legacy.py /dev/tty.usbserial-XXXX

# Verified tests (checks actual results)
python3 python/test_isa_legacy_verified.py /dev/tty.usbserial-XXXX
```

## What Gets Verified

| Test | What's Verified |
|------|----------------|
| Write/Read UB | Data integrity (write → read → compare) |
| LD_UB | UB data preserved after load |
| MATMUL | Actual matrix multiplication results |
| MATMUL + ST_UB | Full pipeline with result verification |

## Limitations

1. **Accumulator Read**: Accumulator results aren't directly readable via UART. Use `ST_UB` to move results to UB first.

2. **Weight Memory Read**: Weight memory might not be directly readable. Tests verify write/load operations complete.

3. **Complex Operations**: Some operations (CONV2D, pooling) require more complex verification logic.

## Next Steps

1. **Add more test cases**: Larger matrices, edge cases
2. **Add tolerance**: For floating-point operations (if any)
3. **Add performance metrics**: Measure execution time
4. **Add stress tests**: Multiple operations in sequence

