# UART Communication Issue - Root Cause & Solution

## Problem Summary

UART communication tests show "No response" when using `simple_test.bit`. Commands are sent but the FPGA doesn't respond.

## Root Cause (100% Confirmed)

The `simple_test_top.sv` design **does NOT implement UART communication**.

**Evidence:**
- Line 254: `assign uart_tx = 1'b1;` - TX is hardcoded to idle (always high)
- No UART RX module instantiated - `uart_rx` input is declared but unused
- No command processing logic - no state machine to handle UART commands

**What `simple_test.bit` DOES support:**
- ✅ Physical I/O (switches, buttons, LEDs, 7-segment display)
- ✅ State machine for testing physical interface
- ❌ **UART communication - NOT IMPLEMENTED**

## Solution Options

### Option 1: Build UART Test Bitstream (Simple Loopback)

This design implements UART RX/TX and echoes back any byte you send.

```bash
cd tpu_to_fpga

# Build with Vivado
vivado -mode batch -source build_uart_test.tcl

# Program FPGA
openFPGALoader -b basys3 build/uart_test/uart_test_project.runs/impl_1/uart_test_simple.bit

# Test UART
python3 ../test_uart_simple.py /dev/tty.usbserial-XXXX
```

**Expected behavior:**
- Any byte you send will be echoed back
- LEDs[7:0] show received byte
- LEDs[15:8] show transmitted byte

### Option 2: Use Full TPU Bitstream (Recommended for TPU Testing)

The full TPU design has complete UART DMA support with command protocol.

```bash
cd tpu_to_fpga

# Build full TPU bitstream
./build_tpu_bitstream.sh

# Program FPGA
openFPGALoader -b basys3 build/tpu_basys3.bit

# Test UART with TPU driver
python3 test_tpu_instructions.py /dev/tty.usbserial-XXXX
```

**Expected behavior:**
- Responds to all UART commands (status, write UB, write weights, instructions, etc.)
- Full TPU instruction set support
- Streaming mode for continuous instruction execution

## Quick Test After Building UART Design

Once you have a UART-capable bitstream loaded:

```bash
# Find UART device
UART_PORT=$(ls /dev/tty.usbserial* | tail -1)

# Test basic UART
python3 test_uart_simple.py $UART_PORT

# For UART test design, you should see:
# ✓ Received status byte: 0xXX (echo of what you sent)
# ✓ NOP instruction sent and echoed back
```

## Files Reference

| File | Purpose |
|------|---------|
| `simple_test.bit` | ❌ No UART support - physical I/O only |
| `uart_test_simple.sv` | ✅ UART loopback design |
| `tpu_top.sv` | ✅ Full TPU with UART DMA |
| `build_uart_test.tcl` | Build script for UART test |
| `build_tpu_bitstream.sh` | Build script for full TPU |

## Summary

**The issue:** `simple_test.bit` doesn't implement UART - it's a physical I/O test design only.

**The fix:** Build and use either:
1. `uart_test_simple.bit` - Simple UART loopback (good for testing UART hardware)
2. `tpu_basys3.bit` - Full TPU with complete UART DMA (for TPU instruction testing)

Both designs properly implement UART RX/TX and will respond to your test scripts.

