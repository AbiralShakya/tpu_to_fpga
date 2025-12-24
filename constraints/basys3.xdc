# =============================================================================
# TPU v1 Constraints for Artix-7 FPGA (Basys3 and Nexys A7)
# =============================================================================

# Clock Constraints
# =============================================================================

# Primary clock input (100MHz crystal on Basys3/Nexys A7)
set_property -dict {PACKAGE_PIN W5 IOSTANDARD LVCMOS33} [get_ports clk_in]
create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports clk_in]

# Generated system clock (100MHz from MMCM)
create_generated_clock -name clk_out -source [get_pins clock_manager_inst/mmcm_inst/CLKIN1] -multiply_by 10 -divide_by 10 [get_pins clock_manager_inst/mmcm_inst/CLKOUT0]

# Clock Groups
# =============================================================================

# Set asynchronous clock groups for different clock domains
set_clock_groups -asynchronous -group [get_clocks sys_clk_pin] -group [get_clocks clk_out]

# I/O Constraints
# =============================================================================

# Clock input (100MHz crystal)
set_property -dict {PACKAGE_PIN W5 IOSTANDARD LVCMOS33} [get_ports clk]
create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports clk]

# Reset input (active low)
set_property -dict {PACKAGE_PIN R2 IOSTANDARD LVCMOS33} [get_ports rst_n]

# UART Interface (USB-UART on Basys3)
set_property -dict {PACKAGE_PIN B18 IOSTANDARD LVCMOS33} [get_ports uart_tx]
set_property -dict {PACKAGE_PIN A18 IOSTANDARD LVCMOS33} [get_ports uart_rx]

# Basys 3 Physical Interface
# Switches (SW[15:0])
set_property -dict {PACKAGE_PIN V17 IOSTANDARD LVCMOS33} [get_ports {sw[0]}]
set_property -dict {PACKAGE_PIN V16 IOSTANDARD LVCMOS33} [get_ports {sw[1]}]
set_property -dict {PACKAGE_PIN W16 IOSTANDARD LVCMOS33} [get_ports {sw[2]}]
set_property -dict {PACKAGE_PIN W17 IOSTANDARD LVCMOS33} [get_ports {sw[3]}]
set_property -dict {PACKAGE_PIN W15 IOSTANDARD LVCMOS33} [get_ports {sw[4]}]
set_property -dict {PACKAGE_PIN V15 IOSTANDARD LVCMOS33} [get_ports {sw[5]}]
set_property -dict {PACKAGE_PIN W14 IOSTANDARD LVCMOS33} [get_ports {sw[6]}]
set_property -dict {PACKAGE_PIN W13 IOSTANDARD LVCMOS33} [get_ports {sw[7]}]
set_property -dict {PACKAGE_PIN V2 IOSTANDARD LVCMOS33} [get_ports {sw[8]}]
set_property -dict {PACKAGE_PIN T3 IOSTANDARD LVCMOS33} [get_ports {sw[9]}]
set_property -dict {PACKAGE_PIN T2 IOSTANDARD LVCMOS33} [get_ports {sw[10]}]
set_property -dict {PACKAGE_PIN R3 IOSTANDARD LVCMOS33} [get_ports {sw[11]}]
set_property -dict {PACKAGE_PIN W2 IOSTANDARD LVCMOS33} [get_ports {sw[12]}]
set_property -dict {PACKAGE_PIN U1 IOSTANDARD LVCMOS33} [get_ports {sw[13]}]
set_property -dict {PACKAGE_PIN T1 IOSTANDARD LVCMOS33} [get_ports {sw[14]}]
set_property -dict {PACKAGE_PIN R2 IOSTANDARD LVCMOS33} [get_ports {sw[15]}]

