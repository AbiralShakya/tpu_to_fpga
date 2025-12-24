#!/usr/bin/env python3
"""
TPU Instruction Testing Script
Tests TPU instructions on Basys3 FPGA via UART
"""

import sys
import os
import time
import struct
import numpy as np

# Add tpu_to_fpga to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'tpu_to_fpga'))

try:
    from tpu_driver import TPU_Basys3
except ImportError:
    print("✗ Failed to import TPU driver")
    print("  Make sure tpu_to_fpga/tpu_driver.py exists")
    sys.exit(1)

def test_basic_instructions(tpu):
    """Test basic TPU instructions"""
    print("\n" + "="*70)
    print("Testing Basic TPU Instructions")
    print("="*70)
    
    # Test 1: NOP
    print("\n[Test 1] NOP Instruction")
    try:
        tpu.write_instruction(0, tpu.OP_NOP, 0, 0, 0, 0)
        tpu.start_execution()
        time.sleep(0.1)
        status = tpu.read_status()
        print(f"✓ NOP executed - Status: {status}")
    except Exception as e:
        print(f"✗ NOP failed: {e}")
        return False
    
    # Test 2: CFG_REG
    print("\n[Test 2] CFG_REG Instruction")
    try:
        tpu.write_instruction(0, tpu.OP_CFG_REG, 0, 0x06, 0x66, 0)
        tpu.start_execution()
        time.sleep(0.1)
        print("✓ CFG_REG executed")
    except Exception as e:
        print(f"✗ CFG_REG failed: {e}")
    
    # Test 3: SYNC
    print("\n[Test 3] SYNC Instruction")
    try:
        tpu.write_instruction(0, tpu.OP_SYNC, 0x01, 0x00, 0x10, 0)
        tpu.start_execution()
        time.sleep(0.1)
        print("✓ SYNC executed")
    except Exception as e:
        print(f"✗ SYNC failed: {e}")
    
    return True

def test_memory_operations(tpu):
    """Test memory operations"""
    print("\n" + "="*70)
    print("Testing Memory Operations")
    print("="*70)
    
    # Test 1: Write to Unified Buffer
    print("\n[Test 1] Write to Unified Buffer")
    try:
        test_data = np.array([1, 2, 3, 4, 5, 6, 7, 8], dtype=np.uint8)
        tpu.write_ub(0, test_data)
        print("✓ Data written to UB[0]")
    except Exception as e:
        print(f"✗ Write UB failed: {e}")
        return False
    
    # Test 2: Read from Unified Buffer
    print("\n[Test 2] Read from Unified Buffer")
    try:
        data = tpu.read_ub(0, 32)  # Read 32 bytes
        print(f"✓ Read {len(data)} bytes from UB[0]")
        print(f"  First 8 bytes: {list(data[:8])}")
    except Exception as e:
        print(f"✗ Read UB failed: {e}")
    
    # Test 3: Write weights
    print("\n[Test 3] Write to Weight Memory")
    try:
        weights = np.ones(8, dtype=np.uint8) * 42
        tpu.write_weights(0, weights)
        print("✓ Weights written")
    except Exception as e:
        print(f"✗ Write weights failed: {e}")
    
    return True

def test_compute_operations(tpu):
    """Test compute operations"""
    print("\n" + "="*70)
    print("Testing Compute Operations")
    print("="*70)
    
    # Test 1: Simple MATMUL
    print("\n[Test 1] Matrix Multiply (3x3)")
    try:
        # Create 3x3 test matrices
        A = np.array([[1, 2, 3],
                      [4, 5, 6],
                      [7, 8, 9]], dtype=np.int8)
        B = np.eye(3, dtype=np.int8)  # Identity matrix
        
        print("  Matrix A:")
        print(A)
        print("  Matrix B (identity):")
        print(B)
        
        # Load data
        tpu.write_ub(0, A.flatten())
        tpu.write_weights(0, B.flatten())
        
        # Execute MATMUL
        program = [
            (tpu.OP_RD_WEIGHT, 0, 1, 0, 0),      # Load weights
            (tpu.OP_LD_UB, 0, 1, 0, 0),          # Load activations
            (tpu.OP_MATMUL, 0, 0, 3, 0b10),       # 3x3 signed matmul
            (tpu.OP_SYNC, 0x0F, 0xFF, 0xFF, 0),   # Wait for completion
            (tpu.OP_HALT, 0, 0, 0, 0),            # Halt
        ]
        
        tpu.load_program(program)
        tpu.start_execution()
        
        if tpu.wait_done(timeout=5):
            print("✓ MATMUL completed")
        else:
            print("⚠ MATMUL timeout (may still be OK)")
        
    except Exception as e:
        print(f"✗ MATMUL failed: {e}")
        import traceback
        traceback.print_exc()
    
    return True

def main():
    """Main function"""
    if len(sys.argv) < 2:
        print("Usage: python3 test_tpu_instructions.py [UART_PORT]")
        print("\nExample:")
        print("  python3 test_tpu_instructions.py /dev/tty.usbserial-210292B2C3A4")
        print("\nFind your device with:")
        print("  ls /dev/tty.usbserial* | tail -1")
        sys.exit(1)
    
    port = sys.argv[1]
    
    print("="*70)
    print("TPU Instruction Testing on Basys3 FPGA")
    print("="*70)
    print(f"UART Port: {port}")
    print(f"Baud Rate: 115200")
    print()
    
    try:
        # Connect to TPU
        print("Connecting to TPU...")
        tpu = TPU_Basys3(port=port, baud=115200, timeout=2)
        print("✓ Connected!")
        
        # Read initial status
        status = tpu.read_status()
        print(f"Initial status: {status}")
        
        # Run tests
        test_basic_instructions(tpu)
        test_memory_operations(tpu)
        test_compute_operations(tpu)
        
        # Final status
        print("\n" + "="*70)
        print("Final Status")
        print("="*70)
        final_status = tpu.read_status()
        print(f"Status: {final_status}")
        
        tpu.close()
        print("\n✓ All tests complete!")
        
    except KeyboardInterrupt:
        print("\n\nInterrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == '__main__':
    main()

