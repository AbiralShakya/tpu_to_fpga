# =============================================================================
# TPU v1 Constraints for Basys3 - WITH TIMING CONSTRAINTS
# =============================================================================
# This file includes timing constraints for UART and other I/O
# Use this for Vivado builds (nextpnr-xilinx doesn't support timing constraints)
# =============================================================================

# Clock input (100MHz crystal)
set_property LOC W5 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]
create_clock -period 10.000 -name sys_clk [get_ports clk]

# Reset input (active low)
set_property LOC R2 [get_ports rst_n]
set_property IOSTANDARD LVCMOS33 [get_ports rst_n]

# UART Interface (USB-UART on Basys3)
set_property LOC B18 [get_ports uart_tx]
set_property IOSTANDARD LVCMOS33 [get_ports uart_tx]
set_property LOC A18 [get_ports uart_rx]
set_property IOSTANDARD LVCMOS33 [get_ports uart_rx]

# UART Timing Constraints (115200 baud = 8.68us per bit)
# For 100MHz clock, this is ~868 clock cycles per bit
# Set relaxed timing for UART (much slower than system clock)
set_input_delay -clock sys_clk -max 8.0 [get_ports uart_rx]
set_input_delay -clock sys_clk -min 0.0 [get_ports uart_rx]
set_output_delay -clock sys_clk -max 8.0 [get_ports uart_tx]
set_output_delay -clock sys_clk -min 0.0 [get_ports uart_tx]

# Basys 3 Physical Interface
# Switches (SW[14:0] - SW15 is reset)
set_property LOC V17 [get_ports sw0]
set_property LOC V16 [get_ports sw1]
set_property LOC W16 [get_ports sw2]
set_property LOC W17 [get_ports sw3]
set_property LOC W15 [get_ports sw4]
set_property LOC V15 [get_ports sw5]
set_property LOC W14 [get_ports sw6]
set_property LOC W13 [get_ports sw7]
set_property LOC V2 [get_ports sw8]
set_property LOC T3 [get_ports sw9]
set_property LOC T2 [get_ports sw10]
set_property LOC R3 [get_ports sw11]
set_property LOC W2 [get_ports sw12]
set_property LOC U1 [get_ports sw13]
set_property LOC T1 [get_ports sw14]

set_property IOSTANDARD LVCMOS33 [get_ports sw0]
set_property IOSTANDARD LVCMOS33 [get_ports sw1]
set_property IOSTANDARD LVCMOS33 [get_ports sw2]
set_property IOSTANDARD LVCMOS33 [get_ports sw3]
set_property IOSTANDARD LVCMOS33 [get_ports sw4]
set_property IOSTANDARD LVCMOS33 [get_ports sw5]
set_property IOSTANDARD LVCMOS33 [get_ports sw6]
set_property IOSTANDARD LVCMOS33 [get_ports sw7]
set_property IOSTANDARD LVCMOS33 [get_ports sw8]
set_property IOSTANDARD LVCMOS33 [get_ports sw9]
set_property IOSTANDARD LVCMOS33 [get_ports sw10]
set_property IOSTANDARD LVCMOS33 [get_ports sw11]
set_property IOSTANDARD LVCMOS33 [get_ports sw12]
set_property IOSTANDARD LVCMOS33 [get_ports sw13]
set_property IOSTANDARD LVCMOS33 [get_ports sw14]

# Push Buttons (using separate ports to avoid nextpnr-xilinx array bug)
# btn0=BTNC, btn1=BTNR, btn2=BTNL, btn3=BTND, btn4=BTNU
set_property LOC U18 [get_ports btn0]
set_property LOC T17 [get_ports btn1]
set_property LOC W19 [get_ports btn2]
set_property LOC U17 [get_ports btn3]
set_property LOC T18 [get_ports btn4]

set_property IOSTANDARD LVCMOS33 [get_ports btn0]
set_property IOSTANDARD LVCMOS33 [get_ports btn1]
set_property IOSTANDARD LVCMOS33 [get_ports btn2]
set_property IOSTANDARD LVCMOS33 [get_ports btn3]
set_property IOSTANDARD LVCMOS33 [get_ports btn4]

