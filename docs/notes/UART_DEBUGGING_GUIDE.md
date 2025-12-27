# UART Communication Debugging Guide

## Current Issues

From test output, we see:
1. **Status reads failing**: `read_status()` returns `None`
2. **UB reads returning 0 bytes**: `read_ub()` gets no data
3. **TPU always busy**: Status checks fail because status is `None`

## Root Causes

### 1. Status Read Issues

**Problem**: `read_status()` returns `None` because FPGA isn't responding.

**Possible causes**:
- FPGA bitstream doesn't have UART DMA properly connected
- UART RX/TX not working
- Status byte format mismatch
- Timing issues (FPGA needs time to respond)

**Fix**: Added retries and better timeout handling in driver.

### 2. UB Read Issues

**Problem**: `read_ub()` returns 0 bytes.

**Looking at hardware** (`uart_dma_basys3.sv`):
- READ_UB state machine sends data byte-by-byte via UART TX
- Driver uses `ser.read(length)` which might timeout

**Possible causes**:
- FPGA not sending data fast enough
- Serial port timeout too short
- UART TX not working
- READ_UB state machine stuck

**Fix**: Increased timeout and added delays.

### 3. Execution Status

**Problem**: Tests check `status.sys_busy` but status is `None`.

**Fix**: Better error handling - check if status is `None` first.

## Debugging Steps

### Step 1: Verify UART Connection

```python
# Test basic UART
import serial
ser = serial.Serial('/dev/tty.usbserial-XXXX', 115200, timeout=1.0)
ser.write(b'\x06')  # READ_STATUS command
response = ser.read(1)
print(f"Status byte: {response.hex() if response else 'None'}")
```

### Step 2: Check FPGA Bitstream

Verify the bitstream includes:
- `uart_dma_basys3` module instantiated
- UART pins correctly assigned
- Clock running at 100MHz

### Step 3: Add Debugging

Enable verbose mode in driver:
```python
tpu = TPU_Coprocessor(port, verbose=True)
```

### Step 4: Check Status Byte Format

The status byte format should match `TPUStatus.from_byte()`:
- Bit 0: sys_busy
- Bit 1: vpu_busy
- Bit 2: ub_busy
- etc.

## Quick Fixes Applied

1. ✅ Added timeout parameter to `_read_response()`
2. ✅ Increased timeout for `read_ub()` (2.0s default)
3. ✅ Added retries for `read_status()`
4. ✅ Better error messages in test script
5. ✅ Check for `None` status before accessing fields

## Next Steps

1. **Test with verbose mode**: See what's actually happening
2. **Check FPGA LEDs**: See if UART DMA is receiving commands
3. **Use oscilloscope/logic analyzer**: Verify UART signals
4. **Test with simple loopback**: Send byte, expect echo

## Expected Behavior

**Working UART**:
- Status read returns 1 byte
- UB read returns requested bytes
- Commands complete without timeout

**Broken UART**:
- Status read returns 0 bytes (timeout)
- UB read returns 0 bytes
- All operations fail

## Hardware Checklist

- [ ] UART pins correctly assigned (B18/A18)
- [ ] UART baud rate matches (115200)
- [ ] Clock running (100MHz)
- [ ] Reset released (rst_n = 1)
- [ ] UART DMA module instantiated
- [ ] UART RX/TX modules working

