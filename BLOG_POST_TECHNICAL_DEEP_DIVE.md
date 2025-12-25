# Chewing on Chips

**Day 25** 🎄

**The Control Plane: ISA Orthogonality, Asynchronous Signaling, and EDA Hybridization**

**Alan Ma and Abiral Shakya** | Dec 25, 2025

---

Welcome back to **Unwrapping TPUs** ✨

Been a minute… Merry Christmas!! We've got a packed agenda to uncover… Since we last talked, we've pushed the needle and hacked on three core topics: our inference demo with the FPGA acting as a coprocessor; our open-source development flow utilizing Yosys Synthesis and the trade-offs between nextpnr-xilinx and Vivado; and the physical implementation challenges that forced us to hybrid flows.

To transform a collection of Processing Elements (PEs) into a programmable accelerator, we must bridge the gap between high-level tensor operations and gate-level execution. Today, we explore the **gate-level design** of our custom Instruction Set Architecture (ISA), the **circuit-level physics** of UART-based command protocols, the **architectural decisions** behind our communication stack, and the **synthesis-to-bitstream** flow using open-source and proprietary EDA tools.

---

## Part I: The FPGA Coprocessor Model — Why UART on Basys3?

### The Basys3 Constraint: No PCIe, Only UART

The **Digilent Basys3** board uses the Xilinx Artix-7 XC7A35T-1CPG236C FPGA. This is a **low-cost development board** ($149) designed for educational use, which means it makes critical I/O trade-offs:

**Available I/O on Basys3:**
- ✅ **UART via FT2232HQ** (USB-to-serial bridge @ 115200 baud)
- ✅ **16 slide switches** (GPIO for test inputs)
- ✅ **5 push buttons** (manual control)
- ✅ **16 LEDs + 4-digit 7-segment display** (status output)
- ❌ **No PCIe connector** (would require Kintex-7 or Zynq)
- ❌ **No Ethernet PHY** (would need external PMOD)
- ❌ **No high-speed SERDES** (GTP transceivers not routed)

**Why UART is the ONLY option:**

For an FPGA to act as a coprocessor, it needs a bidirectional communication link to the host PC. On the Basys3, the **FT2232HQ** USB-UART bridge is the only practical interface:

1. **FTDI FT2232HQ Dual-Channel Bridge:**
   - Channel A: JTAG programming (used by Vivado)
   - Channel B: UART (connected to FPGA pins B18=RX, A18=TX)
   - USB 2.0 Hi-Speed (480 Mbps theoretical, but UART limited to 3 Mbaud max)

2. **Baud Rate Selection:**
   - We chose **115200 baud** for reliability
   - Effective throughput: 115200 bits/sec ÷ 10 bits/byte = **11,520 bytes/sec** ≈ **11.25 KB/s**
   - Higher rates (230400, 460800) are possible but require careful clock calibration

3. **Physical Layer Constraints:**
   - UART is **asynchronous** (no shared clock between host and FPGA)
   - Uses **start/stop bits** for framing (overhead = 20%)
   - **Half-duplex in practice** (though theoretically full-duplex)

**Architectural Consequence:** UART becomes the **I/O bottleneck**. While the systolic array can compute **900 MMAC/s**, we can only transfer **11.25 KB/s**. This 80,000× mismatch means the TPU spends 99.99% of its time idle, waiting for data.

**Solution:** Treat FPGA as a **coprocessor**, not a streaming engine. The host uploads instructions + data in bulk, then the FPGA runs autonomously, only sending back results.

---

## Part II: UART Communication Stack — From Transistors to Python

### Layer 1: Physical UART — The Circuit-Level View

**UART Standard (RS-232 logic levels):**
- **Idle state:** Logic HIGH (3.3V on FPGA, pulled up by resistor)
- **Start bit:** Logic LOW (0V) for 1 bit period
- **Data bits:** LSB-first, 8 bits (D0-D7)
- **Stop bit:** Logic HIGH for 1 bit period
- **No clock line** (asynchronous)

**Timing Diagram (sending 0xC5 = 0b11000101):**

```
       IDLE    START  D0  D1  D2  D3  D4  D5  D6  D7  STOP  IDLE
        │       │     │   │   │   │   │   │   │   │   │     │
  ──────┘       └─────┘   └───┘   └───┘   └───┘   └───┘     ├────
  3.3V  │       │  1  │ 0 │ 1 │ 0 │ 0 │ 1 │ 0 │ 1 │ 1 │     │ 3.3V
        │       │  b  │   │   │   │   │   │   │   │   │     │
        │       │  i  │   │   │   │   │   │   │   │   │     │
        │       │  t  │   │   │   │   │   │   │   │   │     │
        ├───────┼─────┼───┼───┼───┼───┼───┼───┼───┼───┼─────┤
        0      868  1736 2604    ...                    8680  clks
               μs   μs   μs                              μs
              (start) (bit 0)                          (stop)

  Each bit: 100 MHz / 115200 = 868 clock cycles = 8.68 μs
  Total frame: 10 bits × 8.68 μs = 86.8 μs per byte
```

**Critical Timing Challenge:** The UART RX signal arrives **asynchronous** to the 100 MHz system clock. Direct sampling would violate setup and hold times, causing **metastability**.

**Metastability Explained:**

A flip-flop has a finite **aperture time** (tₐₚ ≈ 100 ps for 7-series FPGAs) during which the input must be stable. If a signal transitions within this window:

```
       ┌──────────────────────────────────────────────────────┐
       │  Setup Time    │ Aperture │  Hold Time               │
       │     (tsu)      │  Window  │    (th)                 │
       │                │   (tap)  │                          │
  ─────┴────────────────┼──────────┼──────────────────────────┴─────
                        ↑          ↑
                     Clock Edge   If D transitions here,
                                  Q may oscillate!
```

The output Q can:
1. **Oscillate** at the transistor's natural frequency (GHz range)
2. **Settle to metastable voltage** (between VIL and VIH)
3. **Eventually resolve** to 0 or 1 (but unpredictable timing)

**Mean Time Between Failures (MTBF):**

```
MTBF = e^(tr/τ) / (f_clk × f_data × tap)

Where:
  tr = resolution time (time for Q to settle)
  τ = flip-flop time constant (≈20 ps for 7-series)
  f_clk = clock frequency (100 MHz)
  f_data = data toggle rate (115200 Hz)
  tap = aperture window (≈100 ps)

For 1 synchronizer stage:
  MTBF ≈ 0.3 seconds (UNACCEPTABLE!)

For 2 synchronizer stages (tr = 2 clock periods = 20 ns):
  MTBF ≈ 10^43 years (safe)
```

**Our RTL Solution (uart_rx.sv, lines 50-55):**

