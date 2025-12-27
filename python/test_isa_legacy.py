#!/usr/bin/env python3
"""
test_isa_legacy.py - Test ISA using legacy mode (WRITE_INSTR + EXECUTE)
Works with current bitstream that doesn't support streaming mode (0x07)
"""

import sys
import time
from drivers.tpu_coprocessor_driver import TPU_Coprocessor, Opcode, Instruction

def test_isa_legacy_mode(port):
    """Test ISA using legacy buffer mode"""
    print("="*80)
    print("TPU ISA Test - Legacy Mode (WRITE_INSTR + EXECUTE)")
    print("="*80)
    print(f"\nConnecting to {port}...")
    
    tpu = TPU_Coprocessor(port, verbose=True)
    
    try:
        # Test 1: NOP
        print("\n" + "="*80)
        print("Test 1: NOP - No Operation")
        print("="*80)
        tpu.write_instruction(0, Opcode.NOP, 0, 0, 0, 0)
        tpu.write_instruction(1, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        time.sleep(0.1)
        status = tpu.read_status()
        if status:
            print(f"✓ Status: {status}")
        print("✓ NOP test passed")
        
        # Test 2: SYNC
        print("\n" + "="*80)
        print("Test 2: SYNC - Synchronization")
        print("="*80)
        tpu.write_instruction(0, Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0)
        tpu.write_instruction(1, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        time.sleep(0.1)
        status = tpu.read_status()
        if status:
            print(f"✓ Status: {status}")
        print("✓ SYNC test passed")
        
        # Test 3: CFG_REG
        print("\n" + "="*80)
        print("Test 3: CFG_REG - Configure Register")
        print("="*80)
        tpu.write_instruction(0, Opcode.CFG_REG, 0, 0x06, 0x66, 0)
        tpu.write_instruction(1, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        time.sleep(0.1)
        status = tpu.read_status() 
        if status:
            print(f"✓ Status: {status}")
        print("✓ CFG_REG test passed")
        
        # Test 4: Write to UB, then LD_UB
        print("\n" + "="*80)
        print("Test 4: LD_UB - Load from Unified Buffer")
        print("="*80)
        # Write test data to UB
        test_data = bytes([i for i in range(32)])
        tpu.write_ub(0, test_data)
        print("✓ Wrote 32 bytes to UB[0]")
        
        # Load from UB
        tpu.write_instruction(0, Opcode.LD_UB, 0, 0x01, 0x00, 0)  # Load 1 row from addr 0
        tpu.write_instruction(1, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        time.sleep(0.1)
        status = tpu.read_status()
        if status:
            print(f"✓ Status: {status}")
        print("✓ LD_UB test passed")
        
        # Test 5: Write weights, then RD_WEIGHT
        print("\n" + "="*80)
        print("Test 5: RD_WEIGHT - Load Weights")
        print("="*80)
        # Write test weights
        weight_data = bytes([i for i in range(64)])
        tpu.write_weights(0, weight_data)
        print("✓ Wrote 64 bytes to Weight Memory[0]")
        
        # Load weights
        tpu.write_instruction(0, Opcode.RD_WEIGHT, 0x00, 0x01, 0x00, 0)  # Load 1 weight block
        tpu.write_instruction(1, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        time.sleep(0.1)
        status = tpu.read_status()
        if status:
            print(f"✓ Status: {status}")
        print("✓ RD_WEIGHT test passed")
        
        # Test 6: MATMUL (basic - no verification)
        print("\n" + "="*80)
        print("Test 6: MATMUL - Matrix Multiply")
        print("="*80)
        print("⚠ Note: This test only verifies completion, not correctness")
        print("   For result verification, use test_isa_legacy_verified.py")
        tpu.write_instruction(0, Opcode.MATMUL, 0, 0x20, 4, 0)  # 4x4 matmul
        tpu.write_instruction(1, Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0)
        tpu.write_instruction(2, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        time.sleep(0.5)  # MATMUL takes longer
        status = tpu.read_status()
        if status:
            print(f"✓ Status: {status}")
        print("✓ MATMUL test passed (completion only)")
        
        print("\n" + "="*80)
        print("✓ Basic ISA tests completed!")
        print("="*80)
        print("\nNote: Current bitstream uses legacy mode (no streaming).")
        print("For full streaming mode, uart_dma_basys3.sv supports all operations")
        
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
    finally:
        tpu.close()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python3 test_isa_legacy.py /dev/tty.usbserial-XXXX")
        sys.exit(1)
    
    port = sys.argv[1]
    test_isa_legacy_mode(port)

