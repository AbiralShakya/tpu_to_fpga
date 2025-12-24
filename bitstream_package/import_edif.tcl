# Vivado TCL script to import Yosys EDIF and generate bitstream
# Create minimal project to handle EDIF import properly

# Create project with EDIF as source
create_project tpu_import -part xc7a35tcpg236-1 -force

# Add EDIF file as design source
add_files tpu.edif

# Set top module
set_property top tpu_top [current_fileset]

# Add constraints
add_files -fileset constrs_1 tpu_constraints.xdc

# Import and link the design
link_design

# Run implementation directly (skip synthesis since EDIF is pre-synthesized)
set_property DESIGN_MODE GateLvl [current_fileset]

# Run implementation steps
opt_design
place_design
route_design

# Generate bitstream
write_bitstream -force tpu_top.bit

puts "✓ Bitstream generated: tpu_top.bit"
puts "✓ Transfer back to Mac and program with: openFPGALoader -b basys3 tpu_top.bit"

close_project
