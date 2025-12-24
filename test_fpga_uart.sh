#!/bin/bash
# Comprehensive FPGA Testing Script for Basys3
# Tests bitstream loading, UART communication, and TPU instructions

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Basys3 FPGA Testing Script${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Step 1: Check for bitstream file
echo -e "${CYAN}Step 1: Checking for bitstream file...${NC}"
BITSTREAM=""
if [ -f "simple_test.bit" ]; then
    BITSTREAM="simple_test.bit"
    echo -e "${GREEN}✓ Found: simple_test.bit${NC}"
elif [ -f "build/simple_test.bit" ]; then
    BITSTREAM="build/simple_test.bit"
    echo -e "${GREEN}✓ Found: build/simple_test.bit${NC}"
elif [ -f "tpu_to_fpga/build/tpu_basys3.bit" ]; then
    BITSTREAM="tpu_to_fpga/build/tpu_basys3.bit"
    echo -e "${GREEN}✓ Found: tpu_to_fpga/build/tpu_basys3.bit${NC}"
else
    echo -e "${RED}✗ No bitstream file found!${NC}"
    echo "  Looking for: simple_test.bit, build/simple_test.bit, or tpu_to_fpga/build/tpu_basys3.bit"
    exit 1
fi

# Step 2: Check for openFPGALoader
echo ""
echo -e "${CYAN}Step 2: Checking for openFPGALoader...${NC}"
if command -v openFPGALoader &> /dev/null; then
    echo -e "${GREEN}✓ openFPGALoader found${NC}"
    OPENFPGALOADER="openFPGALoader"
elif command -v openfpgaloader &> /dev/null; then
    echo -e "${GREEN}✓ openfpgaloader found${NC}"
    OPENFPGALOADER="openfpgaloader"
else
    echo -e "${YELLOW}⚠ openFPGALoader not found${NC}"
    echo "  Install with: brew install openfpgaloader"
    echo "  Or skip bitstream loading and program manually"
    read -p "Continue without programming? (y/n) " -n 1 -r
    echo ""
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
    OPENFPGALOADER=""
fi

# Step 3: Program FPGA (if openFPGALoader available)
if [ -n "$OPENFPGALOADER" ]; then
    echo ""
    echo -e "${CYAN}Step 3: Programming FPGA...${NC}"
    echo "  Command: $OPENFPGALOADER -b basys3 $BITSTREAM"
    read -p "Program FPGA now? (y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        if $OPENFPGALOADER -b basys3 "$BITSTREAM"; then
            echo -e "${GREEN}✓ FPGA programmed successfully!${NC}"
            sleep 2  # Wait for FPGA to initialize
        else
            echo -e "${RED}✗ Programming failed${NC}"
            echo "  Make sure Basys3 is connected via USB"
            exit 1
        fi
    else
        echo "  Skipping programming (assuming already programmed)"
    fi
else
    echo ""
    echo -e "${YELLOW}Step 3: Skipping FPGA programming (openFPGALoader not available)${NC}"
    echo "  Program manually with: openFPGALoader -b basys3 $BITSTREAM"
fi

# Step 4: Find UART device
echo ""
echo -e "${CYAN}Step 4: Finding UART device...${NC}"
UART_PORT=""

# Try Mac USB serial devices
if ls /dev/tty.usbserial* 1> /dev/null 2>&1; then
    # Get the last one (usually the UART, not JTAG)
    UART_PORT=$(ls /dev/tty.usbserial* | tail -1)
    echo -e "${GREEN}✓ Found UART device: $UART_PORT${NC}"
elif ls /dev/cu.usbserial* 1> /dev/null 2>&1; then
    UART_PORT=$(ls /dev/cu.usbserial* | tail -1)
    echo -e "${GREEN}✓ Found UART device: $UART_PORT${NC}"
elif ls /dev/ttyUSB* 1> /dev/null 2>&1; then
    UART_PORT=$(ls /dev/ttyUSB* | tail -1)
    echo -e "${GREEN}✓ Found UART device: $UART_PORT${NC}"
else
    echo -e "${RED}✗ No UART device found${NC}"
    echo "  Make sure Basys3 is connected and powered on"
    echo "  Try: ls /dev/tty.* | grep -i usb"
    exit 1
fi

# Step 5: Test UART connection
echo ""
echo -e "${CYAN}Step 5: Testing UART connection...${NC}"
python3 << EOF
import serial
import time
import sys

port = "$UART_PORT"
print(f"Connecting to {port} at 115200 baud...")

try:
    ser = serial.Serial(port, 115200, timeout=2)
    time.sleep(2)  # Wait for FPGA to initialize
    
    # Test 1: Send status query (CMD_READ_STATUS = 0x06)
    print("  Sending status query (0x06)...")
    ser.write(bytes([0x06]))
    time.sleep(0.1)
    
    status = ser.read(1)
    if len(status) == 1:
        status_byte = status[0]
        print(f"  ✓ UART working! Status byte: 0x{status_byte:02X}")
        print(f"    sys_busy: {status_byte & 0x01}")
        print(f"    sys_done: {(status_byte >> 1) & 0x01}")
        print(f"    vpu_busy: {(status_byte >> 2) & 0x01}")
        print(f"    vpu_done: {(status_byte >> 3) & 0x01}")
        print(f"    ub_busy:  {(status_byte >> 4) & 0x01}")
        print(f"    ub_done:  {(status_byte >> 5) & 0x01}")
    else:
        print("  ⚠ No response (this is OK if simple_test design doesn't respond)")
        print("    The FPGA is connected but may not have UART status handler")
    
    ser.close()
    print("  ✓ UART connection test complete")
    
except serial.SerialException as e:
    print(f"  ✗ Serial connection failed: {e}")
    print("    Check permissions: sudo chmod 666 $port")
    sys.exit(1)
except Exception as e:
    print(f"  ✗ Error: {e}")
    sys.exit(1)
EOF

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ UART test failed${NC}"
    exit 1
fi

# Step 6: Test with TPU driver (if available)
echo ""
echo -e "${CYAN}Step 6: Testing with TPU driver...${NC}"

# Check which driver is available
DRIVER=""
if [ -f "tpu_to_fpga/tpu_driver.py" ]; then
    DRIVER="tpu_to_fpga/tpu_driver.py"
    echo -e "${GREEN}✓ Found: tpu_to_fpga/tpu_driver.py${NC}"
elif [ -f "tpu_driver.py" ]; then
    DRIVER="tpu_driver.py"
    echo -e "${GREEN}✓ Found: tpu_driver.py${NC}"
else
    echo -e "${YELLOW}⚠ TPU driver not found${NC}"
    echo "  Skipping TPU instruction tests"
    DRIVER=""
fi

if [ -n "$DRIVER" ]; then
    echo ""
    echo "Running basic TPU driver test..."
    python3 << EOF
import sys
import os
sys.path.insert(0, os.path.dirname("$DRIVER"))

try:
    from tpu_driver import TPU_Basys3
    import time
    
    port = "$UART_PORT"
    print(f"Connecting to TPU on {port}...")
    
    tpu = TPU_Basys3(port=port, baud=115200, timeout=2)
    
    # Test status reading
    print("  Reading TPU status...")
    status = tpu.read_status()
    if status:
        print(f"  ✓ Status read successful!")
        print(f"    sys_busy: {status['sys_busy']}")
        print(f"    sys_done: {status['sys_done']}")
        print(f"    vpu_busy: {status['vpu_busy']}")
        print(f"    vpu_done: {status['vpu_done']}")
        print(f"    ub_busy:  {status['ub_busy']}")
        print(f"    ub_done:  {status['ub_done']}")
    else:
        print("  ⚠ Status read returned None (may be OK for simple_test design)")
    
    # Test NOP instruction
    print("  Testing NOP instruction...")
    try:
        tpu.write_instruction(0, tpu.OP_NOP, 0, 0, 0, 0)
        tpu.start_execution()
        print("  ✓ NOP instruction sent")
    except Exception as e:
        print(f"  ⚠ Instruction test failed: {e}")
        print("    (This is OK if simple_test design doesn't support full TPU)")
    
    tpu.close()
    print("  ✓ TPU driver test complete")
    
except ImportError as e:
    print(f"  ✗ Failed to import driver: {e}")
    print("    Install dependencies: pip3 install pyserial numpy")
    sys.exit(1)
except Exception as e:
    print(f"  ⚠ Driver test error: {e}")
    print("    (This may be OK if using simple_test design)")
EOF
fi

# Step 7: Summary and next steps
echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}Testing Complete!${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "Summary:"
echo "  • Bitstream: $BITSTREAM"
echo "  • UART Port: $UART_PORT"
echo ""
echo "Next steps:"
echo ""
echo "1. Test simple UART communication:"
echo "   python3 test_uart_simple.py $UART_PORT"
echo ""
echo "2. Test TPU instructions (if full TPU bitstream):"
echo "   python3 test_all_instructions.py $UART_PORT"
echo ""
echo "3. Use TPU driver for custom tests:"
echo "   python3 -c \"from tpu_to_fpga.tpu_driver import TPU_Basys3; tpu = TPU_Basys3('$UART_PORT'); print('Connected!')\""
echo ""
echo "4. Re-program FPGA:"
echo "   $OPENFPGALOADER -b basys3 $BITSTREAM"
echo ""

