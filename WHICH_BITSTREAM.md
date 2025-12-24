# Which Bitstream Do You Have?

## ❌ `simple_test.bit` - NOT for ISA Testing

**What it is**: A simple test design that only tests physical I/O
- ✅ Switches, buttons, LEDs, 7-segment display work
- ✅ UART pins are connected
- ❌ **NO TPU functionality**
- ❌ **NO ISA instructions**
- ❌ **Won't respond to TPU commands**

**File**: `simple_test.bit` (2.2 MB)

**Use case**: Testing that FPGA programming works, basic I/O works

## ✅ `tpu_top_wrapper.bit` - FOR ISA Testing

**What it is**: The full TPU with all ISA instructions
- ✅ Complete TPU datapath
- ✅ All 20 ISA instructions
- ✅ UART DMA interface
- ✅ Full systolic array
- ✅ Ready for ISA testing

**File**: `build/tpu_top_wrapper.bit` (needs to be generated)

**Use case**: Testing your ISA, running TPU instructions

## What You Need to Do

### Option 1: Generate Full TPU Bitstream (Recommended)

**On Adroit** (Vivado TCL Console):
```tcl
cd /home/as0714/tpu_build/adroit_upload
read_edif tpu_basys3.edif
set_property top tpu_top_wrapper [current_fileset]
read_xdc basys3_nextpnr.xdc
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper
opt_design
place_design
route_design
write_bitstream -force tpu_top_wrapper.bit
```

**Then download and program**:
```bash
# Download
scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper.bit build/

# Program
openFPGALoader -b basys3 build/tpu_top_wrapper.bit

# Test ISA
python3 python/test_all_instructions.py /dev/tty.usbserial-XXXXX
```

### Option 2: Test with `simple_test.bit` First (Quick Check)

Just to verify FPGA programming works:
```bash
# Program
openFPGALoader -b basys3 simple_test.bit

# Test basic UART (won't respond to TPU commands)
python3 test_uart_simple.py /dev/tty.usbserial-XXXXX
```

**Note**: This will only test UART connectivity, not TPU functionality.

## Summary

- **`simple_test.bit`** = Basic I/O test (not for ISA)
- **`tpu_top_wrapper.bit`** = Full TPU (for ISA testing) ← **You need this!**

**Next step**: Generate `tpu_top_wrapper.bit` on Adroit using the commands above.

