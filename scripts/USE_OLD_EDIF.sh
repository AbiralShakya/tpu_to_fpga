#!/bin/bash
# Use the old working EDIF (has bank bug but UART works)

echo "Using old EDIF to generate bitstream on Adroit..."

scp adroit_upload/tpu_basys3.edif as0714@adroit.princeton.edu:~/tpu_build/
scp constraints/basys3_nextpnr.xdc as0714@adroit.princeton.edu:~/tpu_build/
scp vivado/rebuild_from_edif.tcl as0714@adroit.princeton.edu:~/tpu_build/

ssh as0714@adroit.princeton.edu 'module load vivado/2023.1 && cd ~/tpu_build && vivado -mode batch -source rebuild_from_edif.tcl'

scp as0714@adroit.princeton.edu:~/tpu_build/tpu_from_edif.bit build/

echo ""
echo "Bitstream ready: build/tpu_from_edif.bit"
echo "This bitstream has the bank bug (reads return zeros) but UART works"
echo ""
echo "To test: openFPGALoader -b basys3 build/tpu_from_edif.bit"
