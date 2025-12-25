# Build TPU directly from Verilog (skip Yosys)

set part_name "xc7a35tcpg236-1"
set top_module "tpu_top_wrapper"

puts "=========================================="
puts "TPU Build (Vivado - Direct from Verilog)"
puts "=========================================="

create_project -in_memory -part ${part_name} -force

# Read all RTL files directly (in dependency order)
# Lowest-level building blocks (no dependencies)
read_verilog -sv rtl/pe.sv
read_verilog -sv rtl/activation_func.sv
read_verilog -sv rtl/normalizer.sv
read_verilog -sv rtl/loss_block.sv
read_verilog -sv rtl/uart_rx.sv
read_verilog -sv rtl/uart_tx.sv

# Mid-level modules (depend on basic blocks)
read_verilog -sv rtl/mmu.sv
read_verilog -sv rtl/systolic_controller.sv
read_verilog -sv rtl/accumulator.sv
read_verilog -sv rtl/activation_pipeline.sv
read_verilog -sv rtl/dual_weight_fifo.sv
read_verilog -sv rtl/unified_buffer.sv
read_verilog -sv rtl/uart_dma_basys3.sv

# Top-level modules (depend on everything)
read_verilog -sv rtl/tpu_controller.sv
read_verilog -sv rtl/tpu_datapath.sv
read_verilog -sv rtl/basys3_test_interface.sv
read_verilog -sv rtl/tpu_top.sv
read_verilog -sv rtl/tpu_top_wrapper.sv

set_property top ${top_module} [current_fileset]

# Read constraints
if {![file exists constraints/basys3_vivado.xdc]} {
    puts "ERROR: constraints/basys3_vivado.xdc not found!"
    exit 1
}
read_xdc constraints/basys3_vivado.xdc

# Synthesize
puts "Running synthesis..."
synth_design -top ${top_module} -part ${part_name}

# Check utilization after synthesis
puts "=========================================="
puts "Post-Synthesis Utilization:"
puts "=========================================="
report_utilization

# Implementation
puts "Running implementation..."
opt_design
place_design
phys_opt_design
route_design

# Timing
puts "Timing analysis..."
file mkdir tpu_build
report_timing_summary -file tpu_build/tpu_timing.rpt
set wns [get_property SLACK [get_timing_paths -max_paths 1 -nworst 1]]
puts "WNS = ${wns} ns"

# Final utilization
puts "=========================================="
puts "Post-Implementation Utilization:"
puts "=========================================="
report_utilization

# Bitstream
file mkdir tpu_build
write_bitstream -force tpu_build/tpu_basys3.bit

if {[file exists tpu_build/tpu_basys3.bit]} {
    puts "=========================================="
    puts "✓ TPU BUILD SUCCESSFUL!"
    puts "=========================================="
    puts "Bitstream: tpu_build/tpu_basys3.bit"
    puts "Size: [file size tpu_build/tpu_basys3.bit] bytes"
} else {
    puts "ERROR: Bitstream generation failed!"
    exit 1
}

close_project
exit 0
