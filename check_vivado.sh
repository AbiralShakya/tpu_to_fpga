#!/bin/bash
# Diagnostic script to find Vivado on Adroit

echo "=========================================="
echo "Vivado Diagnostic on Adroit"
echo "=========================================="
echo ""

echo "1. Checking module system..."
module avail 2>&1 | grep -i vivado || echo "   No Vivado in modules"
module avail 2>&1 | grep -i xilinx || echo "   No Xilinx in modules"
echo ""

echo "2. Checking common installation paths..."
[ -d /usr/licensed/vivado ] && echo "   ✓ Found: /usr/licensed/vivado" || echo "   ✗ Not in /usr/licensed/vivado"
[ -d /opt/Xilinx ] && echo "   ✓ Found: /opt/Xilinx" || echo "   ✗ Not in /opt/Xilinx"
[ -d /tools/Xilinx ] && echo "   ✓ Found: /tools/Xilinx" || echo "   ✗ Not in /tools/Xilinx"
echo ""

echo "3. Searching for vivado binary..."
FOUND=$(find /usr /opt /tools -name "vivado" 2>/dev/null | head -1)
if [ -n "$FOUND" ]; then
    echo "   ✓ Found: $FOUND"
    ls -lh "$FOUND"
    file "$FOUND"
else
    echo "   ✗ Vivado binary not found"
fi
echo ""

echo "4. Checking environment variables..."
echo "   XILINX_VIVADO: ${XILINX_VIVADO:-not set}"
echo "   XILINX: ${XILINX:-not set}"
echo ""

echo "5. Checking which command..."
which vivado 2>/dev/null && echo "   ✓ vivado in PATH" || echo "   ✗ vivado not in PATH"
echo ""

echo "=========================================="
echo "Next Steps:"
echo "=========================================="
echo "If Vivado was found, you can:"
echo "  export PATH=<vivado_path>/bin:\$PATH"
echo "  vivado -version"
echo ""
echo "If not found, contact: cses@princeton.edu"
echo ""
