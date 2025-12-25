# Vivado TCL script to build TPU bitstream using Yosys EDIF
# Run with: vivado -mode batch -source vivado/build_tpu_vivado.tcl

# Set project variables
set project_name "tpu_basys3"
set project_dir "vivado/${project_name}"
set part_name "xc7a35tcpg236-1"
set top_module "tpu_top_wrapper"
set edif_file "build/tpu_basys3.edif"
set xdc_file "constraints/basys3_vivado.xdc"

# Get current working directory
set proj_dir [pwd]

# Create project directory
file mkdir ${project_dir}

# Create in-memory project (faster than on-disk project)
# Use -force to overwrite if exists
create_project -in_memory -part ${part_name} ${project_dir} -force

# Read EDIF netlist (use absolute path)
# This imports the pre-synthesized netlist - NO SYNTHESIS NEEDED
set edif_path [file normalize ${edif_file}]
puts "Reading EDIF netlist: ${edif_path}"
if {![file exists ${edif_path}]} {
    puts "ERROR: EDIF file not found: ${edif_path}"
    exit 1
}
read_edif ${edif_path}

# Set top module
puts "Setting top module: ${top_module}"
set_property top ${top_module} [current_fileset]

# Read constraints (use absolute path)
set xdc_path [file normalize ${xdc_file}]
puts "Reading constraints: ${xdc_path}"
if {![file exists ${xdc_path}]} {
    puts "ERROR: Constraints file not found: ${xdc_path}"
    exit 1
}
read_xdc ${xdc_path}

# Link design (this validates the EDIF and constraints)
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

