# Adroit Quick Start - Visual Vivado Setup

## Quick Checklist

### On Your Mac (Local)

- [ ] **Install XQuartz** (for X11 forwarding)
  - Download: https://www.xquartz.org/
  - Restart terminal after installation

- [ ] **Upload files to Adroit**
  ```bash
  ./upload_to_adroit.sh
  # OR manually:
  scp -r rtl/ tpu_constraints.xdc as0714@adroit.princeton.edu:/scratch/network/as0714/tpu_to_fpga/
  ```

### On Adroit (Remote)

- [ ] **SSH with X11 forwarding**
  ```bash
  ssh -X as0714@adroit.princeton.edu
  # OR trusted:
  ssh -Y as0714@adroit.princeton.edu
  ```

- [ ] **Extract files**
  ```bash
  cd /scratch/network/as0714/tpu_to_fpga
  tar -xzf tpu_vivado_project.tar.gz
  ls rtl/*.sv  # Verify files
  ```

- [ ] **Load Vivado**
  ```bash
  module load vivado/2023.1
  vivado -version  # Verify
  ```

- [ ] **Launch Vivado GUI**
  ```bash
  cd /scratch/network/as0714/tpu_to_fpga
  vivado &
  # Wait 30-60 seconds for GUI to open
  ```

### In Vivado GUI

- [ ] **Create Project**
  - File → New Project
  - Name: `tpu_basys3`
  - Location: `/scratch/network/as0714/tpu_to_fpga`
  - Type: RTL Project
  - Board: Basys 3

- [ ] **Add Source Files**
  - Right-click "Design Sources" → Add Sources
  - Add ALL `.sv` files from `rtl/` directory
  - Set `tpu_top.sv` as Top Module

- [ ] **Add Constraints**
  - Right-click "Constraints" → Add Sources
  - Add `tpu_constraints.xdc`

- [ ] **Run Synthesis**
  - Flow Navigator → Synthesis → Run Synthesis
  - Wait 5-10 minutes

- [ ] **Run Implementation**
  - After synthesis completes, click "Run Implementation"
  - Wait 10-20 minutes

- [ ] **Generate Bitstream**
  - After implementation completes, click "Generate Bitstream"
  - Wait 1-2 minutes

### Download to Mac

- [ ] **Download bitstream**
  ```bash
  # From Mac terminal (new window)
  scp as0714@adroit.princeton.edu:/scratch/network/as0714/tpu_to_fpga/tpu_basys3/tpu_basys3.runs/impl_1/tpu_top.bit \
      /Users/abiralshakya/Documents/tpu_to_fpga/build/
  ```

## File Locations

### On Adroit
```
/scratch/network/as0714/tpu_to_fpga/
├── rtl/                    # All .sv files
├── tpu_constraints.xdc     # Constraints
└── tpu_basys3/             # Vivado project (created by Vivado)
    └── tpu_basys3.runs/
        └── impl_1/
            └── tpu_top.bit  # ← Your bitstream!
```

### On Mac
```
/Users/abiralshakya/Documents/tpu_to_fpga/
└── build/
    └── tpu_top.bit  # ← Downloaded bitstream
```

## Common Issues

| Problem | Solution |
|---------|----------|
| Vivado GUI doesn't open | Install XQuartz, use `ssh -Y` |
| "module: command not found" | Run: `source /etc/profile.d/modules.sh` |
| Synthesis fails | Check Messages panel, add missing files |
| Timing violations | Reduce clock frequency in constraints |
| Resource overflow | Design too large, reduce complexity |

## Time Estimates

- Upload files: **1-2 minutes**
- Extract on Adroit: **30 seconds**
- Synthesis: **5-10 minutes**
- Implementation: **10-20 minutes**
- Bitstream: **1-2 minutes**
- Download: **30 seconds**

**Total: ~20-35 minutes**

## Need Help?

See full guide: `VISUAL_VIVADO_ADROIT_GUIDE.md`

