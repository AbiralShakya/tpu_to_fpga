# Accumulator Architecture Analysis

## Summary

The changes to `accumulator.sv` affect **only the main TPU design**. The other accumulator modules (`accumulator_mem.sv` and `accumulator_align.sv`) are either:
- Used in a **different design** (`mlp_top.sv`)
- **Dead code** (not used anywhere)

## Module Usage Map

### ✅ `accumulator.sv` - **MAIN TPU DESIGN** (Changed to 96-bit)

**Used in:**
- `rtl/tpu_datapath.sv` (line 258) → Main TPU datapath
- `rtl/tpu_top.sv` → Top-level TPU module

**Architecture:**
- 96-bit wide-word BRAM storage
- Stores all 3 columns per address: `{Col2[31:0], Col1[31:0], Col0[31:0]}`
- 256 rows × 96 bits = 3 KiB per buffer, 6 KiB total
- Addressable memory with read/write ports
- Used for storing matrix multiplication results

**Changes Made:**
- ✅ Updated to 96-bit data width
- ✅ Simplified buffer selection (removed safety gate)
- ✅ Updated all integration points in `tpu_datapath.sv`
- ✅ Updated `systolic_controller.sv` to write all 3 columns at once
- ✅ Updated `tpu_controller.sv` ST_UB to read all 3 columns at once

---

### ⚠️ `accumulator_mem.sv` - **MLP DESIGN** (Separate, Unchanged)

**Used in:**
- `rtl/mlp_top.sv` (line 186) → MLP/neural network testing module

**Architecture:**
- Simple 3-value storage (one per column)
- Double-buffered with accumulate/overwrite modes
- **NOT** addressable memory (stores single set of 3 values)
- Different interface: `enable`, `accumulator_mode`, `buffer_select`
- Used for MLP layer testing, not main TPU

**Status:**
- ✅ **No changes needed** - This is a separate design
- ✅ **Not affected** by `accumulator.sv` changes
- ✅ **Still functional** for `mlp_top.sv` use case

---

### ❌ `accumulator_align.sv` - **DEAD CODE** (Not Used)

**Used in:**
- **NOWHERE** - No instantiations found in codebase

**Purpose:**
- Was designed to align staggered column outputs
- For 3x3 systolic array column timing alignment

**Status:**
- ❌ **Dead code** - Can be safely removed
- ❌ **Not connected** to any design
- ⚠️ **Recommendation:** Delete this file

---

## Impact Analysis

### Files Affected by `accumulator.sv` Changes

1. ✅ **`rtl/accumulator.sv`** - Changed to 96-bit
2. ✅ **`rtl/tpu_datapath.sv`** - Updated to pack/unpack 96-bit data
3. ✅ **`rtl/systolic_controller.sv`** - Updated to write all 3 columns at once
4. ✅ **`rtl/tpu_controller.sv`** - Updated ST_UB to read all 3 columns at once

### Files NOT Affected

1. ✅ **`rtl/mlp_top.sv`** - Uses `accumulator_mem.sv` (separate design)
2. ✅ **`rtl/accumulator_mem.sv`** - Separate module, unchanged
3. ✅ **`rtl/accumulator_align.sv`** - Dead code, not used

---

## Verification Checklist

- [x] `accumulator.sv` updated to 96-bit wide-word architecture
- [x] `tpu_datapath.sv` updated to pack/unpack 96-bit data
- [x] `systolic_controller.sv` writes all 3 columns at once
- [x] `tpu_controller.sv` ST_UB reads all 3 columns at once
- [x] All integration points verified
- [x] No linter errors
- [ ] **TODO:** Delete `accumulator_align.sv` (dead code)

---

## Recommendations

1. **Delete Dead Code:**
   ```bash
   rm rtl/accumulator_align.sv
   ```

2. **Document Separation:**
   - Consider adding comments to `accumulator_mem.sv` indicating it's for MLP design only
   - Consider adding comments to `accumulator.sv` indicating it's for main TPU design

3. **Future Cleanup:**
   - If `mlp_top.sv` is not actively used, consider moving it to a separate directory
   - Or document that it's a separate test/example design

---

## Conclusion

✅ **All changes are properly propagated** - The 96-bit accumulator changes only affect the main TPU design, which has been fully updated.

✅ **No dead code in active design** - `accumulator_align.sv` is unused but doesn't affect functionality.

✅ **Separate designs remain functional** - `accumulator_mem.sv` in `mlp_top.sv` is unaffected.

