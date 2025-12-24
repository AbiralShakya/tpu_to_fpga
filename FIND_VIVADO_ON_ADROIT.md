# Finding Vivado on Princeton Adroit

## Problem
Vivado is not showing up in `module avail`. Let's find it!

## Solution 1: Check if Vivado is Installed Directly

```bash
# Check common installation locations
ls -la /usr/licensed/vivado/
ls -la /opt/Xilinx/
ls -la /tools/Xilinx/
which vivado

# Check if it's in PATH
find /usr -name "vivado" 2>/dev/null | head -5
find /opt -name "vivado" 2>/dev/null | head -5
```

## Solution 2: Check Module System More Carefully

```bash
# Sometimes modules are in subdirectories
module avail vivado
module avail xilinx
module avail fpga

# Check all available modules
module avail 2>&1 | grep -i vivado
module avail 2>&1 | grep -i xilinx
module avail 2>&1 | grep -i fpga
```

## Solution 3: Contact Princeton Research Computing

If Vivado is not installed, you may need to:

1. **Email Princeton Research Computing:**
   - Email: cses@princeton.edu
   - Subject: "Vivado access on Adroit cluster"
   - Ask: "How do I access Vivado on Adroit? It's not showing in module avail"

2. **Check Documentation:**
   - https://researchcomputing.princeton.edu/systems/adroit
   - Look for FPGA/Vivado documentation

## Solution 4: Alternative - Use F4PGA (Open Source)

If Vivado is not available, you can use F4PGA (formerly SymbiFlow) which is open source:

```bash
# F4PGA might be available or you can install it
# This is a free, open-source alternative to Vivado
```

## Solution 5: Check if Vivado Needs License Setup

Sometimes Vivado is installed but needs license configuration:

```bash
# Check for Vivado installation
ls -la /usr/licensed/
ls -la /opt/Xilinx/

# Check environment variables
echo $XILINX_VIVADO
echo $XILINX
```

## Quick Diagnostic Commands

Run these to gather information:

```bash
# 1. Check for Vivado binary
which vivado
find /usr /opt -name "vivado" 2>/dev/null

# 2. Check module system
module avail 2>&1 | grep -i vivado
module avail 2>&1 | grep -i xilinx

# 3. Check common paths
ls -d /usr/licensed/vivado* 2>/dev/null
ls -d /opt/Xilinx/* 2>/dev/null
ls -d /tools/Xilinx/* 2>/dev/null

# 4. Check if it's a symlink or script
file $(which vivado 2>/dev/null) 2>/dev/null
```

## What to Do Next

1. **Run the diagnostic commands above** to see if Vivado exists
2. **If Vivado is found**, you can add it to PATH manually:
   ```bash
   # Example (adjust path based on what you find):
   export PATH=/usr/licensed/vivado/2023.1/bin:$PATH
   export XILINX_VIVADO=/usr/licensed/vivado/2023.1
   vivado -version
   ```

3. **If Vivado is NOT found**, contact Princeton Research Computing for access

## Alternative: Use Your Local Machine

If Vivado is not available on Adroit, you can:

1. **Use Vivado on your Mac** (if you have it installed)
2. **Use Docker with Vivado** (if you have a license)
3. **Use F4PGA** (open source, no license needed)

---

**Run the diagnostic commands first to see what's available!**

