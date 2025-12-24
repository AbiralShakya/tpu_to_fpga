# Fastest Path to Testing ISA on FPGA

## Step 1: Generate Bitstream on Adroit (5 minutes)

**In Vivado TCL Console** (Window → Tcl Console), copy-paste this:

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

**Wait 15-30 minutes** for it to complete. The bitstream will be at:
`/home/as0714/tpu_build/adroit_upload/tpu_top_wrapper.bit`

## Step 2: Download Bitstream to Mac (30 seconds)

```bash
# From your Mac terminal
scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper.bit build/
```

## Step 3: Program FPGA (10 seconds)

```bash
# On your Mac
cd /Users/abiralshakya/Documents/tpu_to_fpga
openFPGALoader -b basys3 build/tpu_top_wrapper.bit
```

## Step 4: Test ISA (Immediately!)

```bash
# Find your UART port
ls /dev/tty.usbserial-*

# Run ISA tests
python3 python/test_all_instructions.py /dev/tty.usbserial-XXXXX

# Or use the driver
python3 python/drivers/tpu_driver.py /dev/tty.usbserial-XXXXX
```

## That's It!

**Total time**: ~20-35 minutes (mostly waiting for Vivado)

**What you're testing**:
- All 20 ISA instructions
- UART communication
- TPU functionality on real hardware

---

## If Vivado is Still Running

If you already started the build, just wait for it to finish, then:
1. Download bitstream (Step 2)
2. Program FPGA (Step 3)  
3. Test ISA (Step 4)

