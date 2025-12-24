# FT2232HQ USB-UART Bridge Analysis for Basys3

## Hardware Context

The Basys3 board uses the **FT2232HQ USB-UART bridge chip** for serial communication between the host PC and the FPGA. This is a critical component that introduces specific timing and buffering characteristics.

---

## FT2232HQ Characteristics

### Key Features:
- **Dual-channel** USB-to-UART/FIFO converter
- **Internal FIFO buffers**: 
  - TX: 128 bytes
  - RX: 256 bytes
- **Baud rates**: 300 baud to 12 Mbaud (supports 115200)
- **Built-in flow control**: RTS/CTS, DTR/DSR, XON/XOFF
- **USB 2.0 Hi-Speed** (480 Mbps) to UART conversion
- **Latency timer**: Default 16ms (can cause delays in small packet transfers)

### Critical Timing Parameters:
```
USB Polling Interval:     1ms (USB 2.0 Hi-Speed)
Latency Timer:           16ms (default, configurable 1-255ms)
FIFO Threshold:          Programmable
TX Buffer:               128 bytes
RX Buffer:               256 bytes
```

---

## UART Fundamentals Applied to Your Design

### 1. Baud Rate Configuration

#### FT2232HQ Side (PC → UART):
- Configured by PySerial: `115200` baud
- USB packets delivered every 1ms
- Internal clock: **48 MHz** (provides precise baud rate generation)

#### FPGA Side (Your RTL):
```systemverilog:rtl/uart_rx.sv
parameter CLOCK_FREQ = 100_000_000,
parameter BAUD_RATE  = 115200

localparam CLKS_PER_BIT = CLOCK_FREQ / BAUD_RATE;  // = 868 clocks/bit
localparam HALF_BIT = CLKS_PER_BIT / 2;             // = 434 clocks
```

**Analysis**: 
- ✅ Clock divider ratio: **868:1** is correct for 100MHz/115200
- ✅ Start bit detection samples at mid-bit (HALF_BIT)
- ✅ Matches standard UART practice

**Timing Accuracy**:
```
Actual bit period:  868 clocks / 100MHz = 8.68 μs
Expected (115200):  1/115200 = 8.6806 μs
Error:             ~0.007% (well within ±5% tolerance)
```

### 2. Start Bit Detection & Synchronization

#### Your Implementation:
```systemverilog:rtl/uart_rx.sv
28:// Synchronize RX input (prevent metastability)
29:reg rx_sync_1, rx_sync_2;
30:always @(posedge clk) begin
31:    rx_sync_1 <= rx;
32:    rx_sync_2 <= rx_sync_1;
33:end

53:if (rx_sync_2 == 1'b0) begin  // Start bit detected
54:    state <= START;
55:end

58:START: begin
59:    if (clk_count == HALF_BIT - 1) begin
60:        if (rx_sync_2 == 1'b0) begin  // Confirm start bit
61:            clk_count <= 16'd0;
62:            state <= DATA;
```

**Analysis**:
- ✅ **Two-stage synchronizer** (lines 29-33): Prevents metastability
- ✅ **Start bit validation** (line 60): Samples at mid-bit to confirm valid start
- ✅ **Resynchronization**: Occurs on falling edge of start bit (standard practice)

**Comparison to UART Standard**:
> "If the apparent start bit lasts at least one-half of the bit time, it is valid"

Your implementation: ✅ **Samples at HALF_BIT to validate**

### 3. Double Buffering & FIFOs

#### FT2232HQ Buffers:
```
PC → USB (bulk) → FT2232HQ RX FIFO (256B) → UART TX → FPGA RX
FPGA TX → UART RX → FT2232HQ TX FIFO (128B) → USB → PC
```

#### Your FPGA Implementation:

**Low-Level UART** (`uart_rx.sv`/`uart_tx.sv`):
- ✅ Separate shift registers for RX/TX (full-duplex)
- ✅ `rx_ready` flag indicates idle state
- ✅ `tx_ready` flag indicates available for next byte
- ❌ **NO FIFO** - Single-byte buffering only

**Protocol Level** (`uart_dma_basys3.sv`):
```systemverilog:301-314
WRITE_INSTR: begin
    if (rx_valid) begin
        instr_buffer <= {instr_buffer[23:0], rx_data};
        byte_count <= byte_count + 1;
        
        if (byte_count == 16'd3) begin
            // Write complete instruction
            instr_wr_en <= 1'b1;
            instr_wr_addr <= addr_lo[4:0];
            instr_wr_data <= {instr_buffer[23:0], rx_data};
            state <= IDLE;  // ← No ACK sent
        end
    end
end
```