```systemverilog
// 2-stage synchronizer to prevent metastability
reg rx_sync1, rx_sync2;

always @(posedge clk) begin
    rx_sync1 <= rx;      // First stage: may go metastable
    rx_sync2 <= rx_sync1; // Second stage: stable output
end

// Use rx_sync2 for all downstream logic
```

**Why This Works:**
- If `rx_sync1` goes metastable, it has **1 full clock period** (10 ns) to resolve before `rx_sync2` samples it.
- With τ = 20 ps and tr = 10 ns, the probability of `rx_sync2` going metastable is ≈ e^(-10ns/20ps) ≈ 10^-217.

**Mid-Bit Sampling for Clock Recovery:**

Since UART has no shared clock, the receiver must **recover the bit clock** from the start bit edge:

```systemverilog
localparam CLKS_PER_BIT = 868;  // 100 MHz / 115200

reg [9:0] clk_count;

// Detect start bit (falling edge on idle line)
if (state == IDLE && rx_sync2 == 1'b0) begin
    state <= START;
    clk_count <= 10'd0;
end

// Wait to mid-bit for sampling
if (state == START) begin
    if (clk_count == (CLKS_PER_BIT / 2)) begin  // 434 clocks
        if (rx_sync2 == 1'b0) begin  // Verify start bit
            state <= DATA;
            clk_count <= 10'd0;
        end else begin
            state <= IDLE;  // False start (glitch)
        end
    end else begin
        clk_count <= clk_count + 1;
    end
end

// Sample data bits at mid-bit
if (state == DATA) begin
    if (clk_count == CLKS_PER_BIT - 1) begin  // 868 clocks
        rx_shift[bit_index] <= rx_sync2;  // Sample!
        bit_index <= bit_index + 1;
        clk_count <= 10'd0;
    end else begin
        clk_count <= clk_count + 1;
    end
end
```

**Why Mid-Bit?** Sampling at the **center of the bit cell** (434 clocks after start edge) maximizes tolerance to:
- **Baud rate mismatch** (±5% is typical for UART)
- **Clock jitter** (±1% for crystal oscillators)
- **Propagation delay** in FT2232HQ and PCB traces

**Timing Margin Calculation:**

```
Bit period = 8.68 μs
Sampling occurs at: 8.68 μs / 2 = 4.34 μs
Tolerance = ±(8.68 μs / 2) = ±4.34 μs

With 5% baud mismatch:
  Error = 8.68 μs × 0.05 = 0.434 μs
  Margin = 4.34 μs - 0.434 μs = 3.91 μs (90% margin)
```

---

### Layer 2: Protocol Layer — Command State Machine

**Design Goal:** Implement a simple packet-based protocol for:
1. Writing to Unified Buffer (bulk data transfer)
2. Writing instructions (program the TPU)
3. Starting execution
4. Reading status
5. Reading results

**Packet Format (Variable Length):**

```
┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────────┐
│  CMD     │ ADDR_HI  │ ADDR_LO  │  LEN_HI  │  LEN_LO  │   PAYLOAD    │
│ (1 byte) │ (1 byte) │ (1 byte) │ (1 byte) │ (1 byte) │  (N bytes)   │
└──────────┴──────────┴──────────┴──────────┴──────────┴──────────────┘
    ↓          ↓          ↓          ↓          ↓            ↓
  Opcode    Address (16-bit)     Length (16-bit)        Data
```

**Command Opcodes:**

| CMD | Name | Payload | Response | RTL State |
|-----|------|---------|----------|-----------|
| 0x01 | WRITE_UB | N bytes (32-byte chunks) | None | WRITE_UB |
| 0x02 | WRITE_WT | N bytes (8-byte chunks) | None | WRITE_WT |
| 0x03 | WRITE_INSTR | 4 bytes | None | WRITE_INSTR |
| 0x04 | READ_UB | 0 bytes | 32 bytes | READ_UB |
| 0x05 | EXECUTE | 0 bytes | None | EXECUTE |
| 0x06 | STATUS | 0 bytes | 1 byte | SEND_STATUS |

**State Machine Implementation (uart_dma_basys3.sv):**

```systemverilog
// State definitions
localparam IDLE           = 8'd0;
localparam READ_ADDR_HI   = 8'd2;
localparam READ_ADDR_LO   = 8'd3;
localparam READ_LENGTH_HI = 8'd4;
localparam READ_LENGTH_LO = 8'd5;
localparam WRITE_INSTR    = 8'd8;
localparam EXECUTE        = 8'd11;

reg [7:0] state;
reg [7:0] command;
reg [15:0] addr;
reg [15:0] length;
reg [15:0] byte_count;
reg [31:0] instr_buffer;  // Accumulator for 4-byte instruction

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= IDLE;
        instr_wr_en <= 1'b0;
        start_execution <= 1'b0;
    end else begin
        // Default: de-assert control signals
        instr_wr_en <= 1'b0;
        start_execution <= 1'b0;

        case (state)
            // =============================================================
            // IDLE: Wait for command byte
            // =============================================================
            IDLE: begin
                if (rx_valid) begin  // rx_valid = pulse from uart_rx
                    command <= rx_data;
                    byte_count <= 16'd0;

                    case (rx_data)
                        8'h03: state <= READ_ADDR_HI;  // WRITE_INSTR
                        8'h05: state <= EXECUTE;       // EXECUTE
                        8'h06: state <= SEND_STATUS;   // STATUS
                        default: state <= READ_ADDR_HI; // Others need addr
                    endcase
                end
            end

            // =============================================================
            // READ_ADDR_HI/LO: Parse 16-bit address
            // =============================================================
            READ_ADDR_HI: begin
                if (rx_valid) begin
                    addr[15:8] <= rx_data;
                    state <= READ_ADDR_LO;
                end
            end

            READ_ADDR_LO: begin
                if (rx_valid) begin
                    addr[7:0] <= rx_data;
                    // Next state depends on command
                    state <= (command == 8'h03) ? WRITE_INSTR : READ_LENGTH_HI;
                end
            end

            // =============================================================
            // WRITE_INSTR: Accumulate 4 bytes, then write
            // =============================================================
            WRITE_INSTR: begin
                if (rx_valid) begin
                    // Shift in byte (LSB first in packet, but we want MSB first in memory)
                    instr_buffer <= {instr_buffer[23:0], rx_data};
                    byte_count <= byte_count + 1;

                    if (byte_count == 16'd3) begin  // 4th byte received
                        // Write complete instruction to memory
                        instr_wr_en <= 1'b1;
                        instr_wr_addr <= addr[4:0];  // 32 entries (5-bit address)
                        instr_wr_data <= {instr_buffer[23:0], rx_data};

                        state <= IDLE;
                        byte_count <= 16'd0;
                    end
                end
            end

            // =============================================================
            // EXECUTE: Pulse start signal to controller
            // =============================================================
            EXECUTE: begin
                start_execution <= 1'b1;  // 1-cycle pulse
                state <= IDLE;
            end

            // =============================================================
            // SEND_STATUS: Read hardware status, send via UART TX
            // =============================================================
            SEND_STATUS: begin
                if (tx_ready) begin  // Wait for UART TX to be ready
                    tx_valid <= 1'b1;
                    // Pack status into 1 byte:
                    // [7:6] = reserved
                    // [5] = ub_done, [4] = ub_busy
                    // [3] = vpu_done, [2] = vpu_busy
                    // [1] = sys_done, [0] = sys_busy
                    tx_data <= {2'b00, ub_done, ub_busy, vpu_done, vpu_busy, sys_done, sys_busy};
                    state <= IDLE;
                end
            end
        endcase
    end
end
```

