# Vivado TCL Script - Direct RTL Synthesis (no Yosys)
# This script synthesizes the TPU directly from RTL in Vivado
# avoiding Yosys memory inference issues

# Close any existing projects to start fresh
if {[current_project -quiet] != ""} {
    close_project -quiet
}

# Set project paths
set project_dir [file normalize ~/tpu_build]
set rtl_dir [file normalize ~/tpu_build/rtl]
# Use basys3_vivado.xdc with CORRECTED pin assignments (tx→A18, rx→B18)
set constraints_file [file normalize ~/tpu_build/basys3_vivado.xdc]

# Create project directory
file mkdir $project_dir
cd $project_dir

# Clear any existing design state
if {[current_design -quiet] != ""} {
    close_design -quiet
}

# Read all RTL files (in non-project mode)
read_verilog -sv $rtl_dir/tpu_top_wrapper.sv
read_verilog -sv $rtl_dir/tpu_top.sv
read_verilog -sv $rtl_dir/tpu_controller.sv
read_verilog -sv $rtl_dir/tpu_datapath.sv
read_verilog -sv $rtl_dir/unified_buffer.sv
read_verilog -sv $rtl_dir/systolic_controller.sv
read_verilog -sv $rtl_dir/accumulator.sv
read_verilog -sv $rtl_dir/accumulator_mem.sv
read_verilog -sv $rtl_dir/activation_pipeline.sv
read_verilog -sv $rtl_dir/activation_func.sv
read_verilog -sv $rtl_dir/dual_weight_fifo.sv
read_verilog -sv $rtl_dir/weight_fifo.sv
read_verilog -sv $rtl_dir/normalizer.sv
read_verilog -sv $rtl_dir/loss_block.sv
read_verilog -sv $rtl_dir/clock_manager.sv
read_verilog -sv $rtl_dir/pe_dsp.sv
read_verilog -sv $rtl_dir/pe.sv
read_verilog -sv $rtl_dir/mlp_top.sv
read_verilog -sv $rtl_dir/mmu.sv
read_verilog -sv $rtl_dir/vpu.sv
read_verilog -sv $rtl_dir/basys3_test_interface.sv
read_verilog -sv $rtl_dir/uart_rx_improved.sv
read_verilog -sv $rtl_dir/uart_tx.sv
read_verilog -sv $rtl_dir/uart_dma_basys3.sv

# Read constraints (basys3_vivado.xdc already has clock constraint)
read_xdc $constraints_file

# Synthesize with Vivado (not Yosys)
# -flatten_hierarchy none preserves module hierarchy to prevent optimization issues
synth_design -top tpu_top_wrapper -part xc7a35tcpg236-1 -flatten_hierarchy none

# Verify UART pins are assigned CORRECTLY (tx→A18, rx→B18)
puts "\n=========================================="
puts "VERIFYING UART PIN ASSIGNMENTS:"
puts "==========================================\n"
set uart_tx_loc [get_property LOC [get_ports uart_tx]]
set uart_rx_loc [get_property LOC [get_ports uart_rx]]
puts "uart_tx LOC: $uart_tx_loc (expected site for A18)"
puts "uart_rx LOC: $uart_rx_loc (expected site for B18)"

# Get package pin names from the IOB sites
set uart_tx_site [get_sites -of_objects [get_ports uart_tx]]
set uart_rx_site [get_sites -of_objects [get_ports uart_rx]]
if {[llength $uart_tx_site] > 0 && [llength $uart_rx_site] > 0} {
    set uart_tx_pkg_pin [get_package_pins -of_objects $uart_tx_site]
    set uart_rx_pkg_pin [get_package_pins -of_objects $uart_rx_site]
    puts "uart_tx package pin: $uart_tx_pkg_pin (expected: A18)"
    puts "uart_rx package pin: $uart_rx_pkg_pin (expected: B18)"
    
    # Verify package pins match expected values
    if {$uart_tx_pkg_pin != "A18" || $uart_rx_pkg_pin != "B18"} {
        puts "WARNING: UART pins may not match expected package pins!"
        puts "Expected: uart_tx→A18, uart_rx→B18"
        puts "Got: uart_tx→$uart_tx_pkg_pin, uart_rx→$uart_rx_pkg_pin"
        puts "Continuing anyway - verify pin assignments in XDC file..."
    } else {
        puts "✓ UART pins correct (tx→A18, rx→B18)"
    }
} else {
    puts "WARNING: Could not determine package pins from sites"
    puts "Verifying pins are assigned (not empty)..."
    if {$uart_tx_loc == "" || $uart_rx_loc == ""} {
        puts "ERROR: UART pins not assigned!"
        exit 1
    }
    puts "✓ UART pins assigned (LOC: tx→$uart_tx_loc, rx→$uart_rx_loc)"
}
puts ""
report_io -name io_report

# Check utilization after synthesis
puts "\n=========================================="
puts "POST-SYNTHESIS UTILIZATION:"
puts "==========================================\n"
report_utilization -hierarchical

# Save synthesis checkpoint
write_checkpoint -force post_synth.dcp

# Run implementation
# Use conservative optimization to preserve design structure
opt_design
place_design
route_design

# Verify routing
puts "\n=========================================="
puts "ROUTING STATUS:"
puts "==========================================\n"
report_route_status

# Final utilization report
puts "\n=========================================="
puts "POST-IMPLEMENTATION UTILIZATION:"
puts "==========================================\n"
report_utilization -hierarchical

# Generate bitstream
write_bitstream -force tpu_basys3_vivado_synth.bit

# Report bitstream size
set bitstream_size [file size tpu_basys3_vivado_synth.bit]
puts "\n=========================================="
puts "BITSTREAM SIZE: $bitstream_size bytes ([expr {$bitstream_size / 1024}] KB)"
puts "==========================================\n"

puts "\n=========================================="
puts "BUILD COMPLETE!"
puts "Bitstream: $project_dir/tpu_basys3_vivado_synth.bit"
puts "==========================================\n"
