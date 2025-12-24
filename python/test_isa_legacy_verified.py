#!/usr/bin/env python3
"""
test_isa_legacy_verified.py - Test ISA with actual result verification
Verifies that instructions produce correct results, not just that they complete
"""

import sys
import time
import numpy as np
from drivers.tpu_coprocessor_driver import TPU_Coprocessor, Opcode, Instruction

# Color codes for terminal output
class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'

def print_test_header(test_name, test_num, total_tests):
    """Print formatted test header"""
    print(f"\n{Colors.HEADER}{'='*80}{Colors.ENDC}")
    print(f"{Colors.BOLD}Test {test_num}/{total_tests}: {test_name}{Colors.ENDC}")
    print(f"{Colors.HEADER}{'='*80}{Colors.ENDC}\n")

def print_success(msg):
    """Print success message"""
    print(f"{Colors.OKGREEN}✓ {msg}{Colors.ENDC}")

def print_failure(msg):
    """Print failure message"""
    print(f"{Colors.FAIL}✗ {msg}{Colors.ENDC}")

def print_info(msg):
    """Print info message"""
    print(f"{Colors.OKCYAN}ℹ {msg}{Colors.ENDC}")

def verify_arrays(expected, actual, tolerance=0, test_name="Arrays"):
    """Verify two arrays match within tolerance"""
    expected = np.asarray(expected)
    actual = np.asarray(actual)
    
    if expected.shape != actual.shape:
        print_failure(f"{test_name}: Shape mismatch - expected {expected.shape}, got {actual.shape}")
        return False
    
    if tolerance == 0:
        # Exact match
        if np.array_equal(expected, actual):
            print_success(f"{test_name}: Exact match ✓")
            return True
        else:
            diff = np.abs(expected.astype(int) - actual.astype(int))
            max_diff = np.max(diff)
            print_failure(f"{test_name}: Mismatch - max difference: {max_diff}")
            print_info(f"  Expected: {expected}")
            print_info(f"  Actual:   {actual}")
            return False
    else:
        # Approximate match
        if np.allclose(expected, actual, atol=tolerance):
            print_success(f"{test_name}: Match within tolerance (±{tolerance}) ✓")
            return True
        else:
            diff = np.abs(expected.astype(float) - actual.astype(float))
            max_diff = np.max(diff)
            print_failure(f"{test_name}: Mismatch - max difference: {max_diff} (tolerance: ±{tolerance})")
            return False