**Critical Design Choice:** Why **state machine** instead of microcoded sequencer?

- **State machines** are **single-cycle transitions** (pure combinational decode)
- **Microcoded** would require ROM reads (2-3 cycle latency)
- For UART at 115200 baud, we receive 1 byte every **868 × 10 = 8,680 clocks**, so latency doesn't matter, but simplicity does.

---

### Layer 3: Host Driver — macOS Serial Communication

**macOS Serial Driver Stack:**

```
┌─────────────────────────────────────────────────────────────┐
│ Python Application (tpu_coprocessor_driver.py)              │
│ • High-level API: write_instruction(), start_execution()   │
└────────────────────┬────────────────────────────────────────┘
                     │ pyserial library
                     ↓
┌─────────────────────────────────────────────────────────────┐
│ pyserial (cross-platform wrapper)                           │
│ • Calls macOS IOKit serial APIs                             │
└────────────────────┬────────────────────────────────────────┘
                     │ System calls
                     ↓
┌─────────────────────────────────────────────────────────────┐
│ macOS IOKit Serial Driver (/dev/tty.usbserial-xxxxxx)       │
│ • Buffering, flow control, baud rate management             │
└────────────────────┬────────────────────────────────────────┘
                     │ USB bulk transfers
                     ↓
┌─────────────────────────────────────────────────────────────┐
│ FTDI FT2232HQ USB Driver (AppleUSBFTDI.kext)                │
│ • Translates serial API to USB packets                      │
│ • 16ms latency timer (!)                                    │
└────────────────────┬────────────────────────────────────────┘
                     │ USB 2.0
                     ↓
┌─────────────────────────────────────────────────────────────┐
│ FT2232HQ Chip (on Basys3 board)                             │
│ • USB 2.0 Hi-Speed to Dual UART                             │
│ • 256-byte RX buffer, 128-byte TX buffer                    │
└────────────────────┬────────────────────────────────────────┘
                     │ UART (3.3V logic)
                     ↓
┌─────────────────────────────────────────────────────────────┐
│ Artix-7 FPGA (uart_rx.sv)                                   │
└─────────────────────────────────────────────────────────────┘
```

**The FT2232HQ Latency Problem:**

The FTDI chip has a **16ms latency timer**. If a USB packet is incomplete (<64 bytes), the chip waits 16ms before flushing to USB. This is DISASTROUS for single-instruction sends:

```python
# BAD: Send 1 instruction (7 bytes)
tpu.write_instruction(0, instr)  # Packet = 7 bytes
                                 # FT2232HQ waits 16ms!
                                 # UART transmission = 0.6ms
                                 # Total latency = 16.6ms

# GOOD: Batch instructions (>64 bytes)
for i in range(16):  # 16 × 7 = 112 bytes
    packet += encode_instr(i, instrs[i])
ser.write(packet)  # Single USB transfer, no 16ms wait
```

**Python Driver API (tpu_coprocessor_driver.py):**

```python
import serial
import struct
import time

class TPU_Coprocessor:
    def __init__(self, port='/dev/tty.usbserial-210319AF5C91', baudrate=115200):
        """
        Initialize connection to TPU coprocessor.

        Args:
            port: macOS serial device (find with: ls /dev/tty.usbserial*)
            baudrate: UART baud rate (must match FPGA setting)
        """
        self.ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=2.0,              # 2-second read timeout
            write_timeout=2.0         # 2-second write timeout
        )
        time.sleep(0.1)  # Allow UART to stabilize
        print(f"Connected to {port} @ {baudrate} baud")

    def write_instruction(self, addr, instr):
        """
        Write single 32-bit instruction to instruction memory.

        Args:
            addr: Instruction address (0-31)
            instr: Instruction object with (opcode, arg1, arg2, arg3, flags)

        Packet format:
          [0x03][0x00][addr][instr_byte0][instr_byte1][instr_byte2][instr_byte3]
        """
        # Encode instruction to 32-bit word
        word = (instr.opcode << 26) | (instr.arg1 << 18) | \
               (instr.arg2 << 10) | (instr.arg3 << 2) | instr.flags

        # Pack as big-endian (MSB first)
        instr_bytes = struct.pack('>I', word)

        # Build packet
        packet = bytearray([
            0x03,           # Command: WRITE_INSTR
            0x00,           # ADDR_HI (unused for instructions)
            addr & 0xFF,    # ADDR_LO
        ])
        packet.extend(instr_bytes)

        # Send via UART
        self.ser.write(packet)
        time.sleep(0.005)  # 5ms delay (empirical)

    def start_execution(self):
        """
        Send EXECUTE command to start TPU from PC=0.

        Packet: [0x05]
        """
        packet = bytearray([0x05])
        self.ser.write(packet)
        time.sleep(0.001)

    def read_status(self):
        """
        Read 1-byte status register.

        Returns:
            int: Status byte
              [0] = sys_busy
              [1] = sys_done
              [2] = vpu_busy
              [3] = vpu_done
              [4] = ub_busy
              [5] = ub_done
        """
        packet = bytearray([0x06])
        self.ser.write(packet)

        # Read 1-byte response
        status_byte = self.ser.read(1)
        if len(status_byte) != 1:
            raise TimeoutError("No response from FPGA")

        return status_byte[0]

    def wait_done(self, timeout=5.0):
        """
        Poll status until sys_busy=0 (execution complete).

        Args:
            timeout: Maximum wait time in seconds
        """
        start_time = time.time()
        while True:
            status = self.read_status()
            if not (status & 0x01):  # Check sys_busy bit
                return

            if time.time() - start_time > timeout:
                raise TimeoutError(f"Execution timeout (status=0x{status:02X})")

            time.sleep(0.010)  # Poll every 10ms
```

**Complete Usage Example:**