# Push Buttons (btn[4:0] = BTNU, BTND, BTNL, BTNR, BTNC)
set_property -dict {PACKAGE_PIN T18 IOSTANDARD LVCMOS33} [get_ports {btn[4]}]  # BTNU
set_property -dict {PACKAGE_PIN U17 IOSTANDARD LVCMOS33} [get_ports {btn[3]}]  # BTND
set_property -dict {PACKAGE_PIN W19 IOSTANDARD LVCMOS33} [get_ports {btn[2]}]  # BTNL
set_property -dict {PACKAGE_PIN T17 IOSTANDARD LVCMOS33} [get_ports {btn[1]}]  # BTNR
set_property -dict {PACKAGE_PIN U18 IOSTANDARD LVCMOS33} [get_ports {btn[0]}]  # BTNC

# 7-Segment Display
set_property -dict {PACKAGE_PIN W7 IOSTANDARD LVCMOS33} [get_ports {seg[0]}]   # CA
set_property -dict {PACKAGE_PIN W6 IOSTANDARD LVCMOS33} [get_ports {seg[1]}]   # CB
set_property -dict {PACKAGE_PIN U8 IOSTANDARD LVCMOS33} [get_ports {seg[2]}]   # CC
set_property -dict {PACKAGE_PIN V8 IOSTANDARD LVCMOS33} [get_ports {seg[3]}]   # CD
set_property -dict {PACKAGE_PIN U5 IOSTANDARD LVCMOS33} [get_ports {seg[4]}]   # CE
set_property -dict {PACKAGE_PIN V5 IOSTANDARD LVCMOS33} [get_ports {seg[5]}]   # CF
set_property -dict {PACKAGE_PIN U7 IOSTANDARD LVCMOS33} [get_ports {seg[6]}]   # CG

# 7-Segment Anodes
set_property -dict {PACKAGE_PIN U2 IOSTANDARD LVCMOS33} [get_ports {an[0]}]    # AN0
set_property -dict {PACKAGE_PIN U4 IOSTANDARD LVCMOS33} [get_ports {an[1]}]    # AN1
set_property -dict {PACKAGE_PIN V4 IOSTANDARD LVCMOS33} [get_ports {an[2]}]    # AN2
set_property -dict {PACKAGE_PIN W4 IOSTANDARD LVCMOS33} [get_ports {an[3]}]    # AN3

# Decimal Point
set_property -dict {PACKAGE_PIN V7 IOSTANDARD LVCMOS33} [get_ports dp]

# LEDs (LED[15:0])
set_property -dict {PACKAGE_PIN U16 IOSTANDARD LVCMOS33} [get_ports {led[0]}]
set_property -dict {PACKAGE_PIN E19 IOSTANDARD LVCMOS33} [get_ports {led[1]}]
set_property -dict {PACKAGE_PIN U19 IOSTANDARD LVCMOS33} [get_ports {led[2]}]
set_property -dict {PACKAGE_PIN V19 IOSTANDARD LVCMOS33} [get_ports {led[3]}]
set_property -dict {PACKAGE_PIN W18 IOSTANDARD LVCMOS33} [get_ports {led[4]}]
set_property -dict {PACKAGE_PIN U15 IOSTANDARD LVCMOS33} [get_ports {led[5]}]
set_property -dict {PACKAGE_PIN U14 IOSTANDARD LVCMOS33} [get_ports {led[6]}]
set_property -dict {PACKAGE_PIN V14 IOSTANDARD LVCMOS33} [get_ports {led[7]}]
set_property -dict {PACKAGE_PIN V13 IOSTANDARD LVCMOS33} [get_ports {led[8]}]
set_property -dict {PACKAGE_PIN V3 IOSTANDARD LVCMOS33} [get_ports {led[9]}]
set_property -dict {PACKAGE_PIN W3 IOSTANDARD LVCMOS33} [get_ports {led[10]}]
set_property -dict {PACKAGE_PIN U3 IOSTANDARD LVCMOS33} [get_ports {led[11]}]
set_property -dict {PACKAGE_PIN P3 IOSTANDARD LVCMOS33} [get_ports {led[12]}]
set_property -dict {PACKAGE_PIN N3 IOSTANDARD LVCMOS33} [get_ports {led[13]}]
set_property -dict {PACKAGE_PIN P1 IOSTANDARD LVCMOS33} [get_ports {led[14]}]
set_property -dict {PACKAGE_PIN L1 IOSTANDARD LVCMOS33} [get_ports {led[15]}]

