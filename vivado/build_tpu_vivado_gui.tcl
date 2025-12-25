# Build TPU in Vivado GUI (no exit commands)

set part_name "xc7a35tcpg236-1"
set top_module "tpu_top_wrapper"

puts "=========================================="
puts "TPU Build (Vivado GUI Mode)"
puts "=========================================="

# Create regular project (not in-memory for GUI)
create_project tpu_build_project ./vivado_project -part ${part_name} -force

# Read all RTL files directly (in dependency order)
# Basic building blocks
read_verilog -sv rtl/pe.sv
read_verilog -sv rtl/activation_func.sv
read_verilog -sv rtl/normalizer.sv
read_verilog -sv rtl/uart_rx.sv
read_verilog -sv rtl/uart_tx.sv

# Mid-level modules
read_verilog -sv rtl/mmu.sv
read_verilog -sv rtl/systolic_controller.sv
read_verilog -sv rtl/accumulator.sv
read_verilog -sv rtl/activation_pipeline.sv
read_verilog -sv rtl/dual_weight_fifo.sv
read_verilog -sv rtl/unified_buffer.sv
read_verilog -sv rtl/uart_dma_basys3.sv

# Top-level modules
read_verilog -sv rtl/tpu_controller.sv
read_verilog -sv rtl/tpu_datapath.sv
read_verilog -sv rtl/basys3_test_interface.sv
read_verilog -sv rtl/tpu_top.sv
read_verilog -sv rtl/tpu_top_wrapper.sv

set_property top ${top_module} [current_fileset]

# Read constraints
if {[file exists constraints/basys3_vivado.xdc]} {
    read_xdc constraints/basys3_vivado.xdc
    puts "✓ Constraints loaded"
} else {
    puts "WARNING: constraints/basys3_vivado.xdc not found!"
}

# Synthesize
puts "Running synthesis..."
synth_design -top ${top_module} -part ${part_name}

# Check utilization after synthesis
puts "=========================================="
puts "Post-Synthesis Utilization:"
puts "=========================================="
report_utilization

puts ""
puts "=========================================="
puts "Synthesis complete!"
puts "=========================================="
puts "Next steps in GUI:"
puts "  1. Check the utilization above (should be ~15K LUTs, not 270!)"
puts "  2. Run Implementation -> Run Implementation"
puts "  3. Generate Bitstream -> Generate Bitstream"
puts "  4. Find bitstream at: vivado_project/tpu_build_project.runs/impl_1/tpu_top_wrapper.bit"
puts "=========================================="

# DO NOT EXIT - let user work in GUI