**Analysis**:
- ✅ Accumulates 4 bytes into `instr_buffer`
- ✅ Writes complete instruction atomically
- ❌ **NO acknowledgment sent back** to host
- ❌ **NO FIFO for instructions** - directly writes to instruction memory

---

## Latency Issues with FT2232HQ

### The 16ms Latency Timer Problem

The FT2232HQ has a **latency timer** (default 16ms) that controls when it flushes partial USB packets:

```
Scenario 1: Large Data Transfer (>64 bytes)
PC → Python → USB bulk → FT2232HQ → FPGA
    Fast! USB packet full, sent immediately

Scenario 2: Small Data Transfer (<64 bytes)
PC → Python → USB bulk → FT2232HQ (waits...) → FPGA
    Wait 16ms OR until 64-byte threshold
    
Your streaming mode sends: 4 bytes/instruction
Expected: 16ms delay per instruction!
```

### How This Affects Your Protocol:

#### Current Streaming Expectation:
```python
# Python sends 4-byte instruction
self.ser.write(instr_bytes)  # 4 bytes → FT2232HQ TX FIFO
time.sleep(0)                # No delay

# Immediately try to read ACK
response = self._read_response(1)  # BLOCKS!
```

#### What Actually Happens:
```
t=0ms:    Python writes 4 bytes to FT2232HQ TX FIFO
t=0ms:    FT2232HQ: "Only 4 bytes, not 64... waiting..."
t=16ms:   FT2232HQ: "Latency timer expired, flushing"
t=16ms:   FPGA receives 4 bytes via UART
t=16ms:   FPGA processes instruction
t=16ms:   FPGA would send ACK (but doesn't - protocol issue!)
t=16ms+:  Python timeout (no ACK received)
```

### Workarounds for Latency:

1. **Reduce Latency Timer** (pyserial):
```python
import serial
ser = serial.Serial('/dev/ttyUSB0', 115200)
# Linux: Set latency timer to 1ms
import fcntl
USBDEVFS_SETLATENCY = 0x5608
fcntl.ioctl(ser.fd, USBDEVFS_SETLATENCY, struct.pack('I', 1))
```

2. **Batch Transfers** (send multiple instructions):
```python
# Send multiple instructions at once to exceed FIFO threshold
batch = b''.join([instr.to_bytes() for instr in instructions])
self.ser.write(batch)  # >64 bytes → immediate flush
```

3. **Flush Explicitly**:
```python
self.ser.write(instr_bytes)
self.ser.flush()  # Force USB transfer
```

---

## Critical Mismatch: Protocol vs. Hardware

### The Core Issue Remains:

Even with perfect UART timing, the fundamental problem is **protocol-level**:

| Layer | Status | Issue |
|-------|--------|-------|
| **Physical** (UART PHY) | ✅ Working | Baud rate, synchronization correct |
| **FT2232HQ Buffer** | ⚠️ Latency | 16ms timer can delay small packets |
| **Data Link** (Byte framing) | ✅ Working | Start/stop bits, no framing errors expected |
| **Protocol** (Command/Response) | ❌ **BROKEN** | Command 0x07 not implemented, no ACK |

### Evidence Hierarchy:

```
LOWEST PRIORITY: Physical Layer Timing
├─ Baud rate: ✅ 868 clocks/bit = 115200 baud
├─ Synchronizer: ✅ Two-stage flip-flop
└─ Start bit detection: ✅ Mid-bit sampling

MEDIUM PRIORITY: Buffer Management
├─ FT2232HQ latency: ⚠️ 16ms for small packets
├─ Single-byte buffering: ✅ Adequate for current design
└─ No flow control: ⚠️ Could cause issues at high load

HIGHEST PRIORITY: Protocol Mismatch
├─ Command 0x07: ❌ NOT IMPLEMENTED IN RTL
├─ ACK bytes: ❌ NEVER SENT BY FPGA
└─ Handshaking: ❌ NO FEEDBACK MECHANISM
    └─> This is why tests fail! <─┘
```

---

## Detailed FT2232HQ Configuration Recommendations

### 1. PySerial Settings (Python Side):

```python
import serial

ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=115200,
    bytesize=serial.EIGHTBITS,    # 8 data bits ✅
    parity=serial.PARITY_NONE,    # No parity ✅
    stopbits=serial.STOPBITS_ONE, # 1 stop bit ✅
    timeout=2,                     # 2 second timeout
    xonxoff=False,                 # No software flow control
    rtscts=False,                  # No hardware flow control (not wired on Basys3)
    dsrdtr=False                   # No DTR/DSR flow control
)

# Reduce latency (Linux)
try:
    import fcntl
    fcntl.ioctl(ser.fd, 0x5608, struct.pack('I', 1))  # 1ms latency
except:
    pass  # Windows doesn't support this

# Flush buffers on startup
ser.reset_input_buffer()
ser.reset_output_buffer()
```