```python
from tpu_coprocessor_driver import TPU_Coprocessor
from instruction_encoder import *
import numpy as np

# Connect
tpu = TPU_Coprocessor(port='/dev/tty.usbserial-210319AF5C91', baudrate=115200)

# Program instructions
program = [
    build_matmul(ub_addr=0x00, acc_addr=0x20, rows=16, signed=False),
    build_relu(acc_addr=0x20, ub_addr=0x40, length=256),
    build_halt()
]

for i, instr in enumerate(program):
    tpu.write_instruction(i, instr)
    print(f"[{i}] {format_instruction(instr)}")

# Execute
print("Starting execution...")
tpu.start_execution()

# Wait for completion
tpu.wait_done(timeout=2.0)
print("Done!")
```

---

## Part III: The ISA — From Opcodes to Control Signals

### Instruction Encoding: The 32-Bit RISC Format

Our ISA uses a **fixed-length 32-bit encoding** for all 20 instructions. This design choice trades code density for decoder simplicity.

**Instruction Format:**

```
 Bit:   31    26 25    18 17    10 9      2 1  0
       ┌────────┬────────┬────────┬────────┬────┐
       │ OPCODE │  ARG1  │  ARG2  │  ARG3  │FLG │
       │ 6 bits │ 8 bits │ 8 bits │ 8 bits │2bit│
       └────────┴────────┴────────┴────────┴────┘
          ↓        ↓        ↓        ↓       ↓
       Operation Source  Dest    Count/   Mods
                  Addr    Addr    Stride
```

**Field Semantics:**

| Field | Bits | Range | Usage |
|-------|------|-------|-------|
| **OPCODE** | [31:26] | 0-63 | Instruction type (20 defined, 44 reserved) |
| **ARG1** | [25:18] | 0-255 | Source address (UB, Acc, Weight FIFO) |
| **ARG2** | [17:10] | 0-255 | Destination address (UB, Acc) |
| **ARG3** | [9:2] | 0-255 | Loop count, stride, or configuration |
| **FLAGS** | [1:0] | 0-3 | Signed/unsigned, transpose, etc. |

**Design Rationale:**

1. **6-bit opcode** → 64 possible instructions (room for 44 more)
2. **8-bit addresses** → Sufficient for 256-entry memories
   - Unified Buffer: 128 entries × 2 banks = 256 addresses
   - Accumulator: 32 entries × 2 banks = 64 addresses (upper bits unused)
3. **8-bit count** → Max 255 elements per operation (sufficient for 16×16 matrices)
4. **2-bit flags** → Common modifiers without wasting space

**Example: MATMUL Encoding**

```
MATMUL ub_addr=0x00, acc_addr=0x20, rows=16, signed=True

Binary breakdown:
  OPCODE  = 0x10 = 0b010000
  ARG1    = 0x00 = 0b00000000  (UB address)
  ARG2    = 0x20 = 0b00100000  (Accumulator address)
  ARG3    = 16   = 0b00010000  (Number of rows)
  FLAGS   = 0b10               (Bit 1=signed, Bit 0=no transpose)

32-bit word:
  0b 010000 00000000 00100000 00010000 10
     ^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^
     OPCODE   ARG1     ARG2     ARG3   FLG

  Hexadecimal: 0x40_00_20_42
               = (0x10 << 26) | (0x00 << 18) | (0x20 << 10) | (16 << 2) | 0b10
```

---

### Control Signal Explosion: 29 Signals per Instruction

The controller's primary job is to **decode opcodes into control signals**. For a MATMUL instruction, the controller must:

1. **Enable systolic array** (`sys_start = 1`)
2. **Set systolic mode** (`sys_mode = 2'b00` for matrix multiply)
3. **Configure row count** (`sys_rows = 16`)
4. **Enable signed arithmetic** (`sys_signed = 1`)
5. **Read from Unified Buffer** (`ub_rd_en = 1`, `ub_rd_addr = 0x00`)
6. **Read from Weight FIFO** (`wt_mem_rd_en = 1`)
7. **Write to Accumulator** (`acc_wr_en = 1`, `acc_addr = 0x20`)
8. **Select correct memory banks** (`ub_buf_sel`, `acc_buf_sel`)
9. ... plus 21 more signals!

**Full Control Signal List (29 signals):**

```systemverilog
// Systolic Array Control (7 signals)
output reg         sys_start;
output reg [1:0]   sys_mode;        // 00=MatMul, 01=Conv2D, 10=Accumulate
output reg [7:0]   sys_rows;
output reg         sys_signed;
output reg         sys_transpose;
output reg [7:0]   sys_acc_addr;
output reg         sys_acc_clear;

// Unified Buffer Control (7 signals)
output reg         ub_rd_en;
output reg         ub_wr_en;
output reg [8:0]   ub_rd_addr;     // 9-bit (bit 8 = bank select)
output reg [8:0]   ub_wr_addr;
output reg [8:0]   ub_rd_count;
output reg [8:0]   ub_wr_count;
output reg         ub_buf_sel;      // Double-buffer toggle

// Weight FIFO Control (5 signals)
output reg         wt_mem_rd_en;
output reg [23:0]  wt_mem_addr;
output reg         wt_fifo_wr;
output reg [7:0]   wt_num_tiles;
output reg         wt_buf_sel;

// Accumulator Control (4 signals)
output reg         acc_wr_en;
output reg         acc_rd_en;
output reg [7:0]   acc_addr;
output reg         acc_buf_sel;

// VPU Control (6 signals)
output reg         vpu_start;
output reg [3:0]   vpu_mode;        // 0001=ReLU, 0010=Sigmoid, etc.
output reg [7:0]   vpu_in_addr;
output reg [7:0]   vpu_out_addr;
output reg [7:0]   vpu_length;
output reg [15:0]  vpu_param;       // Optional parameter
```

**Decoder Implementation (tpu_controller.sv, lines 500-620):**

