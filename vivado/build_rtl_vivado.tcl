# Vivado TCL - Full RTL Synthesis on Adroit
# This synthesizes directly from RTL (no Yosys EDIF)

cd ~/tpu_build

# Read all RTL files
read_verilog -sv rtl/tpu_top_wrapper.sv
read_verilog -sv rtl/tpu_top.sv
read_verilog -sv rtl/tpu_controller.sv
read_verilog -sv rtl/tpu_datapath.sv
read_verilog -sv rtl/unified_buffer.sv
read_verilog -sv rtl/systolic_controller.sv
read_verilog -sv rtl/accumulator.sv
read_verilog -sv rtl/accumulator_mem.sv
read_verilog -sv rtl/activation_pipeline.sv
read_verilog -sv rtl/activation_func.sv
read_verilog -sv rtl/dual_weight_fifo.sv
read_verilog -sv rtl/weight_fifo.sv
read_verilog -sv rtl/normalizer.sv
read_verilog -sv rtl/loss_block.sv
read_verilog -sv rtl/clock_manager.sv
read_verilog -sv rtl/pe_dsp.sv
read_verilog -sv rtl/pe.sv
read_verilog -sv rtl/mlp_top.sv
read_verilog -sv rtl/mmu.sv
read_verilog -sv rtl/vpu.sv
read_verilog -sv rtl/basys3_test_interface.sv
read_verilog -sv rtl/uart_rx.sv
read_verilog -sv rtl/uart_rx_improved.sv
read_verilog -sv rtl/uart_tx.sv
read_verilog -sv rtl/uart_dma_basys3.sv

# Read constraints
read_xdc basys3_nextpnr.xdc

# Synthesize
synth_design -top tpu_top_wrapper -part xc7a35tcpg236-1

# VERIFY UART pins before continuing
puts "\n========== VERIFYING UART PIN ASSIGNMENTS =========="
set uart_tx_pin [get_property LOC [get_ports uart_tx]]
set uart_rx_pin [get_property LOC [get_ports uart_rx]]
puts "uart_tx assigned to: $uart_tx_pin (expected: A18)"
puts "uart_rx assigned to: $uart_rx_pin (expected: B18)"

if {$uart_tx_pin != "A18" || $uart_rx_pin != "B18"} {
    puts "ERROR: UART pins not assigned correctly!"
    exit 1
}
puts "✓ UART pins correct\n"

# Implementation
opt_design
place_design
route_design

# Check routing
report_route_status

# Generate bitstream
write_bitstream -force tpu_basys3_fixed.bit

puts "\n========== BUILD COMPLETE =========="
puts "Bitstream: ~/tpu_build/tpu_basys3_fixed.bit"
puts "====================================\n"
