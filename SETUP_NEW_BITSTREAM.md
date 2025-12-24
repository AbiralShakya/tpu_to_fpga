# Setup and Test with New Bitstream (tpu_top_wrapper_rev2.bit)

## Current Issue

The logs show:
- ✅ Commands ARE being sent successfully
- ❌ FPGA NEVER responds (0 bytes received)
- ❌ All reads show `in_waiting: 0` (serial buffer empty)

**Root Cause**: Current bitstream (`tpu_top_wrapper.bit`) likely doesn't have UART DMA properly connected or is the wrong bitstream.

## Solution: Use New Bitstream

You have `tpu_top_wrapper_rev2.bit` which should have:
- ✅ `uart_dma_basys3_improved` properly instantiated
- ✅ Streaming mode support (command 0x07)
- ✅ Improved UART RX with oversampling

## Step 1: Verify Current Bitstream Issue

```bash
# Test basic UART connectivity
python3 python/test_uart_basic.py /dev/tty.usbserial-XXXXX
```

This will confirm if FPGA is responding at all.

## Step 2: Program New Bitstream

```bash
# Program the new bitstream with streaming mode
openFPGALoader -b basys3 build/tpu_top_wrapper_rev2.bit
```

## Step 3: Test Basic Connectivity

```bash
# Test if FPGA responds now
python3 python/test_uart_basic.py /dev/tty.usbserial-XXXXX
```

**Expected**: Should see responses from FPGA (status bytes, etc.)

## Step 4: Test Legacy Mode

```bash
# Test with legacy mode (commands 0x01-0x06)
python3 python/test_isa_legacy.py /dev/tty.usbserial-XXXXX
```

## Step 5: Test Streaming Mode (if supported)

```bash
# Test with streaming mode (command 0x07)
python3 python/test_all_instructions.py /dev/tty.usbserial-XXXXX
```

## Step 6: Test Verified Results

```bash
# Test with result verification
python3 python/test_isa_legacy_verified.py /dev/tty.usbserial-XXXXX
```

## Troubleshooting

### If Still No Response After Programming New Bitstream

1. **Check bitstream file**:
   ```bash
   ls -lh build/tpu_top_wrapper_rev2.bit
   ```
   Should be ~2-3 MB (not empty)

2. **Verify programming**:
   ```bash
   openFPGALoader -b basys3 -v build/tpu_top_wrapper_rev2.bit
   ```
   Should show "Done" message

3. **Check UART port**:
   ```bash
   ls -l /dev/tty.usbserial-*
   ```
   Should show your device

4. **Test with simple loopback** (if FPGA has loopback):
   - Some designs echo bytes back
   - Try sending any byte and see if it comes back

### If New Bitstream Works

Great! The issue was the old bitstream. Now you can:
- Test all 20 ISA instructions
- Use streaming mode for faster execution
- Verify actual results (not just completion)

## Quick Reference

```bash
# 1. Program new bitstream
openFPGALoader -b basys3 build/tpu_top_wrapper_rev2.bit

# 2. Test basic connectivity
python3 python/test_uart_basic.py /dev/tty.usbserial-XXXXX

# 3. Test ISA (legacy mode)
python3 python/test_isa_legacy.py /dev/tty.usbserial-XXXXX

# 4. Test ISA with verification
python3 python/test_isa_legacy_verified.py /dev/tty.usbserial-XXXXX

# 5. Test streaming mode (if bitstream supports it)
python3 python/test_all_instructions.py /dev/tty.usbserial-XXXXX
```