```systemverilog
// Opcode definitions
localparam MATMUL   = 6'h10;
localparam CONV2D   = 6'h11;
localparam RELU     = 6'h18;
localparam SIGMOID  = 6'h1A;
localparam HALT     = 6'h3F;

// Control signal generation (combinational)
always_comb begin
    // Default: All control signals OFF
    sys_start = 1'b0;
    sys_mode = 2'b00;
    sys_rows = 8'h00;
    // ... (25 more signals = 0)

    // Decode based on opcode
    case (exec_opcode)
        // =============================================================
        // MATMUL: Matrix Multiply
        // =============================================================
        MATMUL: begin
            // Systolic array
            sys_start = 1'b1;
            sys_mode = 2'b00;                    // Matrix multiply mode
            sys_rows = exec_arg3[7:0];           // Number of rows
            sys_signed = exec_flags[1];          // Signed arithmetic?
            sys_transpose = exec_flags[0];       // Transpose input?
            sys_acc_addr = exec_arg2;            // Accumulator address
            sys_acc_clear = 1'b0;                // Don't clear accumulator

            // Unified Buffer
            ub_rd_en = 1'b1;                     // Enable read
            ub_rd_addr = {1'b0, exec_arg1};      // Address (bit 8=bank, 7:0=addr)
            ub_rd_count = {1'b0, exec_arg3};     // Number of rows to read
            ub_buf_sel = current_ub_buf;         // Current bank selection

            // Weight FIFO
            wt_mem_rd_en = 1'b1;                 // Read weights from DRAM
            wt_fifo_wr = 1'b1;                   // Stream to FIFO
            wt_num_tiles = exec_arg3;            // Number of tiles

            // Accumulator
            acc_wr_en = 1'b1;                    // Write results
            acc_addr = exec_arg2;                // Destination address
            acc_buf_sel = current_acc_buf;       // Current bank selection
        end

        // =============================================================
        // RELU: Activation Function
        // =============================================================
        RELU: begin
            // VPU
            vpu_start = 1'b1;
            vpu_mode = 4'h1;                     // ReLU mode
            vpu_in_addr = exec_arg1;             // Read from accumulator
            vpu_out_addr = exec_arg2;            // Write to UB
            vpu_length = exec_arg3;              // Number of elements

            // Accumulator (read)
            acc_rd_en = 1'b1;
            acc_addr = exec_arg1;

            // Unified Buffer (write)
            ub_wr_en = 1'b1;
            ub_wr_addr = {1'b0, exec_arg2};
            ub_wr_count = {1'b0, exec_arg3};
        end

        // =============================================================
        // SIGMOID: Activation Function (LUT-based)
        // =============================================================
        SIGMOID: begin
            vpu_start = 1'b1;
            vpu_mode = 4'h2;                     // Sigmoid mode
            vpu_in_addr = exec_arg1;
            vpu_out_addr = exec_arg2;
            vpu_length = exec_arg3;

            acc_rd_en = 1'b1;
            acc_addr = exec_arg1;

            ub_wr_en = 1'b1;
            ub_wr_addr = {1'b0, exec_arg2};
        end

        // ... 17 more instruction decoders ...

        default: begin
            // All signals remain at default (0)
        end
    endcase
end
```

**Critical Observation:** This is a **purely combinational** decoder. The opcode→control mapping happens in **0 clock cycles** (subject only to gate delays). This is why RISC architectures are fast: no microcode ROM reads.

---

### The Missing Sigmoid: LUT-Based Approximation (Not Implemented)

The ISA defines **SIGMOID (0x1A)** and **TANH (0x1B)** opcodes, but the **RTL implementation is incomplete**!

**Current State (vpu.sv, lines 81-99):**

```systemverilog
case (vpu_mode)
    MODE_RELU: begin
        // IMPLEMENTED: ReLU = max(0, x)
        relu_result0 = ($signed(vpu_in_data[31:0]) > 0) ?
                       vpu_in_data[31:0] : 32'h00000000;
    end

    MODE_SIGMOID: begin
        // NOT IMPLEMENTED! Falls through to default (pass-through)
    end

    default: begin
        // Pass-through (no activation)
        data_buffer[i] <= vpu_in_data[31:0];
    end
endcase
```

**Why Sigmoid is Hard in Hardware:**

The sigmoid function is:

```
σ(x) = 1 / (1 + e^(-x))
```

This requires:
1. **Exponential function** (e^(-x)) — NOT a simple gate operation
2. **Division** (1 / ...) — Expensive in hardware (32-cycle divider)
3. **High precision** (floating-point or fixed-point with many bits)

**Solution: Piecewise Linear Approximation + LUT**

We can approximate sigmoid using a **8-segment piecewise linear** function:

```
          1.0 ┤                    ╭──────
              │                 ╭──┘
          0.5 ┤             ╭──┘
              │          ╭──┘
          0.0 ┼──────╭──┘
              └────────────────────────────
             -8  -4   0   4   8

Segments:
  x < -4.0:  σ(x) ≈ 0.0
  x ∈ [-4, -2): σ(x) ≈ 0.02 + 0.05×(x+4)
  x ∈ [-2, -1): σ(x) ≈ 0.12 + 0.10×(x+2)
  x ∈ [-1, 0):  σ(x) ≈ 0.27 + 0.23×(x+1)
  x ∈ [0, 1):   σ(x) ≈ 0.50 + 0.23×x
  x ∈ [1, 2):   σ(x) ≈ 0.73 + 0.10×(x-1)
  x ∈ [2, 4):   σ(x) ≈ 0.88 + 0.05×(x-2)
  x ∈ [4, 8):   σ(x) ≈ 0.98 + 0.01×(x-4)
  x ≥ 8.0:   σ(x) ≈ 1.0
```

**LUT Implementation (SystemVerilog):**

```systemverilog
// Sigmoid LUT (8 segments × 2 parameters = 16 entries)
// Format: {slope[15:0], intercept[15:0]} in Q8.8 fixed-point
logic [31:0] sigmoid_lut [0:7];

initial begin
    sigmoid_lut[0] = {16'h0000, 16'h0000};  // x < -4: slope=0, intercept=0
    sigmoid_lut[1] = {16'h000D, 16'h0005};  // x∈[-4,-2): slope=0.05, int=0.02
    sigmoid_lut[2] = {16'h0019, 16'h001F};  // x∈[-2,-1): slope=0.10, int=0.12
    sigmoid_lut[3] = {16'h003B, 16'h0045};  // x∈[-1,0): slope=0.23, int=0.27
    sigmoid_lut[4] = {16'h003B, 16'h0080};  // x∈[0,1): slope=0.23, int=0.50
    sigmoid_lut[5] = {16'h0019, 16'h00BB};  // x∈[1,2): slope=0.10, int=0.73
    sigmoid_lut[6] = {16'h000D, 16'h00E1};  // x∈[2,4): slope=0.05, int=0.88
    sigmoid_lut[7] = {16'h0003, 16'h00FB};  // x∈[4,8): slope=0.01, int=0.98
end

// Sigmoid computation
always_comb begin
    // Select segment based on input value
    if (x_in < -16'sd1024) segment = 3'd0;      // x < -4 (Q8.8)
    else if (x_in < -16'sd512) segment = 3'd1;  // x ∈ [-4, -2)
    else if (x_in < -16'sd256) segment = 3'd2;  // x ∈ [-2, -1)
    else if (x_in < 16'sd0) segment = 3'd3;     // x ∈ [-1, 0)
    else if (x_in < 16'sd256) segment = 3'd4;   // x ∈ [0, 1)
    else if (x_in < 16'sd512) segment = 3'd5;   // x ∈ [1, 2)
    else if (x_in < 16'sd1024) segment = 3'd6;  // x ∈ [2, 4)
    else segment = 3'd7;                        // x ≥ 4

    // Lookup slope and intercept
    {slope, intercept} = sigmoid_lut[segment];

    // Linear approximation: y = slope × x + intercept
    product = x_in * slope;          // Q8.8 × Q8.8 = Q16.16
    result = (product >>> 8) + intercept;  // Back to Q8.8
end
```

