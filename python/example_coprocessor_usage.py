#!/usr/bin/env python3
"""
Example: Using TPU as Coprocessor with UART Streaming
Demonstrates all 20 ISA instructions via streaming mode
"""

import numpy as np
from tpu_coprocessor_driver import TPU_Coprocessor, Opcode
from instruction_encoder import assemble_program

def example_1_simple_streaming():
    """Example 1: Simple instruction streaming"""
    print("\n" + "="*60)
    print("Example 1: Simple Instruction Streaming")
    print("="*60)
    
    tpu = TPU_Coprocessor(port='/dev/ttyUSB0', verbose=True)
    
    try:
        # Enable streaming mode
        if not tpu.enable_stream_mode():
            print("Failed to enable streaming mode")
            return
        
        # Stream simple instructions
        instructions = [
            (Opcode.NOP, 0, 0, 0, 0),
            (Opcode.CFG_REG, 0, 0x06, 0x66, 0),  # Set config register
            (Opcode.HALT, 0, 0, 0, 0)
        ]
        
        sent = tpu.stream_instructions(instructions)
        print(f"\n✓ Sent {sent}/{len(instructions)} instructions")
        
    finally:
        tpu.close()

def example_2_matrix_multiply():
    """Example 2: Complete matrix multiply pipeline"""
    print("\n" + "="*60)
    print("Example 2: Matrix Multiply Pipeline")
    print("="*60)
    
    tpu = TPU_Coprocessor(port='/dev/ttyUSB0', verbose=True)
    
    try:
        # Prepare data
        activations = np.ones((4, 256), dtype=np.uint8) * 2
        weights = np.ones((256, 256), dtype=np.uint8) * 3
        
        # Load data
        print("\nLoading data...")
        tpu.write_ub(0, activations)
        tpu.write_weights(0, weights)
        
        # Enable streaming mode
        if not tpu.enable_stream_mode():
            print("Failed to enable streaming mode")
            return
        
        # Stream computation instructions
        instructions = [
            (Opcode.MATMUL, 0, 0x20, 4, 0),      # Matrix multiply: UB[0] × Weights → Acc[0x20]
            (Opcode.SYNC, 0x01, 0x00, 0x10, 0),  # Wait for systolic completion
            (Opcode.RELU, 0x20, 0x40, 4, 0),     # ReLU: Acc[0x20] → UB[0x40]
            (Opcode.SYNC, 0x02, 0x00, 0x10, 0),  # Wait for VPU completion
            (Opcode.WR_HOST_MEM, 0x40, 0x01, 0x00, 0),  # Send results to host
            (Opcode.HALT, 0, 0, 0, 0)
        ]
        
        sent = tpu.stream_instructions(instructions)
        print(f"\n✓ Sent {sent}/{len(instructions)} instructions")
        
        # Wait for completion
        print("\nWaiting for completion...")
        if tpu.wait_done(timeout=5.0):
            print("✓ Computation complete!")
            
            # Read results
            results = tpu.read_ub(0x40, 256)
            print(f"✓ Read {len(results)} bytes of results")
        else:
            print("⚠ Timeout waiting for completion")
        
    finally:
        tpu.close()

