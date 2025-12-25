# =============================================================================
# Vivado TCL script to build TPU bitstream on Adroit
# =============================================================================
# Run with: vivado -mode batch -source build_on_adroit.tcl
# Make sure tpu_basys3.edif and basys3_vivado.xdc are in current directory
# =============================================================================

set part_name "xc7a35tcpg236-1"
set top_module "tpu_top_wrapper"

puts ""
puts "=========================================="
puts "TPU Bitstream Build for Basys3"
puts "=========================================="
puts "Part: ${part_name}"
puts "Top: ${top_module}"
puts ""

# Create in-memory project (faster)
puts "Creating project..."
create_project -in_memory -part ${part_name} -force

# Read EDIF netlist
puts "Reading EDIF netlist..."
if {![file exists tpu_basys3.edif]} {
    puts "ERROR: tpu_basys3.edif not found in current directory!"
    exit 1
}
read_edif tpu_basys3.edif

# Set top module
puts "Setting top module: ${top_module}"
set_property top ${top_module} [current_fileset]

# Read constraints with clock definitions
puts "Reading constraints..."
if {![file exists basys3_vivado.xdc]} {
    puts "ERROR: basys3_vivado.xdc not found in current directory!"
    exit 1
}
read_xdc basys3_vivado.xdc

# Link design
puts "Linking design..."
link_design -part ${part_name} -top ${top_module}

# Report design after link
puts ""
puts "Design linked successfully. Hierarchy:"
report_design_analysis -complexity

# Implementation
puts ""
puts "=========================================="
puts "Running Implementation"
puts "=========================================="

# Optimization
puts "  [1/4] Running opt_design..."
opt_design
puts "  ✓ Optimization complete"

# Placement
puts "  [2/4] Running place_design..."
place_design
puts "  ✓ Placement complete"

# Post-placement optimization
puts "  [3/4] Running phys_opt_design..."
phys_opt_design
puts "  ✓ Physical optimization complete"

# Routing
puts "  [4/4] Running route_design..."
route_design
puts "  ✓ Routing complete"

# Check timing
puts ""
puts "=========================================="
puts "Timing Analysis"
puts "=========================================="
report_timing_summary -file timing_summary.rpt
report_timing -file timing.rpt -max_paths 10

# Check for timing violations
set wns [get_property SLACK [get_timing_paths -max_paths 1 -nworst 1]]
if {$wns < 0} {
    puts "WARNING: Design has timing violations (WNS = ${wns} ns)"
    puts "         Check timing_summary.rpt for details"
} else {
    puts "✓ Timing constraints met (WNS = ${wns} ns)"
}

# Utilization
puts ""
puts "=========================================="
puts "Resource Utilization"
puts "=========================================="
report_utilization -file utilization.rpt
report_utilization

# Generate bitstream
puts ""
puts "=========================================="
puts "Generating Bitstream"
puts "=========================================="
write_bitstream -force tpu_top_wrapper_rev6_fixed_clock.bit

# Verify bitstream was created
if {[file exists tpu_top_wrapper_rev6_fixed_clock.bit]} {
    set bitsize [file size tpu_top_wrapper_rev6_fixed_clock.bit]
    puts ""
    puts "=========================================="
    puts "✓ BUILD SUCCESSFUL!"
    puts "=========================================="
    puts "Bitstream: tpu_top_wrapper_rev6_fixed_clock.bit"
    puts "Size: [expr {$bitsize / 1024}] KB"
    puts ""
    puts "To download and program:"
    puts "  scp as0714@adroit.princeton.edu:~/tpu_build/adroit_upload/tpu_top_wrapper_rev6_fixed_clock.bit build/"
    puts "  openFPGALoader -b basys3 build/tpu_top_wrapper_rev6_fixed_clock.bit"
    puts "=========================================="
} else {
    puts ""
    puts "=========================================="
    puts "✗ ERROR: Bitstream generation failed!"
    puts "=========================================="
    exit 1
}

# Close project
close_project

exit 0