**Resource Cost:**
- **LUT Storage:** 8 entries × 32 bits = 256 bits = 8 LUT6 (negligible)
- **Multiplier:** 1× DSP48E1 (or 16× LUTs if using soft multiply)
- **Comparison Chain:** 7× comparators = ~35 LUTs
- **Total:** ~50 LUTs + 1 DSP (or ~100 LUTs without DSP)

**Error Analysis:**

```
Max absolute error: ±0.05 (compared to true sigmoid)
Typical error: ±0.02
This is sufficient for neural network inference (INT8 quantization error is ~0.01)
```

---

## Part IV: Yosys Synthesis — From RTL to Netlist

### What is Synthesis?

**Synthesis** is the process of converting RTL (Register Transfer Level) Verilog/SystemVerilog into a gate-level netlist. It involves:

1. **Parsing** RTL into an abstract syntax tree (AST)
2. **Elaboration** (expanding `generate` loops, resolving parameters)
3. **High-level optimization** (constant propagation, dead code elimination)
4. **Technology mapping** (RTL constructs → FPGA primitives)
5. **Logic optimization** (minimizing LUTs, reducing critical paths)

**Yosys** is an open-source synthesis framework developed by Claire Xen (formerly Clifford Wolf). It's scriptable, extensible, and supports Verilog-2005 and most of SystemVerilog.

---

### Our Yosys Synthesis Script (Detailed Walkthrough)

**File:** `synthesis/yosys/synth_basys3_vivado.ys`

```tcl
# =====================================================================
# STEP 1: Read RTL Sources
# =====================================================================

# Read all SystemVerilog files in dependency order
# (low-level modules first, top-level modules last)

read_verilog -sv rtl/pe.sv
read_verilog -sv rtl/pe_dsp.sv
read_verilog -sv rtl/activation_func.sv
read_verilog -sv rtl/normalizer.sv
read_verilog -sv rtl/loss_block.sv
read_verilog -sv rtl/uart_rx.sv
read_verilog -sv rtl/uart_tx.sv
read_verilog -sv rtl/mmu.sv
read_verilog -sv rtl/systolic_controller.sv
read_verilog -sv rtl/accumulator.sv
read_verilog -sv rtl/accumulator_mem.sv
read_verilog -sv rtl/activation_pipeline.sv
read_verilog -sv rtl/dual_weight_fifo.sv
read_verilog -sv rtl/weight_fifo.sv
read_verilog -sv rtl/unified_buffer.sv
read_verilog -sv rtl/uart_dma_basys3.sv
read_verilog -sv rtl/tpu_controller.sv
read_verilog -sv rtl/tpu_datapath.sv
read_verilog -sv rtl/basys3_test_interface.sv
read_verilog -sv rtl/tpu_top.sv
read_verilog -sv rtl/tpu_top_wrapper.sv
# ... 12 more files (33 total)

# Read Xilinx primitive library (required for tech-mapping)
read_verilog /usr/local/share/yosys/xilinx/cells_sim.v

# =====================================================================
# STEP 2: Hierarchy Elaboration
# =====================================================================

# Set top module
hierarchy -top tpu_top_wrapper

# Check for undefined modules
hierarchy -check

# Verify all modules are defined (catch typos early)
check

# =====================================================================
# STEP 3: High-Level RTL Transformations
# =====================================================================

# Convert always blocks to logic gates
proc

# Explanation:
#   always @(posedge clk) q <= d;
#   → Becomes: DFF with D=d, Q=q, CLK=clk

# Handle undefined values (X → 0)
setundef -zero

# Explanation:
#   Uninitialized registers are 'X' in simulation
#   In hardware, they power up to random 0/1
#   This pass converts X to 0 for synthesis

# =====================================================================
# STEP 4: Xilinx-Specific Synthesis
# =====================================================================

# Synthesize for Xilinx 7-series architecture
synth_xilinx -flatten -abc9 -arch xc7 -top tpu_top_wrapper -nodsp

# Flag breakdown:
#   -flatten: Flatten module hierarchy (except blackboxes)
#             Pros: Better cross-module optimization
#             Cons: Harder to debug netlists
#
#   -abc9: Use ABC9 logic optimizer (instead of ABC)
#          ABC9 uses And-Inverter Graphs (AIGs) for better LUT minimization
#          Typical improvement: 5-10% LUT reduction vs ABC
#
#   -arch xc7: Target Xilinx 7-series primitives
#              Maps to: LUT6, FDRE/FDCE, CARRY4, DSP48E1, RAMB18E1, etc.
#
#   -nodsp: Do NOT infer DSP48E1 blocks during synthesis
#           Let Vivado handle DSP inference during implementation
#           Why? Vivado has better DSP placement constraints
#
#   -top: Top-level module name

# =====================================================================
# STEP 5: Post-Synthesis Optimization
# =====================================================================

# Full optimization pass (constant propagation, CSE, etc.)
opt -full

# Remove unused cells and wires
clean -purge

# =====================================================================
# STEP 6: Output Netlist in EDIF Format
# =====================================================================

# Write EDIF (Electronic Design Interchange Format)
# EDIF is an industry-standard netlist format that Vivado can read
write_edif -pvector bra -attrprop build/tpu_basys3.edif

# Flag breakdown:
#   -pvector bra: Use bracket notation for bus vectors
#                 Example: data[7:0] instead of data<7:0>
#   -attrprop: Propagate attributes (like (* keep *), (* ram_style *))

# =====================================================================
# STEP 7: Generate Reports
# =====================================================================

# Print resource utilization statistics
stat

# Write Verilog netlist (for debugging)
write_verilog -noattr -noexpr build/tpu_basys3_synth.v

# Flags:
#   -noattr: Don't include (* attribute *) comments
#   -noexpr: Don't use ternary operators (? :) in output
```

**Running Yosys:**

```bash
cd /Users/abiralshakya/Documents/tpu_to_fpga

# Run synthesis
yosys synthesis/yosys/synth_basys3_vivado.ys 2>&1 | tee yosys_build.log

# Expected output (truncated):
# ...
# === tpu_top_wrapper ===
#
#    Number of wires:              14562
#    Number of wire bits:          24389
#    Number of public wires:        1024
#    Number of cells:              16874
#      CARRY4                          35
#      FDCE                           336
#      FDRE                          1185
#      LUT1                            40
#      LUT2                           102
#      LUT3                           562
#      LUT4                           121
#      LUT5                           116
#      LUT6                           502
#      RAM64M                         340  ← Distributed RAM
#      ...
#
# Chip area for module '\tpu_top_wrapper': 12543.612345
```

**What Just Happened?**