def example_3_all_instructions():
    """Example 3: Test all 20 ISA instructions"""
    print("\n" + "="*60)
    print("Example 3: All 20 ISA Instructions")
    print("="*60)
    
    tpu = TPU_Coprocessor(port='/dev/ttyUSB0', verbose=True)
    
    try:
        # Prepare test data
        test_data = np.arange(256, dtype=np.uint8)
        tpu.write_ub(0, test_data)
        tpu.write_weights(0, np.ones(256, dtype=np.uint8))
        
        # Enable streaming mode
        if not tpu.enable_stream_mode():
            print("Failed to enable streaming mode")
            return
        
        # All 20 instructions
        instructions = [
            # Control
            (Opcode.NOP, 0, 0, 0, 0),
            
            # Memory
            (Opcode.RD_HOST_MEM, 0, 0x01, 0x00, 0),
            (Opcode.WR_HOST_MEM, 0, 0x01, 0x00, 0),
            (Opcode.RD_WEIGHT, 0, 0x01, 0, 0),
            (Opcode.LD_UB, 0, 0x04, 0, 0),
            (Opcode.ST_UB, 0x20, 0x04, 0, 0),
            
            # Compute
            (Opcode.MATMUL, 0, 0x20, 4, 0),
            (Opcode.CONV2D, 0, 0x20, 0x03, 0),
            (Opcode.MATMUL_ACC, 0, 0x20, 4, 0),
            
            # Activation
            (Opcode.RELU, 0x20, 0x40, 4, 0),
            (Opcode.RELU6, 0x20, 0x40, 4, 0),
            (Opcode.SIGMOID, 0x20, 0x40, 4, 0),
            (Opcode.TANH, 0x20, 0x40, 4, 0),
            
            # Pooling
            (Opcode.MAXPOOL, 0, 0x20, 0, 0),
            (Opcode.AVGPOOL, 0, 0x20, 0, 0),
            
            # Other
            (Opcode.ADD_BIAS, 0x20, 0x40, 4, 0),
            (Opcode.BATCH_NORM, 0, 0x20, 0, 0),
            
            # Control
            (Opcode.SYNC, 0x01, 0x00, 0x10, 0),
            (Opcode.CFG_REG, 0, 0x06, 0x66, 0),
            (Opcode.HALT, 0, 0, 0, 0)
        ]
        
        print(f"\nStreaming {len(instructions)} instructions...")
        sent = tpu.stream_instructions(instructions, flow_control=True)
        print(f"✓ Sent {sent}/{len(instructions)} instructions")
        
        # Print statistics
        tpu.print_stats()
        
    finally:
        tpu.close()

def example_4_neural_network_layer():
    """Example 4: Complete neural network layer"""
    print("\n" + "="*60)
    print("Example 4: Neural Network Layer")
    print("="*60)
    
    tpu = TPU_Coprocessor(port='/dev/ttyUSB0', verbose=True)
    
    try:
        # Layer parameters
        batch_size = 4
        input_dim = 256
        output_dim = 256
        
        # Prepare data
        print("\nPreparing data...")
        input_data = np.random.randint(0, 8, (batch_size, input_dim), dtype=np.uint8)
        weights = np.random.randint(0, 8, (input_dim, output_dim), dtype=np.uint8)
        
        tpu.write_ub(0, input_data)
        tpu.write_weights(0, weights)
        
        # Enable streaming mode
        if not tpu.enable_stream_mode():
            print("Failed to enable streaming mode")
            return
        
        # Neural network layer: MATMUL + ReLU
        instructions = [
            # Load weights
            (Opcode.RD_WEIGHT, 0, 0x04, 0, 0),  # Load 4 weight tiles
            (Opcode.SYNC, 0x08, 0x00, 0x20, 0),  # Wait for weight load
            
            # Matrix multiply
            (Opcode.MATMUL, 0, 0x20, batch_size, 0),  # UB[0] × Weights → Acc[0x20]
            (Opcode.SYNC, 0x01, 0x00, 0x20, 0),       # Wait for completion
            
            # ReLU activation
            (Opcode.RELU, 0x20, 0x40, batch_size, 0),  # Acc[0x20] → UB[0x40]
            (Opcode.SYNC, 0x02, 0x00, 0x20, 0),       # Wait for VPU
            
            # Send results
            (Opcode.WR_HOST_MEM, 0x40, batch_size * output_dim >> 8, 
             batch_size * output_dim & 0xFF, 0),
            
            (Opcode.HALT, 0, 0, 0, 0)
        ]
        
        print("\nStreaming neural network layer instructions...")
        sent = tpu.stream_instructions(instructions)
        print(f"✓ Sent {sent}/{len(instructions)} instructions")
        
        # Wait and read results
        if tpu.wait_done(timeout=10.0):
            results = tpu.read_ub(0x40, batch_size * output_dim)
            print(f"✓ Layer complete! Results: {len(results)} bytes")
        else:
            print("⚠ Timeout")
        
    finally:
        tpu.close()

if __name__ == '__main__':
    import sys
    
    if len(sys.argv) > 1:
        example_num = int(sys.argv[1])
    else:
        example_num = 1
    
    examples = {
        1: example_1_simple_streaming,
        2: example_2_matrix_multiply,
        3: example_3_all_instructions,
        4: example_4_neural_network_layer
    }
    
    if example_num in examples:
        examples[example_num]()
    else:
        print("Usage: python3 example_coprocessor_usage.py [1-4]")
        print("\nExamples:")
        print("  1: Simple instruction streaming")
        print("  2: Matrix multiply pipeline")
        print("  3: All 20 ISA instructions")
        print("  4: Complete neural network layer")