# UART Debug LEDs (additional status)
set_property -dict {PACKAGE_PIN U15 IOSTANDARD LVCMOS33} [get_ports uart_debug_state[0]]
set_property -dict {PACKAGE_PIN U14 IOSTANDARD LVCMOS33} [get_ports uart_debug_state[1]]
set_property -dict {PACKAGE_PIN V14 IOSTANDARD LVCMOS33} [get_ports uart_debug_state[2]]
set_property -dict {PACKAGE_PIN V13 IOSTANDARD LVCMOS33} [get_ports uart_debug_cmd[0]]

# DMA Interface (using PMOD headers on Basys3)
# PMOD JA (Header J1)
set_property -dict {PACKAGE_PIN J1 IOSTANDARD LVCMOS33} [get_ports dma_start_in]
set_property -dict {PACKAGE_PIN L2 IOSTANDARD LVCMOS33} [get_ports dma_dir_in]
set_property -dict {PACKAGE_PIN J2 IOSTANDARD LVCMOS33} [get_ports dma_ub_addr_in[0]]
set_property -dict {PACKAGE_PIN G2 IOSTANDARD LVCMOS33} [get_ports dma_ub_addr_in[1]]
set_property -dict {PACKAGE_PIN H1 IOSTANDARD LVCMOS33} [get_ports dma_ub_addr_in[2]]
set_property -dict {PACKAGE_PIN K2 IOSTANDARD LVCMOS33} [get_ports dma_ub_addr_in[3]]

# PMOD JB (Header J2)
set_property -dict {PACKAGE_PIN A14 IOSTANDARD LVCMOS33} [get_ports dma_ub_addr_in[4]]
set_property -dict {PACKAGE_PIN A16 IOSTANDARD LVCMOS33} [get_ports dma_ub_addr_in[5]]
set_property -dict {PACKAGE_PIN B15 IOSTANDARD LVCMOS33} [get_ports dma_ub_addr_in[6]]
set_property -dict {PACKAGE_PIN B16 IOSTANDARD LVCMOS33} [get_ports dma_ub_addr_in[7]]
set_property -dict {PACKAGE_PIN A15 IOSTANDARD LVCMOS33} [get_ports dma_length_in[0]]
set_property -dict {PACKAGE_PIN A17 IOSTANDARD LVCMOS33} [get_ports dma_length_in[1]]

# Instruction interface (using switches and remaining PMOD)
set_property -dict {PACKAGE_PIN V17 IOSTANDARD LVCMOS33} [get_ports instr_addr_out[0]]
set_property -dict {PACKAGE_PIN V16 IOSTANDARD LVCMOS33} [get_ports instr_addr_out[1]]
set_property -dict {PACKAGE_PIN W16 IOSTANDARD LVCMOS33} [get_ports instr_addr_out[2]]
set_property -dict {PACKAGE_PIN W17 IOSTANDARD LVCMOS33} [get_ports instr_addr_out[3]]
set_property -dict {PACKAGE_PIN W15 IOSTANDARD LVCMOS33} [get_ports instr_addr_out[4]]
set_property -dict {PACKAGE_PIN V15 IOSTANDARD LVCMOS33} [get_ports instr_addr_out[5]]
set_property -dict {PACKAGE_PIN W14 IOSTANDARD LVCMOS33} [get_ports instr_addr_out[6]]
set_property -dict {PACKAGE_PIN W13 IOSTANDARD LVCMOS33} [get_ports instr_addr_out[7]]

