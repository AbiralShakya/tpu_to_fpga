# Controller Architecture Refactoring - Files Involved

## Primary Files (Core Implementation)

### 1. **rtl/tpu_controller.sv** ⭐ PRIMARY
- **Status**: Major refactoring required
- **Changes**:
  - Add explicit state machine (IDLE → DISPATCH → EXEC_* → WAIT_DONE)
  - Add `PIPE_LATENCY` parameter
  - Add `op_cycle_cnt` and `sub_step_cnt` counters
  - Hard-code ST_UB sequence (prevent zombie mode)
  - Enforce LD_UB → MATMUL sequence
  - Add handshaking for LD_UB completion
- **Lines affected**: ~967 lines total, major sections:
  - State machine definition (lines 189-199)
  - Control signal generation (lines 531-967)
  - ST_UB state transitions (lines 369-392)

### 2. **rtl/tpu_datapath.sv** ⚠️ MODERATE
- **Status**: Interface changes, handshaking signals
- **Changes**:
  - May need additional handshake signals for LD_UB completion
  - Verify `st_ub_passthrough` timing with new controller
  - Ensure `ub_rd_valid` is properly exposed for controller
- **Lines affected**: 
  - Handshaking signals (lines 37-62)
  - ST_UB passthrough logic (lines 316-330)

### 3. **rtl/systolic_controller.sv** ✅ MINOR
- **Status**: Interface verification
- **Changes**:
  - Verify `weight_load_counter` interface (already exposed)
  - Verify `sys_done` timing matches controller expectations
- **Lines affected**: Minimal - interface already correct

### 4. **rtl/tpu_top.sv** ⚠️ MODERATE
- **Status**: Signal routing, may need new debug signals
- **Changes**:
  - Verify all controller signals properly routed
  - May need new debug outputs for state machine visibility
- **Lines affected**:
  - Controller instantiation (lines ~200-300)
  - Signal routing (lines 68-150)

---

## Secondary Files (Integration & Testing)

### 5. **rtl/uart_dma_basys3.sv** ✅ MINOR
- **Status**: Interface verification
- **Changes**:
  - Verify instruction encoding/decoding still works
  - May need to update if opcode format changes
- **Lines affected**: Minimal - should be compatible

### 6. **rtl/basys3_test_interface.sv** ✅ MINOR
- **Status**: Interface verification
- **Changes**: None expected, but verify controller interface

### 7. **rtl/tpu_top_wrapper.sv** ✅ MINOR
- **Status**: Interface verification
- **Changes**: None expected

---

## Test Files (May Need Updates)

### 8. **sim/test_tpu_controller.v** ⚠️ MODERATE
- **Status**: May need updates for new state machine
- **Changes**:
  - Update state machine tests
  - Add tests for new counters
  - Add tests for ST_UB sequence enforcement

### 9. **sim/tests/test_tpu_controller_isa.py** ⚠️ MODERATE
- **Status**: May need updates for timing changes
- **Changes**:
  - Verify instruction timing still correct
  - Add tests for LD_UB → MATMUL enforcement
  - Add tests for ST_UB sequence completion

### 10. **rtl/tb_tpu_top_functional.sv** ✅ MINOR
- **Status**: Interface verification
- **Changes**: Minimal - should work with new controller

### 11. **rtl/tb_datapath_signals.sv** ✅ MINOR
- **Status**: Interface verification
- **Changes**: Minimal

### 12. **rtl/tb_integration.sv** ✅ MINOR
- **Status**: Interface verification
- **Changes**: Minimal

---

## Documentation Files

### 13. **CONTROLLER_ARCHITECTURE_ANALYSIS.md** ✅ NEW
- **Status**: Created
- **Purpose**: Analysis and refactoring plan

### 14. **CONTROLLER_REFACTORING_FILES.md** ✅ NEW (this file)
- **Status**: Created
- **Purpose**: File inventory

---

## Python Test Files (May Need Updates)

### 15. **python/elite_comprehensive_tpu_test.py** ⚠️ MODERATE
- **Status**: May need timing adjustments
- **Changes**:
  - Verify test timing accounts for new state machine
  - May need to adjust wait times for LD_UB completion
  - Verify ST_UB tests still pass with hard-coded sequence

---

## Summary by Priority

### 🔴 **CRITICAL** (Must Change)
1. **rtl/tpu_controller.sv** - Complete refactoring

### 🟡 **IMPORTANT** (Interface Changes)
2. **rtl/tpu_datapath.sv** - Handshaking signals
3. **rtl/tpu_top.sv** - Signal routing

### 🟢 **VERIFY** (Should Still Work)
4. **rtl/systolic_controller.sv** - Interface check
5. **rtl/uart_dma_basys3.sv** - Interface check
6. **rtl/basys3_test_interface.sv** - Interface check
7. **rtl/tpu_top_wrapper.sv** - Interface check

### 🔵 **TESTING** (May Need Updates)
8. **sim/test_tpu_controller.v** - State machine tests
9. **sim/tests/test_tpu_controller_isa.py** - Timing tests
10. **python/elite_comprehensive_tpu_test.py** - Integration tests

---

## Files NOT Affected

- **rtl/accumulator.sv** - No changes needed
- **rtl/mmu.sv** - No changes needed
- **rtl/pe.sv** - No changes needed
- **rtl/unified_buffer.sv** - No changes needed
- **rtl/dual_weight_fifo.sv** - No changes needed
- **rtl/activation_pipeline.sv** - No changes needed
- **rtl/vpu.sv** - No changes needed

---

## Implementation Order

1. **Phase 1**: Refactor `rtl/tpu_controller.sv` (critical fixes)
2. **Phase 2**: Update `rtl/tpu_datapath.sv` (handshaking)
3. **Phase 3**: Verify `rtl/tpu_top.sv` (routing)
4. **Phase 4**: Update test files (validation)

