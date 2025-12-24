# Vivado Project Creation Script for TPU v1
# Run with: vivado -mode batch -source create_vivado_project.tcl

# Create project
create_project tpu_basys3 . -part xc7a35tcpg236-1 -force

# Add source files
add_files -norecurse rtl/tpu_top.sv
add_files -norecurse rtl/tpu_controller.sv
add_files -norecurse rtl/tpu_datapath.sv
add_files -norecurse rtl/unified_buffer.sv
add_files -norecurse rtl/systolic_controller.sv
add_files -norecurse rtl/accumulator.sv
add_files -norecurse rtl/activation_pipeline.sv
add_files -norecurse rtl/dual_weight_fifo.sv
add_files -norecurse rtl/weight_fifo.sv
add_files -norecurse rtl/normalizer.sv
add_files -norecurse rtl/loss_block.sv
add_files -norecurse rtl/clock_manager.sv
add_files -norecurse rtl/pe_dsp.sv
add_files -norecurse rtl/pe.sv
add_files -norecurse rtl/mlp_top.sv
add_files -norecurse rtl/vpu.sv
add_files -norecurse rtl/basys3_test_interface.sv
add_files -norecurse rtl/uart_rx.sv
add_files -norecurse rtl/uart_tx.sv
add_files -norecurse rtl/uart_dma_basys3.sv
add_files -norecurse xilinx_primitives.v

# Set top module
set_property top tpu_top [current_fileset]

# Add constraints
add_files -fileset constrs_1 -norecurse tpu_constraints.xdc

# Set synthesis and implementation strategies for better results
set_property strategy Flow_PerfOptimized_high [get_runs synth_1]
set_property strategy Performance_ExtraTimingOpt [get_runs impl_1]

# Enable SystemVerilog
set_property file_type SystemVerilog [get_files *.sv]

# Update compile order
update_compile_order -fileset sources_1

puts "TPU Vivado project created successfully!"
puts "Run: vivado tpu_basys3.xpr"
puts "Then: Run Synthesis → Run Implementation → Generate Bitstream → Program Device"
