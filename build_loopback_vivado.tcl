# Build UART loopback test with Vivado locally

set part_name "xc7a35tcpg236-1"
set top_module "uart_loopback_test"

puts "=========================================="
puts "UART Loopback Test Build (Vivado)"
puts "=========================================="

create_project -in_memory -part ${part_name} -force

# Read EDIF
if {![file exists build/uart_loopback.edif]} {
    puts "ERROR: build/uart_loopback.edif not found!"
    exit 1
}
read_edif build/uart_loopback.edif

set_property top ${top_module} [current_fileset]

# Read constraints
if {![file exists constraints/uart_loopback.xdc]} {
    puts "ERROR: constraints/uart_loopback.xdc not found!"
    exit 1
}
read_xdc constraints/uart_loopback.xdc

# Link
link_design -part ${part_name} -top ${top_module}

# Implementation
puts "Running implementation..."
opt_design
place_design
phys_opt_design
route_design

# Timing
puts "Timing analysis..."
report_timing_summary -file build/loopback_timing.rpt
set wns [get_property SLACK [get_timing_paths -max_paths 1 -nworst 1]]
puts "WNS = ${wns} ns"

# Utilization
report_utilization

# Bitstream
write_bitstream -force build/uart_loopback.bit

if {[file exists build/uart_loopback.bit]} {
    puts "=========================================="
    puts "✓ LOOPBACK TEST BUILD SUCCESSFUL!"
    puts "=========================================="
    puts "Bitstream: build/uart_loopback.bit"
    puts ""
    puts "To test:"
    puts "  1. Make sure SW15 (reset) is UP"
    puts "  2. openFPGALoader -b basys3 build/uart_loopback.bit"
    puts "  3. python3 -c \"import serial; s=serial.Serial('/dev/tty.usbserial-210183A27BE01',115200,timeout=1); s.write(b'\\x06'); print('Sent 0x06, got:', hex(ord(s.read(1))) if s.read(1) else 'NOTHING')\""
    puts "  (Should print 0x7 which is 0x6+1)"
} else {
    puts "ERROR: Bitstream generation failed!"
    exit 1
}

close_project
exit 0