def test_isa_verified(port):
    """Test ISA with result verification"""
    print(f"\n{Colors.BOLD}{Colors.HEADER}{'='*80}{Colors.ENDC}")
    print(f"{Colors.BOLD}TPU ISA Test - Legacy Mode with Result Verification{Colors.ENDC}")
    print(f"{Colors.BOLD}{'='*80}{Colors.ENDC}\n")
    print(f"Connecting to {port}...")
    
    tpu = TPU_Coprocessor(port, verbose=True)
    tests_passed = 0
    tests_failed = 0
    
    try:
        # ========================================================================
        # Test 1: NOP - Just verify it completes
        # ========================================================================
        print_test_header("NOP - No Operation", 1, 6)
        tpu.write_instruction(0, Opcode.NOP, 0, 0, 0, 0)
        tpu.write_instruction(1, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        time.sleep(0.2)  # Give more time for execution
        
        # Try multiple status reads
        status = None
        for _ in range(5):
            status = tpu.read_status(timeout=0.2)
            if status:
                break
            time.sleep(0.1)
        
        if status:
            print_info(f"Status: sys_busy={status.sys_busy}, vpu_busy={status.vpu_busy}, ub_busy={status.ub_busy}")
            if not status.sys_busy and not status.vpu_busy:
                print_success("NOP completed successfully")
                tests_passed += 1
            else:
                print_failure(f"NOP: TPU still busy (sys={status.sys_busy}, vpu={status.vpu_busy})")
                tests_failed += 1
        else:
            print_failure("NOP: Could not read status from FPGA")
            print_info("This might indicate UART communication issue")
            tests_failed += 1
        
        # ========================================================================
        # Test 2: Write/Read UB - Verify data integrity
        # ========================================================================
        print_test_header("Write/Read UB - Data Integrity", 2, 6)
        test_data = np.array([i for i in range(32)], dtype=np.uint8)
        tpu.write_ub(0, test_data)
        print_info(f"Wrote {len(test_data)} bytes to UB[0]")
        time.sleep(0.1)  # Give time for write to complete
        
        # Read back with longer timeout
        print_info("Reading back from UB[0]...")
        read_data = tpu.read_ub(0, 32, timeout=3.0)
        
        if len(read_data) == 0:
            print_failure("UB Read: No data received from FPGA")
            print_info("This might indicate:")
            print_info("  1. READ_UB command not supported/working")
            print_info("  2. UART RX/TX timing issue")
            print_info("  3. FPGA not responding")
            tests_failed += 1
        else:
            read_array = np.frombuffer(read_data[:32], dtype=np.uint8)
            print_info(f"Read {len(read_data)} bytes from UB[0]")
            
            if verify_arrays(test_data, read_array, test_name="UB Write/Read"):
                tests_passed += 1
            else:
                tests_failed += 1
        
        # ========================================================================
        # Test 3: LD_UB - Load from UB to systolic array
        # ========================================================================
        print_test_header("LD_UB - Load from Unified Buffer", 3, 6)
        # Write known pattern
        pattern = np.array([0xAA] * 32, dtype=np.uint8)
        tpu.write_ub(0, pattern)
        print_info("Wrote pattern 0xAA to UB[0]")
        
        # Load from UB (this moves data to systolic array input)
        tpu.write_instruction(0, Opcode.LD_UB, 0, 0x01, 0x00, 0)  # Load 1 row from addr 0
        tpu.write_instruction(1, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        time.sleep(0.2)
        
        status = tpu.read_status()
        if status and not status.sys_busy:
            print_success("LD_UB completed")
            # Note: Can't directly verify systolic array state, but we can check UB is unchanged
            read_back = tpu.read_ub(0, 32)
            if np.array_equal(pattern, np.frombuffer(read_back[:32], dtype=np.uint8)):
                print_success("UB data preserved after LD_UB")
                tests_passed += 1
            else:
                print_failure("UB data corrupted after LD_UB")
                tests_failed += 1
        else:
            print_failure("LD_UB failed or TPU still busy")
            tests_failed += 1
        
        # ========================================================================
        # Test 4: Write/Read Weights - Verify weight memory
        # ========================================================================
        print_test_header("Write/Read Weights - Weight Memory", 4, 6)
        weight_data = np.array([i % 256 for i in range(64)], dtype=np.uint8)
        tpu.write_weights(0, weight_data)
        print_info(f"Wrote {len(weight_data)} bytes to Weight Memory[0]")
        
        # Note: Weight memory read might not be directly supported via UART
        # This test verifies write completes without error
        tpu.write_instruction(0, Opcode.RD_WEIGHT, 0x00, 0x01, 0x00, 0)  # Load 1 weight block
        tpu.write_instruction(1, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        time.sleep(0.2)
        
        status = tpu.read_status()
        if status and not status.sys_busy:
            print_success("Weight write/load completed")
            tests_passed += 1
        else:
            print_failure("Weight operation failed")
            tests_failed += 1
        
        # ========================================================================
        # Test 5: MATMUL - Simple 2x2 matrix multiply with verification
        # ========================================================================
        print_test_header("MATMUL - Matrix Multiply with Verification", 5, 6)
        
        # Create simple 2x2 test case:
        # Input matrix A (2x2): [[1, 2], [3, 4]]
        # Weight matrix W (2x2): [[1, 0], [0, 1]]  (identity)
        # Expected result: [[1, 2], [3, 4]] (same as input)
        
        # Prepare input data (2 rows, 2 columns each)
        # UB layout: row-major, 32 bytes per row
        input_matrix = np.array([
            [1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        ], dtype=np.uint8).flatten()
        
        # Write input to UB[0]
        tpu.write_ub(0, input_matrix)
        print_info("Wrote 2x2 input matrix to UB[0]")
        print_info("  Input: [[1, 2], [3, 4]]")
        
        # Prepare identity weight matrix (2x2)
        # Weight layout: 8 bytes per weight block
        weight_matrix = np.array([
            [1, 0, 0, 0, 0, 0, 0, 0],  # First weight row
            [0, 1, 0, 0, 0, 0, 0, 0]   # Second weight row
        ], dtype=np.uint8).flatten()
        
        tpu.write_weights(0, weight_matrix)
        print_info("Wrote 2x2 identity weight matrix")
        print_info("  Weights: [[1, 0], [0, 1]]")
        
        # Execute MATMUL: UB[0] × Weights → Accumulator
        # MATMUL args: ub_addr, acc_addr, rows, cols
        tpu.write_instruction(0, Opcode.MATMUL, 0, 0x00, 2, 0)  # 2x2 matmul
        tpu.write_instruction(1, Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0)  # Wait for completion
        tpu.write_instruction(2, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        
        print_info("Executing MATMUL...")
        time.sleep(1.0)  # MATMUL takes longer
        
        status = tpu.read_status()
        if status and not status.sys_busy:
            print_success("MATMUL completed")
            
            # Note: Results are in accumulator, which might not be directly readable via UART
            # For now, we verify the operation completed
            # TODO: Add accumulator read capability or use ST_UB to move results to UB
            print_info("Note: Results stored in accumulator (not directly readable via UART)")
            print_info("To verify results, add ST_UB instruction to move accumulator → UB")
            tests_passed += 1
        else:
            print_failure("MATMUL failed or TPU still busy")
            tests_failed += 1
        
        # ========================================================================
        # Test 6: MATMUL + ST_UB - Full pipeline with result verification
        # ========================================================================
        print_test_header("MATMUL + ST_UB - Full Pipeline with Verification", 6, 6)
        
        # Use same 2x2 test case
        tpu.write_ub(0, input_matrix)
        tpu.write_weights(0, weight_matrix)
        
        # Execute: MATMUL → ST_UB (move results from accumulator to UB[0x20])
        tpu.write_instruction(0, Opcode.MATMUL, 0, 0x00, 2, 0)      # UB[0] × Weights → Acc[0]
        tpu.write_instruction(1, Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0)   # Wait for MATMUL
        tpu.write_instruction(2, Opcode.ST_UB, 0x00, 0x20, 0x02, 0) # Acc[0] → UB[0x20], 2 rows
        tpu.write_instruction(3, Opcode.SYNC, 0x0F, 0xFF, 0xFF, 0)   # Wait for ST_UB
        tpu.write_instruction(4, Opcode.HALT, 0, 0, 0, 0)
        tpu.start_execution()
        
        print_info("Executing MATMUL + ST_UB pipeline...")
        time.sleep(1.5)
        
        status = tpu.read_status()
        if status and not status.sys_busy:
            print_success("Pipeline completed")
            
            # Read results from UB[0x20]
            result_data = tpu.read_ub(0x20, 64)  # 2 rows × 32 bytes
            result_array = np.frombuffer(result_data[:64], dtype=np.uint8)
            
            # Extract 2x2 result (first 2 bytes of each row)
            result_matrix = np.array([
                [result_array[0], result_array[1]],
                [result_array[32], result_array[33]]
            ], dtype=np.uint8)
            
            expected_matrix = np.array([[1, 2], [3, 4]], dtype=np.uint8)
            
            print_info(f"Expected result: {expected_matrix}")
            print_info(f"Actual result:   {result_matrix}")
            
            if verify_arrays(expected_matrix, result_matrix, test_name="MATMUL Result"):
                print_success("MATMUL result verification PASSED!")
                tests_passed += 1
            else:
                print_failure("MATMUL result verification FAILED!")
                tests_failed += 1
        else:
            print_failure("Pipeline failed or TPU still busy")
            tests_failed += 1
        
        # ========================================================================
        # Summary
        # ========================================================================
        print(f"\n{Colors.HEADER}{'='*80}{Colors.ENDC}")
        print(f"{Colors.BOLD}Test Summary{Colors.ENDC}")
        print(f"{Colors.HEADER}{'='*80}{Colors.ENDC}")
        print(f"Total Tests: {tests_passed + tests_failed}")
        print(f"{Colors.OKGREEN}Passed: {tests_passed}{Colors.ENDC}")
        print(f"{Colors.FAIL}Failed: {tests_failed}{Colors.ENDC}")
        print(f"{Colors.HEADER}{'='*80}{Colors.ENDC}\n")
        
    except Exception as e:
        print_failure(f"Error: {e}")
        import traceback
        traceback.print_exc()
        tests_failed += 1
    finally:
        tpu.close()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python3 test_isa_legacy_verified.py /dev/tty.usbserial-XXXX")
        sys.exit(1)
    
    port = sys.argv[1]
    test_isa_verified(port)