### 2. FPGA Configuration (Constraints):

Your constraints should match FT2232HQ pinout:

```xdc
## USB-UART Interface (via FT2232HQ)
set_property -dict { PACKAGE_PIN B18 IOSTANDARD LVCMOS33 } [get_ports uart_rx]
set_property -dict { PACKAGE_PIN A18 IOSTANDARD LVCMOS33 } [get_ports uart_tx]

## Timing constraints for UART (asynchronous to FPGA clock)
set_input_delay -clock [get_clocks clk_out1_clock_manager_0] -min 0 [get_ports uart_rx]
set_input_delay -clock [get_clocks clk_out1_clock_manager_0] -max 8680 [get_ports uart_rx]
set_output_delay -clock [get_clocks clk_out1_clock_manager_0] -min 0 [get_ports uart_tx]
set_output_delay -clock [get_clocks clk_out1_clock_manager_0] -max 8680 [get_ports uart_tx]

## False path (UART is async)
set_false_path -from [get_ports uart_rx]
set_false_path -to [get_ports uart_tx]
```

---

## Framing Error Detection

### What Causes Framing Errors:

> "Receiving UART may detect some mismatched settings and set a 'framing error' flag"

**Framing error occurs when**:
1. Stop bit is not HIGH when expected
2. Indicates baud rate mismatch or noise

### Your Implementation:

```systemverilog:rtl/uart_rx.sv
87:STOP: begin
88:    if (clk_count == CLKS_PER_BIT - 1) begin
89:        clk_count <= 16'd0;
90:        if (rx_sync_2 == 1'b1) begin  // Valid stop bit
91:            rx_data <= rx_byte;
92:            rx_valid <= 1'b1;
93:        end
94:        state <= IDLE;
```

**Analysis**:
- ✅ Checks stop bit is HIGH (line 90)
- ❌ **No framing error flag** exposed to higher layers
- ❌ If stop bit is LOW, **silently discards data** (no error reporting)

**Improvement Needed**:
```systemverilog
output reg framing_error;

STOP: begin
    if (clk_count == CLKS_PER_BIT - 1) begin
        if (rx_sync_2 == 1'b1) begin
            rx_data <= rx_byte;
            rx_valid <= 1'b1;
            framing_error <= 1'b0;
        end else begin
            // Framing error: stop bit not HIGH
            framing_error <= 1'b1;
            rx_valid <= 1'b0;
        end
        state <= IDLE;
    end
end
```

---

## Full-Duplex Operation & Timing

### Your Implementation:

```systemverilog:rtl/uart_dma_basys3.sv
// RX instance
uart_rx #(...) uart_rx_inst (
    .clk(clk),
    .rx(uart_rx),
    .rx_data(rx_data),
    .rx_valid(rx_valid),
    .rx_ready(rx_ready)
);

// TX instance
uart_tx #(...) uart_tx_inst (
    .clk(clk),
    .tx(uart_tx),
    .tx_data(tx_data),
    .tx_valid(tx_valid),
    .tx_ready(tx_ready)
);
```

**Analysis**:
- ✅ **Separate RX and TX modules** (standard for full-duplex)
- ✅ Independent state machines
- ✅ Can send and receive simultaneously

### Timing Diagram (Theoretical - Current Protocol):

```
PC sends instruction (4 bytes):
t=0:      [CMD][BYTE0][BYTE1][BYTE2][BYTE3]
          ↓
t=16ms:   FT2232HQ flushes to UART
t=17ms:   FPGA receives all 5 bytes
t=17ms:   FPGA processes instruction
t=17ms:   FPGA should send ACK ← BUT DOESN'T!
          
PC waits for ACK:
t=17ms → t=2000ms: timeout (no data received)
```

### Timing Diagram (If ACK Were Implemented):

```
PC sends instruction:
t=0:      [0x03][0x00][0x05][INSTR4BYTES]
          ↓
t=16ms:   FPGA receives
t=16ms:   FPGA processes
t=16ms:   FPGA sends [ACK=0x00] ← NEW!
          ↑
t=17ms:   FT2232HQ receives ACK
t=17ms:   PC Python reads ACK ✅
```

---

## Recommended Test Sequence (When Hardware Available)

### Phase 1: UART Physical Layer Test

**Test 1.1: Loopback**
```python
# Add to RTL: Echo command (0xFF)
# Any byte sent is immediately echoed back
import serial
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
ser.write(b'\xFF\xAA')
response = ser.read(1)
assert response == b'\xAA', "Loopback failed!"
```

