#!/bin/bash
# Setup script for TPU Coprocessor Python environment

echo "🔧 Setting up TPU Coprocessor Python environment..."

# Check if virtual environment exists
if [ ! -d "venv" ]; then
    echo "📦 Creating virtual environment..."
    python3 -m venv venv

    echo "✅ Virtual environment created"
else
    echo "✅ Virtual environment already exists"
fi

# Activate virtual environment
echo "🔌 Activating virtual environment..."
source venv/bin/activate

# Install dependencies
echo "📥 Installing dependencies..."
pip install --upgrade pip
pip install pyserial numpy

# Test import
echo "🧪 Testing driver import..."
python3 -c "from tpu_coprocessor_driver import TPU_Coprocessor; print('✓ Driver import successful')"

echo ""
echo "✨ Setup complete!"
echo ""
echo "To use the TPU driver in a new terminal session:"
# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
echo "  1. cd \"$PROJECT_ROOT\""
echo "  2. source venv/bin/activate"
echo "  3. python3 test_all_instructions.py /dev/tty.usbserial-XXXX"
echo ""
echo "Your virtual environment is now active (notice the (venv) prefix)"
