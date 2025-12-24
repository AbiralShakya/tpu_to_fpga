# FPGA Programming and Runtime Communication Flow

## Complete System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         YOUR COMPUTER                           │
│                                                                 │
│  ┌──────────────┐         ┌──────────────┐                    │
│  │   Vivado /   │         │   Python     │                    │
│  │ openFPGALoader│         │   Driver     │                    │
│  │              │         │ (tpu_driver) │                    │
│  └──────┬───────┘         └──────┬───────┘                    │
│         │                        │                             │
│         │ JTAG Protocol          │ UART Protocol               │
│         │ (Programming)          │ (Runtime Communication)     │
└─────────┼────────────────────────┼─────────────────────────────┘
          │                        │
          │ USB Cable              │ USB Cable
          │                        │
          ▼                        ▼
┌─────────────────────────────────────────────────────────────────┐
│                      BASYS 3 BOARD                             │
│                                                                 │
│  ┌──────────────┐         ┌──────────────┐                    │
│  │  USB-UART    │         │  USB-UART    │                    │
│  │   Bridge     │         │   Bridge     │                    │
│  │  (FTDI Chip) │         │  (FTDI Chip) │                    │
│  └──────┬───────┘         └──────┬───────┘                    │
│         │                        │                             │
│         │ JTAG                   │ UART                         │
│         │                        │                             │
└─────────┼────────────────────────┼─────────────────────────────┘
          │                        │
          │                        │
          ▼                        ▼
┌─────────────────────────────────────────────────────────────────┐
│                    ARTIX-7 FPGA                                 │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐ │
│  │         Configuration Memory (SRAM)                      │ │
│  │  ┌──────────────────────────────────────────────────┐  │ │
│  │  │  Your TPU Design (loaded from .bit file)         │  │ │
│  │  │                                                    │  │ │
│  │  │  ┌──────────────┐    ┌──────────────┐            │  │ │
│  │  │  │   UART RX    │───▶│  UART DMA    │            │  │ │
│  │  │  │   Module     │    │  Controller  │            │  │ │
│  │  │  └──────────────┘    └──────┬───────┘            │  │ │
│  │  │                             │                     │  │ │
│  │  │                    ┌────────▼────────┐           │  │ │
│  │  │                    │  TPU Controller │           │  │ │
│  │  │                    │  (Executes ISA) │           │  │ │
│  │  │                    └─────────────────┘           │  │ │
│  │  └──────────────────────────────────────────────────┘  │ │
│  └──────────────────────────────────────────────────────────┘ │
│                                                                 │
│  Pin A18: uart_rx (receives from PC)                           │
│  Pin B18: uart_tx (sends to PC)                               │
└─────────────────────────────────────────────────────────────────┘
```

## Two Separate Processes

### Process 1: FPGA Programming (One-Time)

```
┌─────────────┐
│  Build .bit  │
│   file       │
└──────┬───────┘
       │
       ▼
┌─────────────┐      JTAG      ┌─────────────┐
│  Vivado /   │───────────────▶│   FPGA      │
│ openFPGALoader│   Protocol    │ Configuration│
└─────────────┘                │   Memory    │
                                └─────────────┘
                                
Time: ~2 seconds
Frequency: Once per design change
Result: Your TPU design is now running on FPGA
```

### Process 2: Runtime Communication (Continuous)

```
┌─────────────┐
│  Python      │
│  Driver      │
└──────┬───────┘
       │
       │ UART Protocol
       │ (115200 baud)
       ▼
┌─────────────┐      UART      ┌─────────────┐
│  USB-UART   │───────────────▶│  Your TPU  │
│   Bridge     │   (Pin A18)    │  Design    │
└─────────────┘                │  (Running) │
                                └──────┬──────┘
                                       │
                                       │ Execute
                                       ▼
                                ┌─────────────┐
                                │  Results    │
                                │  (via UART) │
                                └─────────────┘
                                
Time: Continuous
Frequency: As needed
Result: Instructions executed, data transferred
```

## Step-by-Step: First Time Setup

### 1. Write Your Design
```systemverilog
// rtl/tpu_top.sv
module tpu_top (
    input  logic uart_rx,  // Pin A18
    output logic uart_tx   // Pin B18
);
    // Your TPU design
endmodule
```

### 2. Build Bitstream
```bash
vivado -mode batch -source build.tcl
# Creates: build/tpu_basys3.bit
```

### 3. Program FPGA
```bash
openFPGALoader -b basys3 build/tpu_basys3.bit
# FPGA now running your design!
```

### 4. Communicate via UART
```python
# FPGA is already running - now use UART
import serial
ser = serial.Serial('/dev/ttyUSB0', 115200)
ser.write([0x03, 0x00, 0x00, ...])  # Send instruction
```

## What Happens Inside FPGA

```
┌─────────────────────────────────────────────────────────┐
│  FPGA Running Your TPU Design                          │
│                                                          │
│  ┌──────────┐                                           │
│  │ uart_rx  │───▶ UART RX Module                       │
│  │ (Pin A18)│    (uart_rx.sv)                          │
│  └──────────┘                                           │
│       │                                                  │
│       ▼                                                  │
│  ┌──────────┐                                           │
│  │ UART DMA │───▶ Instruction Buffer                   │
│  │ Controller│    (uart_dma_basys3.sv)                 │
│  └──────────┘                                           │
│       │                                                  │
│       ▼                                                  │
│  ┌──────────┐                                           │
│  │ TPU      │───▶ Execute Instructions                 │
│  │ Controller│    (tpu_controller.sv)                  │
│  └──────────┘                                           │
│       │                                                  │
│       ▼                                                  │
│  ┌──────────┐                                           │
│  │ Results  │───▶ UART TX Module                       │
│  │          │    (uart_tx.sv)                          │
│  └──────────┘                                           │
│       │                                                  │
│       ▼                                                  │
│  ┌──────────┐                                           │
│  │ uart_tx  │───▶ Back to PC                            │
│  │ (Pin B18)│                                           │
│  └──────────┘                                           │
└─────────────────────────────────────────────────────────┘
```

## Key Points

1. **FPGA Programming** = Load design (JTAG) - **ONE TIME**
2. **UART Communication** = Runtime data (UART) - **MANY TIMES**
3. **Same USB cable** used for both, but different protocols
4. **UART modules** are part of your design running on FPGA
5. **After programming**, FPGA runs your design continuously

## Common Confusion

**❌ WRONG:** "I'll send the bitstream via UART to program FPGA"
- Artix-7 doesn't support UART configuration
- UART is for application communication, not programming

**✅ CORRECT:** "I'll program FPGA via JTAG, then communicate via UART"
- Program once via JTAG (Vivado/openFPGALoader)
- Then use UART for runtime communication (Python driver)

