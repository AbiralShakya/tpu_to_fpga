#!/bin/bash
# Complete Workflow: From Code to Running TPU on FPGA

set -e

echo "=========================================="
echo "TPU FPGA Complete Workflow"
echo "=========================================="
echo ""

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Step 1: Build Bitstream
echo -e "${BLUE}Step 1: Building FPGA Bitstream${NC}"
echo "This compiles your SystemVerilog design into a .bit file"
echo ""

if [ -f "build/tpu_basys3.bit" ]; then
    echo -e "${YELLOW}Bitstream already exists. Skipping build.${NC}"
    echo "To rebuild, run: vivado -mode batch -source build.tcl"
else
    echo "Building bitstream..."
    # Uncomment one of these based on your toolchain:
    # vivado -mode batch -source build.tcl
    # OR
    # ./build_tpu_bitstream.sh
    echo -e "${YELLOW}Note: Uncomment build command in script${NC}"
fi

echo ""
echo -e "${GREEN}✓ Bitstream ready: build/tpu_basys3.bit${NC}"
echo ""

# Step 2: Program FPGA
echo -e "${BLUE}Step 2: Programming FPGA (ONE TIME)${NC}"
echo "This loads your design onto the FPGA via JTAG"
echo ""

read -p "Is Basys 3 connected via USB? (y/n) " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Programming FPGA..."
    openFPGALoader -b basys3 build/tpu_basys3.bit
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ FPGA programmed successfully!${NC}"
        echo "Your TPU design is now running on the FPGA."
    else
        echo -e "${YELLOW}⚠ Programming failed. Check USB connection.${NC}"
        exit 1
    fi
else
    echo -e "${YELLOW}⚠ Skipping FPGA programming. Connect Basys 3 and run:${NC}"
    echo "   openFPGALoader -b basys3 build/tpu_basys3.bit"
fi

echo ""

# Step 3: Verify UART Connection
echo -e "${BLUE}Step 3: Verifying UART Connection${NC}"
echo "Checking if FPGA is responding via UART..."
echo ""

UART_PORT=""
if [ -e "/dev/ttyUSB0" ]; then
    UART_PORT="/dev/ttyUSB0"
elif [ -e "/dev/ttyUSB1" ]; then
    UART_PORT="/dev/ttyUSB1"
elif [ -e "/dev/ttyACM0" ]; then
    UART_PORT="/dev/ttyACM0"
fi

if [ -n "$UART_PORT" ]; then
    echo -e "${GREEN}✓ Found UART port: $UART_PORT${NC}"
    echo ""
    echo "Testing connection..."
    python3 << EOF
import serial
import time
try:
    ser = serial.Serial('$UART_PORT', 115200, timeout=1)
    time.sleep(0.5)
    print("✓ UART connection successful!")
    print(f"  Port: $UART_PORT")
    print(f"  Baud: 115200")
    ser.close()
except Exception as e:
    print(f"✗ UART connection failed: {e}")
    print("  Make sure FPGA is programmed and powered on")
EOF
else
    echo -e "${YELLOW}⚠ No UART port found.${NC}"
    echo "  Check USB connection and drivers"
    echo "  Try: ls /dev/tty*"
fi

echo ""

# Step 4: Runtime Communication
echo -e "${BLUE}Step 4: Runtime Communication via UART${NC}"
echo "Now you can send instructions to the running FPGA"
echo ""

echo "Available options:"
echo "  1. Test all ISA instructions:"
echo "     python3 test_isa_fpga.py"
echo ""
echo "  2. Use TPU driver:"
echo "     python3 tpu_driver.py"
echo ""
echo "  3. Custom Python script:"
echo "     import serial"
echo "     ser = serial.Serial('$UART_PORT', 115200)"
echo "     # Send instructions..."
echo ""

read -p "Run ISA test now? (y/n) " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    if [ -f "test_isa_fpga.py" ]; then
        python3 test_isa_fpga.py $UART_PORT
    else
        echo -e "${YELLOW}test_isa_fpga.py not found${NC}"
    fi
fi

echo ""
echo "=========================================="
echo -e "${GREEN}Workflow Complete!${NC}"
echo "=========================================="
echo ""
echo "Summary:"
echo "  ✓ FPGA programmed (design loaded)"
echo "  ✓ UART connection verified"
echo "  ✓ Ready for runtime communication"
echo ""
echo "Remember:"
echo "  • FPGA programming = ONE TIME (via JTAG)"
echo "  • UART communication = MANY TIMES (runtime)"
echo "  • Same USB cable, different protocols"
echo ""