1. **RTL → Gates:** Your SystemVerilog `always @(posedge clk)` blocks became FDRE flip-flops
2. **Combinational Logic → LUTs:** Your `assign y = a & b | c;` became LUT6 primitives
3. **Memory → RAM64M:** Your `reg [255:0] mem [0:127];` became distributed RAM
4. **Arithmetic → CARRY4:** Your `count + 1` became a carry chain

**Key Yosys Optimization: ABC9**

ABC9 (And-Inverter Graph optimization) is a **logic minimization** algorithm. It works by:

1. Converting logic to AIG (directed acyclic graph of 2-input AND gates + inverters)
2. Applying rewrite rules to reduce gate count
3. Mapping back to LUT6 primitives

**Example:**

```verilog
// Original RTL
assign y = (a & b) | (a & c) | (b & c);

// Naive synthesis: 3 ANDs + 2 ORs = 5 gates
// ABC9 synthesis: Factor to (a & (b | c)) | (b & c) = 3 gates
// LUT6 mapping: 1 LUT6 (all 3 inputs fit in 6-input LUT)
```

**Result:** Yosys typically produces 5-10% fewer LUTs than Vivado's built-in synthesis.

---

## Part V: Vivado Place & Route vs. nextpnr-xilinx

### The Problem: Why Not Use nextpnr-xilinx for Everything?

**nextpnr-xilinx** is an open-source place-and-route tool for Xilinx 7-series FPGAs. It's part of the **SymbiFlow** project (now **F4PGA**). However, it has **critical limitations**:

---

### nextpnr-xilinx Architecture

**How nextpnr Works:**

```
Netlist (JSON)
     ↓
┌────────────────────────────────────────────────────────────┐
│ 1. Packing                                                 │
│    Group related cells into BELs (Basic Elements)          │
│    Example: 2 LUT6 + 2 FDRE → 1 SLICE (has 4 LUT+FF pairs)│
└────────────────────────────────────────────────────────────┘
     ↓
┌────────────────────────────────────────────────────────────┐
│ 2. Placement (Simulated Annealing)                        │
│    • Start with random placement                           │
│    • Iteratively swap cells to minimize cost function:     │
│      Cost = α×wirelength + β×timing_slack                  │
│    • Accept swaps if ΔCost < 0 (always)                    │
│      or with probability e^(-ΔCost/T) (sometimes)          │
│    • Decrease temperature T over 10,000+ iterations        │
└────────────────────────────────────────────────────────────┘
     ↓
┌────────────────────────────────────────────────────────────┐
│ 3. Routing (A* Pathfinding)                               │
│    • For each net, find path from source to all sinks      │
│    • Use A* with cost = path_length + congestion_penalty   │
│    • If routing fails (congestion), rip-up and reroute     │
└────────────────────────────────────────────────────────────┘
     ↓
┌────────────────────────────────────────────────────────────┐
│ 4. Timing Analysis                                         │
│    • Calculate arrival times for all paths                 │
│    • Report Worst Negative Slack (WNS)                     │
│    • WNS < 0 = TIMING VIOLATION                            │
└────────────────────────────────────────────────────────────┘
     ↓
FASM (bitstream assembly)
```

**Simulated Annealing Explained:**

```python
# Pseudocode for simulated annealing placement
def simulated_annealing_placement(netlist, temperature):
    placement = random_initial_placement(netlist)
    cost = compute_cost(placement)

    for iteration in range(10000):
        # Try random swap
        swap = random_swap(placement)
        new_cost = compute_cost(swap)
        delta_cost = new_cost - cost

        # Accept if better, or probabilistically if worse
        if delta_cost < 0:
            placement = swap  # Always accept improvements
            cost = new_cost
        elif random() < exp(-delta_cost / temperature):
            placement = swap  # Sometimes accept worse swaps (escape local minima)
            cost = new_cost

        # Cool down (reduce probability of accepting worse swaps)
        temperature *= 0.9995

    return placement
```

**Why This is Slow:**
- Each iteration computes **total wirelength** (sum of Manhattan distances for all nets)
- 10,000 iterations × 3,421 nets × O(fanout) = **millions of distance calculations**
- For our TPU design: **45 minutes** vs. Vivado's **2 minutes**

---

### nextpnr-xilinx Timing Model Inaccuracy

**The Critical Flaw:** nextpnr uses timing models reverse-engineered by **Project X-Ray**, which are **incomplete** for:

1. **Block RAM (RAMB18E1)** setup/hold times
2. **DSP48E1** internal pipeline delays
3. **Long-distance routing** (>10 CLB tiles)

**Concrete Example from Our Build:**

```
# nextpnr-xilinx timing report
Critical Path (11.28 ns):
  FDRE(Q) → LUT6(I0) [0.45 ns]  ← Accurate
  LUT6(O) → CARRY4(S0) [0.23 ns]  ← Accurate
  CARRY4(CO3) → LUT6(I1) [0.56 ns]  ← INACCURATE (missing carry delay)
  LUT6(O) → FDRE(D) [0.89 ns]  ← Accurate
  Total: 2.13 ns (logic) + 9.15 ns (routing)

  WNS = 10.0 ns (period) - 11.28 ns (delay) = -1.28 ns
  → TIMING VIOLATION!

# Vivado timing report (same critical path)
Critical Path (8.57 ns):
  FDRE(Q) → LUT6(I0) [0.45 ns]
  LUT6(O) → CARRY4(S0) [0.23 ns]
  CARRY4(CO3) → LUT6(I1) [0.31 ns]  ← Correct delay model
  LUT6(O) → FDRE(D) [0.89 ns]
  Total: 1.88 ns (logic) + 6.69 ns (routing)

  WNS = 10.0 ns - 8.57 ns = +1.43 ns
  → PASS!
```

**The Problem:** nextpnr's CARRY4 delay model is **80% higher** than reality! This causes false timing violations.

---

### Why We Use Vivado for Place & Route

**Vivado's Advantages:**

1. **Accurate Timing Models**
   - Xilinx has **characterized every primitive** on real silicon
   - Setup/hold times extracted from SPICE simulations
   - Routing delay models calibrated with parasitic extraction

2. **Physical Optimization (phys_opt_design)**
   - **Retiming:** Move registers across combinational logic to balance delays
   - **Replication:** Duplicate high-fanout cells to reduce routing congestion
   - **Buffering:** Insert BUFGs on high-fanout clock trees
   - **Logic restructuring:** Rewrite LUTs to improve timing

3. **Constraint-Driven Placement**
   - Understands `PBLOCK` constraints (place DSPs in specific columns)
   - Respects `LOC` constraints (pin assignments)
   - Handles `CLOCK_DEDICATED_ROUTE` (clock routing rules)

4. **Multi-Pass Routing**
   - Initial routing with low congestion tolerance
   - Rip-up and reroute with higher effort for failing nets
   - Timing-driven routing (prioritize critical paths)

**Vivado Implementation Script:**

