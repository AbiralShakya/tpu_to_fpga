# Vivado TCL script to build TPU bitstream using Yosys EDIF
# Run with: vivado -mode batch -source vivado/build_tpu_vivado.tcl

# Set project variables
set project_name "tpu_basys3"
set project_dir "vivado/${project_name}"
set part_name "xc7a35tcpg236-1"
set top_module "tpu_top_wrapper"
set edif_file "build/tpu_basys3.edif"
set xdc_file "constraints/basys3_nextpnr.xdc"

# Get current working directory
set proj_dir [pwd]

# Create project directory
file mkdir ${project_dir}

# Create in-memory project (faster than on-disk project)
create_project -in_memory -part ${part_name} ${project_dir}

# Read EDIF netlist (use absolute path)
set edif_path [file normalize ${edif_file}]
puts "Reading EDIF netlist: ${edif_path}"
read_edif ${edif_path}

# Set top module
puts "Setting top module: ${top_module}"
set_property top ${top_module} [current_fileset]

# Read constraints (use absolute path)
set xdc_path [file normalize ${xdc_file}]
puts "Reading constraints: ${xdc_path}"
read_xdc ${xdc_path}

# Link design
puts "Linking design..."
link_design -part ${part_name} -top ${top_module}

# Run implementation
puts "Running implementation..."

# Optimization
puts "  Running opt_design..."
opt_design

# Placement
puts "  Running place_design..."
place_design

# Routing
puts "  Running route_design..."
route_design

# Timing analysis
puts "  Running timing analysis..."
report_timing_summary -file ${project_dir}/timing_summary.rpt
report_timing -file ${project_dir}/timing.rpt

# Utilization report
puts "  Generating utilization report..."
report_utilization -file ${project_dir}/utilization.rpt

# Generate bitstream
puts "  Generating bitstream..."
set bitstream_path "${proj_dir}/build/${top_module}.bit"
write_bitstream -force ${bitstream_path}

puts ""
puts "=========================================="
puts "✓ BUILD COMPLETE!"
puts "=========================================="
puts "Bitstream: ${bitstream_path}"
puts ""
puts "To program FPGA:"
puts "  openFPGALoader -b basys3 ${bitstream_path}"
puts "=========================================="

# Close project
close_project

