#!/bin/bash

# Complete TPU Testing Script
# Run all tests to verify complete functionality before FPGA synthesis

echo "TPU testing"

cd /Users/abiralshakya/Documents/tpu_to_fpga/sim

# Activate virtual environment
echo "venv activation"
source ../tpu_env/bin/activate

echo "Runing all tests"


# Test 1: PE (Processing Element)
echo "Testing PE (Processing Element)..."
make test_pe
echo ""

# Test 2: MMU (Matrix Multiply Unit)
echo "Testing MMU (3x3 Systolic Array)..."
make test_mmu
echo ""

# Test 3: Accumulator
echo "Testing Accumulator (Double Buffering)..."
make test_accumulator
echo ""

# Test 4: Activation Function
echo "Testing Activation Function (ReLU)..."
make test_activation_func
echo ""

# Test 5: Normalizer
echo "Testing Normalizer (Gain/Bias/Shift)..."
make test_normalizer
echo ""

# Test 6: Activation Pipeline
echo "Testing Activation Pipeline..."
make test_activation_pipeline
echo ""

# Test 7: Weight FIFO
echo "Testing Weight FIFO..."
make test_weight_fifo
echo ""

# Test 8: Dual Weight FIFO
echo "Testing Dual Weight FIFO..."
make test_dual_fifo
echo ""

# Test 9: ISA Controller
echo "Testing ISA Controller..."
make test_isa
echo ""

# Test 10: MLP Integration
echo "Testing MLP Integration..."
make test_mlp
echo ""