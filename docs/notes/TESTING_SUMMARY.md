# FPGA Testing Summary

## What You Have

✅ **Bitstream file:** `simple_test.bit` (2.2 MB)
✅ **Constraints file:** `simple_test.xdc`
✅ **Testing scripts:** Created and ready to use

## Quick Commands

### 1. Program FPGA
```bash
# Your bitstream is at the root
openFPGALoader -b basys3 simple_test.bit
```

### 2. Find UART Device
```bash
# On Mac, usually the last USB serial device is UART
UART_PORT=$(ls /dev/tty.usbserial* | tail -1)
echo "UART Port: $UART_PORT"
```

### 3. Run Automated Test
```bash
./test_fpga_uart.sh
```

### 4. Manual Testing
```bash
# Basic UART test
python3 test_uart_simple.py $UART_PORT

# TPU instructions test (if full TPU bitstream)
python3 test_tpu_instructions.py $UART_PORT
```

## Testing Scripts Created

1. **`test_fpga_uart.sh`** - Comprehensive automated test
   - Checks for bitstream
   - Programs FPGA (if openFPGALoader available)
   - Finds UART device
   - Tests UART communication
   - Tests TPU driver

2. **`test_uart_simple.py`** - Basic UART test
   - Tests UART RX/TX
   - Sends status queries
   - Tests instruction writes

3. **`test_tpu_instructions.py`** - TPU instruction test
   - Tests basic instructions (NOP, CFG_REG, SYNC)
   - Tests memory operations
   - Tests compute operations (MATMUL)

## Important Notes

### About `simple_test.bit`

The `simple_test_top.sv` design is a **simple test interface** that:
- ✅ Tests physical I/O (switches, buttons, LEDs, 7-seg)
- ✅ Has UART pins connected
- ⚠️ **Does NOT have full TPU functionality**
- ⚠️ **May not respond to all UART commands**

For full TPU testing, you need the full TPU bitstream:
- Location: `tpu_to_fpga/build/tpu_basys3.bit`
- Build with: `cd tpu_to_fpga && ./build_tpu_bitstream.sh`

### UART Device on Mac

Basys3 creates **two** USB serial devices:
1. **First device:** JTAG (for programming) - `/dev/tty.usbserial-XXXX1`
2. **Second device:** UART (for communication) - `/dev/tty.usbserial-XXXX2` ← **Use this!**

The script automatically finds the UART device (usually the last one).

### Testing Flow

```
1. Program FPGA (one-time)
   ↓
2. Find UART device
   ↓
3. Test UART communication
   ↓
4. Test TPU instructions (if full TPU bitstream)
```

## Expected Results

### With `simple_test.bit`:

- ✅ UART connection should work
- ✅ Status query may or may not respond (depends on design)
- ⚠️ Full TPU instructions won't work (design doesn't have TPU)

### With Full TPU Bitstream:

- ✅ All UART commands work
- ✅ Status register responds
- ✅ All TPU instructions work
- ✅ Matrix operations work

## Next Steps

1. **Test current bitstream:**
   ```bash
   ./test_fpga_uart.sh
   ```

2. **If you want full TPU functionality:**
   ```bash
   cd tpu_to_fpga
   ./build_tpu_bitstream.sh  # Build full TPU
   openFPGALoader -b basys3 build/tpu_basys3.bit  # Program
   python3 test_tpu_instructions.py $UART_PORT  # Test
   ```

3. **For detailed guides:**
   - `QUICK_TEST_GUIDE.md` - Quick reference
   - `UART_SETUP_GUIDE.md` - Detailed UART setup
   - `FPGA_PROGRAMMING_GUIDE.md` - Programming details

## Troubleshooting

### "No UART device found"
- Check USB connection
- Power on Basys3
- Check: `ls /dev/tty.usbserial*`

### "Permission denied"
```bash
sudo chmod 666 /dev/tty.usbserial-XXXX
# Or add to uucp group:
sudo dseditgroup -o edit -a $USER -t user uucp
```

### "No response from FPGA"
- Re-program bitstream
- Check power LED
- Verify correct UART device (second one)

### "openFPGALoader not found"
```bash
brew install openfpgaloader
```

## Files Reference

| File | Purpose |
|------|---------|
| `simple_test.bit` | Your current bitstream |
| `simple_test.xdc` | Constraints file |
| `test_fpga_uart.sh` | Automated test script |
| `test_uart_simple.py` | Basic UART test |
| `test_tpu_instructions.py` | TPU instruction test |
| `tpu_to_fpga/tpu_driver.py` | Python TPU driver |
| `QUICK_TEST_GUIDE.md` | Quick reference |
| `TESTING_SUMMARY.md` | This file |

## Ready to Test!

Run this command to start:
```bash
./test_fpga_uart.sh
```

The script will guide you through each step!

