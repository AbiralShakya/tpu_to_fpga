# Test Your ISA on FPGA - Step by Step

## ✅ You Have: `tpu_top_wrapper.bit` on Adroit

Now let's get it running on your FPGA!

## Step 1: Download Bitstream to Mac

**From your Mac terminal**:

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga

# Download from Adroit
scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper.bit build/

# Verify it downloaded
ls -lh build/tpu_top_wrapper.bit
# Should show ~2-5 MB file
```

## Step 2: Connect Basys3 FPGA

1. **Connect USB cable** to Basys3
2. **Power on** the board (switch on back)
3. **Verify connection**:
   ```bash
   # Check if FPGA is detected
   openFPGALoader -b basys3 -c
   ```

## Step 3: Program FPGA

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga

# Program the bitstream
openFPGALoader -b basys3 build/tpu_top_wrapper.bit
```

**Expected output**: 
```
Jtag frequency : requested 6.00MHz   -> real 6.00MHz
Open file build/tpu_top_wrapper.bit: OK
Parse file build/tpu_top_wrapper.bit: OK
...
Done
```

## Step 4: Find UART Port

```bash
# List all USB serial devices
ls /dev/tty.usbserial-*

# Usually the LAST one is UART (first is JTAG)
# Example output:
# /dev/tty.usbserial-210183A27BE01  (JTAG)
# /dev/tty.usbserial-210183A27BE02  (UART) ← Use this one!
```

**Note**: Basys3 creates TWO devices - use the **second/last one** for UART.

## Step 5: Test ISA Instructions

### Option A: Run All ISA Tests (Recommended)

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga

# Replace XXXXX with your actual port number
python3 python/test_all_instructions.py /dev/tty.usbserial-XXXXX
```

This will test all 20 ISA instructions and show pass/fail for each.

### Option B: Use TPU Driver (Interactive)

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga

# Interactive driver
python3 python/drivers/tpu_driver.py /dev/tty.usbserial-XXXXX
```

Then you can send individual instructions manually.

## Step 6: Verify It's Working

**What to expect**:
- ✅ UART connection established
- ✅ Status queries return valid responses
- ✅ Instructions execute successfully
- ✅ Results match expected values

**If you see errors**:
- Check UART port (use the second/last device)
- Verify baud rate is 115200
- Make sure FPGA is programmed (LEDs should show activity)

## Quick One-Liner (After Download)

```bash
# Find UART port and test
UART_PORT=$(ls /dev/tty.usbserial-* | tail -1)
echo "Using UART: $UART_PORT"
python3 python/test_all_instructions.py $UART_PORT
```

## Troubleshooting

### "Device not found"
- Check USB cable is connected
- Try different USB port
- Check `ls /dev/tty.usbserial-*` shows devices

### "Permission denied"
```bash
# Add yourself to dialout group (if needed)
sudo usermod -a -G dialout $USER
# Then logout/login
```

### "No response from FPGA"
- Verify bitstream programmed successfully
- Check UART port (use second device)
- Try resetting FPGA (power cycle)

### "Serial port error"
- Close other programs using the port
- Check baud rate is 115200
- Verify cable is data-capable (not charge-only)

## Success Indicators

✅ **FPGA programmed**: `openFPGALoader` shows "Done"
✅ **UART connected**: Test script connects without errors
✅ **Instructions work**: Tests show "PASS" for instructions
✅ **Results correct**: Computations match expected values

## Next Steps After Testing

Once ISA is working:
1. Test your specific workloads
2. Measure performance
3. Debug any failing instructions
4. Optimize if needed

---

**You're ready to test!** Start with Step 1 (download bitstream).