# 7-Segment Display (using separate ports)
set_property LOC W7 [get_ports seg0]
set_property LOC W6 [get_ports seg1]
set_property LOC U8 [get_ports seg2]
set_property LOC V8 [get_ports seg3]
set_property LOC U5 [get_ports seg4]
set_property LOC V5 [get_ports seg5]
set_property LOC U7 [get_ports seg6]

set_property IOSTANDARD LVCMOS33 [get_ports seg0]
set_property IOSTANDARD LVCMOS33 [get_ports seg1]
set_property IOSTANDARD LVCMOS33 [get_ports seg2]
set_property IOSTANDARD LVCMOS33 [get_ports seg3]
set_property IOSTANDARD LVCMOS33 [get_ports seg4]
set_property IOSTANDARD LVCMOS33 [get_ports seg5]
set_property IOSTANDARD LVCMOS33 [get_ports seg6]

# 7-Segment Anodes (using separate ports to avoid nextpnr-xilinx array bug)
set_property LOC U2 [get_ports an0]
set_property LOC U4 [get_ports an1]
set_property LOC V4 [get_ports an2]
set_property LOC W4 [get_ports an3]

set_property IOSTANDARD LVCMOS33 [get_ports an0]
set_property IOSTANDARD LVCMOS33 [get_ports an1]
set_property IOSTANDARD LVCMOS33 [get_ports an2]
set_property IOSTANDARD LVCMOS33 [get_ports an3]

# Decimal Point
set_property LOC V7 [get_ports dp]
set_property IOSTANDARD LVCMOS33 [get_ports dp]

# LEDs (LED[15:0] - using separate ports)
set_property LOC U16 [get_ports led0]
set_property LOC E19 [get_ports led1]
set_property LOC U19 [get_ports led2]
set_property LOC V19 [get_ports led3]
set_property LOC W18 [get_ports led4]
set_property LOC U15 [get_ports led5]
set_property LOC U14 [get_ports led6]
set_property LOC V14 [get_ports led7]
set_property LOC V13 [get_ports led8]
set_property LOC V3 [get_ports led9]
set_property LOC W3 [get_ports led10]
set_property LOC U3 [get_ports led11]
set_property LOC P3 [get_ports led12]
set_property LOC N3 [get_ports led13]
set_property LOC P1 [get_ports led14]
set_property LOC L1 [get_ports led15]

set_property IOSTANDARD LVCMOS33 [get_ports led0]
set_property IOSTANDARD LVCMOS33 [get_ports led1]
set_property IOSTANDARD LVCMOS33 [get_ports led2]
set_property IOSTANDARD LVCMOS33 [get_ports led3]
set_property IOSTANDARD LVCMOS33 [get_ports led4]
set_property IOSTANDARD LVCMOS33 [get_ports led5]
set_property IOSTANDARD LVCMOS33 [get_ports led6]
set_property IOSTANDARD LVCMOS33 [get_ports led7]
set_property IOSTANDARD LVCMOS33 [get_ports led8]
set_property IOSTANDARD LVCMOS33 [get_ports led9]
set_property IOSTANDARD LVCMOS33 [get_ports led10]
set_property IOSTANDARD LVCMOS33 [get_ports led11]
set_property IOSTANDARD LVCMOS33 [get_ports led12]
set_property IOSTANDARD LVCMOS33 [get_ports led13]
set_property IOSTANDARD LVCMOS33 [get_ports led14]
set_property IOSTANDARD LVCMOS33 [get_ports led15]

# Set false paths for asynchronous inputs (switches, buttons)
set_false_path -from [get_ports {sw* btn* rst_n}]

# Set false paths for UART (asynchronous serial interface)
set_false_path -from [get_ports uart_rx]
set_false_path -to [get_ports uart_tx]

# Note: The timing warnings you see are normal for:
# - no_clock: Internal signals that don't need clock constraints
# - unconstrained internal endpoints: Internal logic paths
# - no_input_delay/no_output_delay: Signals that don't need I/O timing
# These warnings are OK and won't prevent the design from working.


