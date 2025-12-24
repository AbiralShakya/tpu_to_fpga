# Vivado TCL script to import EDIF and generate bitstream
# Run from Vivado TCL Console or batch mode
# Usage: source vivado_edif_import.tcl

# Get current directory
set script_dir [file dirname [file normalize [info script]]]
cd $script_dir

puts "=========================================="
puts "TPU Bitstream Build - EDIF Import"
puts "=========================================="

# Set variables
set part_name "xc7a35tcpg236-1"
set top_module "tpu_top_wrapper"
set edif_file "tpu_top_wrapper.edif"
set xdc_file "basys3_nextpnr.xdc"

# Check files exist
if {![file exists $edif_file]} {
    puts "ERROR: EDIF file not found: $edif_file"
    exit 1
}
if {![file exists $xdc_file]} {
    puts "ERROR: Constraints file not found: $xdc_file"
    exit 1
}

puts "Reading EDIF netlist: $edif_file"
read_edif $edif_file

puts "Setting top module: $top_module"
set_property top $top_module [current_fileset]

puts "Reading constraints: $xdc_file"
read_xdc $xdc_file

puts "Linking design..."
link_design -part $part_name -top $top_module

puts ""
puts "=========================================="
puts "Running Implementation..."
puts "=========================================="

puts "  Running opt_design..."
opt_design

puts "  Running place_design..."
place_design

puts "  Running route_design..."
route_design

puts ""
puts "=========================================="
puts "Generating Bitstream..."
puts "=========================================="
write_bitstream -force ${top_module}.bit

puts ""
puts "=========================================="
puts "✓ BUILD COMPLETE!"
puts "=========================================="
puts "Bitstream: ${top_module}.bit"
puts ""
puts "To download to Mac:"
puts "  scp ${top_module}.bit YOUR_NETID@adroit.princeton.edu:~/tpu_build/adroit_upload/"
puts "  (from Mac: scp YOUR_NETID@adroit.princeton.edu:~/tpu_build/adroit_upload/${top_module}.bit build/)"
puts "=========================================="

