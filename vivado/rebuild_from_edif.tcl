# Vivado TCL - Rebuild using existing EDIF (no synthesis)
# This avoids the Vivado synthesis issues

cd ~/tpu_build

# Use the existing EDIF from adroit_upload
read_edif tpu_basys3.edif

# Set top module
set_property top tpu_top_wrapper [current_fileset]

# Read constraints
read_xdc basys3_nextpnr.xdc

# Link design
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper

# Verify UART pins
puts "\n========== UART PIN VERIFICATION =========="
report_io -name io_pins
puts "==========================================\n"

# Implementation
opt_design
place_design
route_design

# Check routing
report_route_status

# Generate bitstream
write_bitstream -force tpu_from_edif.bit

puts "\n========== BUILD COMPLETE =========="
puts "Bitstream: ~/tpu_build/tpu_from_edif.bit"
puts "NOTE: This uses the OLD EDIF (with bank bug)"
puts "====================================\n"