**Test 1.2: Framing Error Detection**
```python
# Try mismatched baud rates
ser_bad = serial.Serial('/dev/ttyUSB0', 9600)  # Wrong!
ser_bad.write(b'\x01\x00\x00\x00\x01')
# FPGA should reject due to framing errors
```

**Test 1.3: Latency Measurement**
```python
import time
ser.write(b'\xFF\x55')  # 2 bytes
t0 = time.time()
response = ser.read(1)
t1 = time.time()
print(f"Latency: {(t1-t0)*1000:.2f}ms")  # Expect ~16ms
```

### Phase 2: Protocol Layer Test

**Test 2.1: Status Command (Works Now)**
```python
from drivers.tpu_coprocessor_driver import TPU_Coprocessor
tpu = TPU_Coprocessor('/dev/ttyUSB0')
status = tpu.read_status()  # Command 0x06
print(status)  # Should work!
```

**Test 2.2: Instruction Write (No ACK Currently)**
```python
tpu.write_instruction(0, 0x00, 0, 0, 0)  # Command 0x03
# Currently: No response, just writes instruction
```

**Test 2.3: Streaming Mode (Currently Fails)**
```python
tpu.enable_stream_mode()  # Command 0x07 → NOT IMPLEMENTED!
# Expected: Timeout (no response from FPGA)
```

### Phase 3: After RTL Fix

After adding command 0x07 and ACK protocol:

```python
# This will work:
tpu.enable_stream_mode()  # Gets 0x00 response
tpu.stream_instruction(instr)  # Gets 0x00 ACK
tpu.stream_instruction(instr)  # Gets 0x00 ACK
# ...
```

---

## Summary: Why Handshaking Fails

### Root Cause Analysis:

```
┌─────────────────────────────────────────────────────────────┐
│ Layer 7: Application (Python Test Script)                   │
│   Status: ✅ Correctly calls enable_stream_mode()           │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│ Layer 6: Driver Protocol (tpu_coprocessor_driver.py)        │
│   Status: ✅ Sends 0x07, waits for response                 │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│ Layer 5: PySerial (USB → FT2232HQ)                          │
│   Status: ⚠️ 16ms latency, but functional                   │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│ Layer 4: FT2232HQ USB-UART Bridge                           │
│   Status: ✅ Converts USB to UART correctly                 │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│ Layer 3: UART Physical (uart_rx.sv)                         │
│   Status: ✅ Receives bytes correctly                       │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│ Layer 2: UART Protocol (uart_dma_basys3.sv)                 │
│   Status: ❌ Command 0x07 NOT IMPLEMENTED                   │
│           ❌ No ACK mechanism                                │
│           ❌ No response sent                                │
│                                                              │
│   ┌──────────────────────────────────────────────────────┐  │
│   │ IDLE: if (rx_data == 0x07) → default → stay in IDLE │  │
│   │       Never transitions to response state            │  │
│   └──────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
                     [No TX response]
                            │
                            ▼
                   Python times out after 2s
```

### The Issue is NOT:

- ❌ Baud rate mismatch (would cause framing errors)
- ❌ UART timing (synchronization is correct)
- ❌ FT2232HQ buffer issues (it's working as designed)
- ❌ Physical layer problems (single-byte echo would work)

### The Issue IS:

- ✅ **Missing protocol handler for 0x07 command**
- ✅ **No acknowledgment mechanism in RTL**
- ✅ **Python driver expects features not implemented in FPGA**

---

## Conclusion

Your **UART physical layer is correctly implemented** with proper:
- ✅ Baud rate generation (868 clocks = 115200 baud)
- ✅ Two-stage synchronization (metastability prevention)
- ✅ Start bit detection (mid-bit sampling)
- ✅ Full-duplex operation (separate RX/TX)

The **FT2232HQ bridge** introduces:
- ⚠️ 16ms latency for small packets (<64 bytes)
- ✅ Otherwise transparent USB-to-UART conversion

The **protocol layer** has critical gaps:
- ❌ Command 0x07 (STREAM_INSTR) not implemented
- ❌ No ACK transmission after instruction writes
- ❌ No handshaking/flow control mechanism

**Fix Priority**:
1. **CRITICAL**: Add command 0x07 handler in RTL
2. **CRITICAL**: Implement ACK transmission
3. **HIGH**: Add framing error detection/reporting
4. **MEDIUM**: Consider reducing FT2232HQ latency timer
5. **LOW**: Add instruction FIFO for buffering

When you have hardware access, the instrumentation logs will show exactly where the timeout occurs, confirming this theoretical analysis.