```tcl
# vivado/build_tpu_vivado.tcl

# ===================================================================
# Step 1: Create In-Memory Project (No GUI, No Disk I/O)
# ===================================================================
create_project -in_memory -part xc7a35tcpg236-1 -force

# ===================================================================
# Step 2: Import EDIF Netlist from Yosys
# ===================================================================
read_edif build/tpu_basys3.edif

# Link design (resolve module references)
link_design -part xc7a35tcpg236-1 -top tpu_top_wrapper

# ===================================================================
# Step 3: Read Constraints
# ===================================================================
read_xdc constraints/basys3_vivado.xdc

# Example XDC constraints:
#   create_clock -period 10.0 [get_ports clk]  # 100 MHz
#   set_property PACKAGE_PIN E3 [get_ports clk]
#   set_property IOSTANDARD LVCMOS33 [get_ports clk]

# ===================================================================
# Step 4: Implementation Flow
# ===================================================================

# ---------------------------------------------------------------
# 4a. opt_design - Netlist Optimization
# ---------------------------------------------------------------
opt_design

# What it does:
#   • Constant propagation (if a=0, then y=a&b → y=0)
#   • Dead code elimination (remove unused logic)
#   • Merge equivalent flip-flops
#   • Push inverters through LUTs
#
# Typical result: 2-5% LUT reduction

# ---------------------------------------------------------------
# 4b. place_design - Cell Placement
# ---------------------------------------------------------------
place_design

# What it does:
#   • Assign each cell to a specific site (SLICE_X12Y45, DSP48_X3Y8, etc.)
#   • Minimize wirelength (sum of Manhattan distances)
#   • Balance utilization across CLB columns
#   • Respect placement constraints (LOC, PBLOCK)
#
# Algorithm: Hierarchical placement with analytical solving
#   1. Global placement (quadratic wirelength minimization)
#   2. Legalization (snap to valid sites)
#   3. Detail placement (local swaps for timing)

# ---------------------------------------------------------------
# 4c. phys_opt_design - Physical Optimization
# ---------------------------------------------------------------
phys_opt_design

# What it does:
#   • Retiming: Move FFs to balance path delays
#   • Replication: Duplicate cells to reduce fanout
#   • Fanout optimization: Insert buffers on high-fanout nets
#   • DSP optimization: Merge adjacent DSP48E1 blocks
#
# This is WHERE VIVADO WINS OVER NEXTPNR!
# Example: Critical path has 3 LUTs in series
#   Before: LUT → LUT → LUT → FF (delay = 3×0.5ns = 1.5ns)
#   After:  LUT → FF → LUT → FF → LUT → FF (delay = 3×0.5ns = 1.5ns, but pipelined!)

# ---------------------------------------------------------------
# 4d. route_design - Net Routing
# ---------------------------------------------------------------
route_design

# What it does:
#   • Connect all nets using routing resources (PIPs, switchboxes)
#   • Avoid congestion (too many nets in one routing channel)
#   • Minimize critical path delay (use faster routing arcs)
#
# Algorithm: Multi-pass negotiated congestion routing
#   1. Pathfinder algorithm (global routing)
#   2. Rip-up and reroute failing nets
#   3. Timing-driven routing (critical paths get priority)

# ===================================================================
# Step 5: Timing Analysis
# ===================================================================
report_timing_summary -file build/tpu_timing.rpt

# Get WNS (Worst Negative Slack)
set wns [get_property SLACK [get_timing_paths -max_paths 1 -nworst 1]]
puts "WNS = ${wns} ns"

# Check if timing met
if {$wns < 0} {
    puts "ERROR: Timing violation! WNS = ${wns} ns"
    exit 1
}

# ===================================================================
# Step 6: Generate Bitstream
# ===================================================================
write_bitstream -force build/tpu_basys3.bit

# ===================================================================
# Step 7: Cleanup
# ===================================================================
close_project
exit 0
```

**Running the Full Hybrid Flow:**

```bash
# Step 1: Yosys synthesis (2 minutes 15 seconds)
yosys synthesis/yosys/synth_basys3_vivado.ys

# Step 2: Vivado implementation (1 minute 54 seconds)
vivado -mode batch -source vivado/build_tpu_vivado.tcl 2>&1 | tee vivado_build.log

# Total: 4 minutes 9 seconds (vs 47 minutes for full nextpnr)
```

---

### Performance Comparison Table

| Metric | nextpnr-xilinx | Yosys + Vivado | Full Vivado |
|--------|----------------|----------------|-------------|
| **Synthesis Time** | 2m 15s | 2m 15s | 4m 30s |
| **Place & Route Time** | 45m 12s | 1m 54s | 1m 54s |
| **Total Build Time** | **47m 27s** | **4m 09s** ✅ | 6m 24s |
| **Max Clock Freq** | 87.3 MHz ❌ | 107.2 MHz ✅ | 107.2 MHz |
| **WNS** | -1.28 ns | +1.43 ns | +1.43 ns |
| **LUT Utilization** | 14.2% | **12.4%** ✅ | 14.8% |
| **Register Utilization** | 3.8% | 3.66% | 3.66% |
| **DSP48E1 Usage** | 9 | 9 | 9 |
| **Power Estimate** | N/A | 0.142 W | 0.138 W |
| **Tool Cost** | $0 | $0* | $3,000+ |

*Requires Vivado for P&R, but synthesis is free (Yosys)

**Winner:** **Yosys + Vivado** (11× faster than nextpnr, better LUT density than Vivado alone)

---

## Conclusion

We've explored the **complete stack** of our TPU coprocessor:

1. **Physical Layer:** UART at 115200 baud with 2-stage CDC synchronizers
2. **Protocol Layer:** Command-based DMA for instruction/data transfer
3. **Host Driver:** Python API with FT2232HQ latency mitigation
4. **ISA Design:** 32-bit RISC encoding with 29-signal control explosion
5. **Missing Sigmoid:** LUT-based piecewise linear approximation (not implemented)
6. **Yosys Synthesis:** RTL → gates with ABC9 logic minimization
7. **Vivado P&R:** Timing closure with physical optimization

**Key Takeaways:**

- UART is **fine for prototyping** but needs replacement with PCIe for production
- **Metastability prevention** requires multi-stage synchronizers
- **Fixed-length ISA** simplifies decoder at cost of code density
- **Yosys produces better synthesis** than Vivado (12.4% vs 14.8% LUTs)
- **nextpnr-xilinx needs work** (45-minute builds, timing model inaccuracies)
- **Hybrid flow wins:** Yosys synthesis + Vivado P&R

**GitHub:** [github.com/alanma23/tinytinytpu/](https://github.com/alanma23/tinytinytpu/)

---

Until next time...

**Discussion:** Should we tackle the **diffusion-based PNR hack** next, or deep-dive into **INT8 quantization** for the systolic array?
