# Path Fixes Summary

## Issue
Build scripts had hardcoded absolute paths that would fail if:
- The project was moved to a different location
- The script was run from a different directory
- The user's home directory was different

## Fixed Scripts

### 1. `scripts/build_tpu_bitstream.sh`
- **Before:** `cd /Users/abiralshakya/Documents/tpu_to_fpga`
- **After:** Uses `$PROJECT_ROOT` variable derived from script location
- **Also fixed:** Chipdb path now checks multiple locations and supports `$CHIPDB_PATH` env var

### 2. `vivado/docker_build.sh`
- **Before:** Hardcoded `/Users/abiralshakya/Downloads/diffTPU` path
- **After:** Dynamically finds chipdb file and mounts it correctly

### 3. `build_uart_test_bitstream.sh`
- **Before:** Assumed `tpu_to_fpga` subdirectory existed
- **After:** Uses script location to find project root and source files

### 4. `scripts/test_full_tpu.sh`
- **Before:** All test commands used `cd /Users/abiralshakya/Documents/tpu_to_fpga`
- **After:** Uses `$PROJECT_ROOT` variable for all paths

### 5. `scripts/test_all.sh`
- **Before:** Hardcoded path to sim directory
- **After:** Uses `$PROJECT_ROOT` variable

### 6. `scripts/upload_to_adroit.sh`
- **Before:** `LOCAL_DIR="/Users/abiralshakya/Documents/tpu_to_fpga"`
- **After:** Dynamically calculates from script location

### 7. `scripts/setup_env.sh`
- **Before:** Hardcoded path in instructions
- **After:** Uses `$PROJECT_ROOT` variable

## Pattern Used

All scripts now use this pattern at the top:

```bash
# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
```

Then use `$PROJECT_ROOT` instead of hardcoded paths.

## Benefits

1. **Portable:** Scripts work regardless of where the project is located
2. **Reliable:** No more "No such file or directory" errors from wrong paths
3. **Maintainable:** One pattern used consistently across all scripts

## Remaining Hardcoded Paths

Some documentation files still reference `/Users/abiralshakya/Documents/tpu_to_fpga` but these are just examples and don't affect functionality.

## Testing

To verify fixes work:
```bash
# From any directory
cd /tmp
/path/to/tpu_to_fpga/scripts/build_tpu_bitstream.sh

# Should work without path errors
```