# Timing Constraints
# =============================================================================

# Input delay constraints for external interfaces
set_input_delay -clock [get_clocks sys_clk_pin] -min 1.0 [get_ports {dma_*_in instr_data_in}]
set_input_delay -clock [get_clocks sys_clk_pin] -max 2.0 [get_ports {dma_*_in instr_data_in}]

# Output delay constraints
set_output_delay -clock [get_clocks sys_clk_pin] -min 1.0 [get_ports {dma_*_out}]
set_output_delay -clock [get_clocks sys_clk_pin] -max 2.0 [get_ports {dma_*_out}]

# False path constraints for asynchronous interfaces
set_false_path -from [get_ports rst_n] -to [get_clocks clk_out]

# Multi-cycle path constraints for systolic array
set_multicycle_path -from [get_pins systolic_array/pe_*/*/CLK] -to [get_pins accumulators/buffer*/CLK] 2

# Physical Constraints
# =============================================================================

# Place MMCM near clock input
set_property LOC MMCME2_ADV_X0Y0 [get_cells clock_manager_inst/mmcm_inst]

# Reserve DSP48 slices for PE MAC operations
# This is a hint to the placer - actual DSP usage depends on synthesis
create_pblock pblock_dsp
add_cells_to_pblock [get_pblocks pblock_dsp] [get_cells -hierarchical -filter {NAME =~ "*pe*"}]
resize_pblock [get_pblocks pblock_dsp] -add {DSP48E2_X0Y0:DSP48E2_X1Y9}

# Reserve BRAM for memories
create_pblock pblock_bram
add_cells_to_pblock [get_pblocks pblock_bram] [get_cells -hierarchical -filter {NAME =~ "*buffer*"}]
add_cells_to_pblock [get_pblocks pblock_bram] [get_cells -hierarchical -filter {NAME =~ "*fifo*"}]
resize_pblock [get_pblocks pblock_bram] -add {RAMB36_X0Y0:RAMB36_X1Y9}

# Power Constraints
# =============================================================================

# Set power optimization
set_property -dict {MAX_FANOUT 100} [get_ports {*}]
set_property -dict {MAX_FANOUT 50} [get_nets -hierarchical -filter {NAME =~ "*psum*"}]

# =============================================================================
# Additional Constraints for Nexys A7 Compatibility
# =============================================================================

# Uncomment these lines for Nexys A7 instead of Basys3:
#
# # Clock (Nexys A7)
# set_property -dict {PACKAGE_PIN E3 IOSTANDARD LVCMOS33} [get_ports clk_in]
#
# # Reset (Nexys A7 BTNC button)
# set_property -dict {PACKAGE_PIN C12 IOSTANDARD LVCMOS33} [get_ports rst_n]
#
# # LEDs (Nexys A7)
# set_property -dict {PACKAGE_PIN H17 IOSTANDARD LVCMOS33} [get_ports tpu_busy]
# set_property -dict {PACKAGE_PIN K15 IOSTANDARD LVCMOS33} [get_ports tpu_done]
# set_property -dict {PACKAGE_PIN J13 IOSTANDARD LVCMOS33} [get_ports hazard_detected]
# set_property -dict {PACKAGE_PIN N14 IOSTANDARD LVCMOS33} [get_ports pipeline_stage[0]]
# set_property -dict {PACKAGE_PIN R18 IOSTANDARD LVCMOS33} [get_ports pipeline_stage[1]]
#
# # PMOD headers (Nexys A7 - JA and JB)
# # Similar pin assignments as Basys3 but different package pins

# =============================================================================
# Synthesis and Implementation Options
# =============================================================================

# Enable retiming for better timing closure
set_property RETIMING true [current_design]

# Set maximum fanout
set_property MAX_FANOUT 100 [current_design]

# Configuration options
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
set_property BITSTREAM.CONFIG.CONFIGRATE 50 [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
