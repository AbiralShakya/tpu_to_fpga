# UART Protocol Analysis: Handshaking Issues

## Executive Summary

After comparing your UART implementation with Basys3 reference designs and analyzing the RTL code, **I've identified a critical protocol mismatch** that explains why the handshaking doesn't work:

### 🔴 **CRITICAL ISSUE: Command 0x07 (STREAM_INSTR) Is Not Implemented in the RTL**

Your Python driver uses `UARTCommand.STREAM_INSTR = 0x07` for streaming mode, but this command **does not exist** in the FPGA's `uart_dma_basys3.sv` module.

---

## Current RTL Implementation (`uart_dma_basys3.sv`)

### Supported Commands (lines 164-171):
```systemverilog
case (rx_data)
    8'h01: state <= READ_ADDR_HI;  // Write UB
    8'h02: state <= READ_ADDR_HI;  // Write Weight
    8'h03: state <= READ_ADDR_HI;  // Write Instruction
    8'h04: state <= READ_ADDR_HI;  // Read UB
    8'h05: state <= EXECUTE;       // Start execution
    8'h06: state <= SEND_STATUS;   // Read status
    default: state <= IDLE;
endcase
```

### What's Missing:
- ❌ **No 0x07 (STREAM_INSTR) command**
- ❌ **No acknowledgment protocol** for streamed instructions
- ❌ **No double-buffering logic** for instruction streaming
- ❌ **No backpressure/flow control** mechanism

---

## Python Driver Expectations (`tpu_coprocessor_driver.py`)

### Command Definitions (lines 48-56):
```python
class UARTCommand(IntEnum):
    WRITE_UB      = 0x01  # Write to Unified Buffer
    WRITE_WT      = 0x02  # Write to Weight Memory
    WRITE_INSTR   = 0x03  # Write instruction to buffer
    READ_UB       = 0x04  # Read from Unified Buffer
    EXECUTE       = 0x05  # Start execution
    READ_STATUS   = 0x06  # Read TPU status
    STREAM_INSTR  = 0x07  # ⚠️ NOT IMPLEMENTED IN RTL!
```

### Streaming Mode Protocol (Driver's Expectation):
1. **Enable Stream Mode (line 353-409)**:
   - Send: `0x07` command
   - Expect: Response byte (`0x00`=ready, `0xFF`=not ready, `0x01`=buffer select)
   
2. **Stream Each Instruction (line 417-524)**:
   - Send: 4 bytes (instruction)
   - Expect: ACK byte (`0x00`=accepted, `0xFF`=buffer full, `0x01`=buffer swapped)

3. **Continuous Streaming**:
   - Send instruction → Read ACK → Send next instruction → Read ACK...

---

## Why the Handshaking Fails

### Problem 1: Unrecognized Command
When Python sends `0x07`, the FPGA:
1. Receives `0x07` in the `IDLE` state
2. Hits the `default` case (line 171)
3. **Stays in IDLE and does nothing**
4. Never sends a response byte

Result: Python waits for a response that never comes → **timeout**

### Problem 2: No ACK Protocol in Write Instruction Mode
Even if you use command `0x03` (WRITE_INSTR) for streaming:
1. The RTL **never sends acknowledgment bytes** back to the host
2. The `WRITE_INSTR` state (lines 301-314) only **receives** data, never **transmits**
3. Python's `stream_instruction()` will timeout waiting for ACK

### Problem 3: Timing Mismatch
The current `WRITE_INSTR` state:
- Expects exactly 4 bytes after the address
- Immediately returns to `IDLE` after receiving all 4 bytes
- **No feedback mechanism** to tell the host it's ready for the next instruction

---

## Comparison with Reference Designs

### Standard UART Handshaking Patterns:

#### Pattern 1: Command-Response (What You Need)
```
Host → FPGA: [CMD] [DATA...]
FPGA → Host: [ACK/NAK]
Host → FPGA: [NEXT_CMD] [DATA...]
FPGA → Host: [ACK/NAK]
```

#### Pattern 2: XON/XOFF Flow Control
```
Host → FPGA: [DATA] [DATA] [DATA]...
FPGA → Host: [XOFF] (buffer full)
Host: (pauses)
FPGA → Host: [XON] (buffer ready)
Host → FPGA: [DATA] [DATA]...
```

#### Pattern 3: RTS/CTS Hardware Flow Control
- Uses dedicated hardware pins
- Not applicable to your single-wire UART

### Your Current Implementation:
```
Host → FPGA: [CMD] [DATA...]
FPGA → Host: (nothing)
Host → FPGA: [NEXT_CMD] [DATA...]
FPGA → Host: (nothing)
```
→ **No handshaking at all!**

---

## Root Causes (Ranked by Impact)

### 1. **Protocol Design Mismatch** (CRITICAL)
- Python driver implements a command (0x07) that doesn't exist in RTL
- Python expects ACK bytes that RTL never sends
- **Impact**: 100% failure rate for streaming mode

### 2. **Missing Acknowledgment Logic** (HIGH)
- RTL has no `tx_valid` assertion in instruction write states
- No mechanism to signal "ready for next instruction"
- **Impact**: Even with correct command, no feedback loop

### 3. **Buffer Management Issues** (MEDIUM)
- RTL has 32-entry instruction buffer but no occupancy tracking
- No way to tell host when buffer is full
- **Impact**: Potential instruction loss if sending too fast

### 4. **Timing and Synchronization** (LOW)
- Your UART RX/TX modules look correct (proper synchronizers on lines 29-33 of `uart_rx.sv`)
- Baud rate calculation is correct: `100MHz / 115200 = 868 clocks/bit`
- **Impact**: Unlikely to be the primary issue

---

## Specific Issues Compared to Basys3 Reference Designs

### Your Implementation vs. Standard Practice:

| Aspect | Your RTL | Standard Basys3 | Impact |
|--------|----------|-----------------|--------|
| **Command Set** | 0x01-0x06 | Usually includes echo/loopback | Missing 0x07 breaks streaming |
| **ACK Protocol** | ❌ None | ✅ Send status after each command | Python times out |
| **Flow Control** | ❌ None | ✅ Hardware RTS/CTS or SW XON/XOFF | Can't handle backpressure |
| **Buffer Status** | ❌ Not reported | ✅ Status register with FIFO flags | No visibility into buffer state |
| **Error Handling** | ❌ Silent failures | ✅ NAK or error codes | Can't detect/recover from errors |

---

## Detailed Evidence from Code

### Evidence 1: No Response in WRITE_INSTR State
```systemverilog:rtl/uart_dma_basys3.sv
301:            WRITE_INSTR: begin
302:                if (rx_valid) begin
303:                    instr_buffer <= {instr_buffer[23:0], rx_data};
304:                    byte_count <= byte_count + 1;
305|
306:                    if (byte_count == 16'd3) begin
307:                        // Write complete instruction
308:                        instr_wr_en <= 1'b1;
309:                        instr_wr_addr <= addr_lo[4:0];
310:                        instr_wr_data <= {instr_buffer[23:0], rx_data};
311:                        state <= IDLE;  // ← Goes back to IDLE without sending ACK
312:                    end
313:                end
314:            end
```
**Problem**: Line 311 goes to IDLE without setting `tx_valid = 1'b1` or `tx_data = ACK_BYTE`.

### Evidence 2: Only SEND_STATUS and READ_UB Send Data
```systemverilog:rtl/uart_dma_basys3.sv
358:            SEND_STATUS: begin
359:                if (tx_ready) begin
360:                    tx_valid <= 1'b1;  // ← Only place outside READ_UB where tx_valid is set
361:                    tx_data <= {2'b00, ub_done, ub_busy, vpu_done, vpu_busy, sys_done, sys_busy};
362:                    state <= IDLE;
363:                end
364:            end
```

### Evidence 3: Python Driver Expects ACK After Each Instruction
```python:python/drivers/tpu_coprocessor_driver.py
452:        # Send instruction (4 bytes, big-endian)
453:        instr_bytes = instr.to_bytes()
457:        self.ser.write(instr_bytes)
458:        self.stats['bytes_sent'] += 4
459:        
460:        # Read acknowledgment: 0x00=accepted, 0xFF=full, 0x01=buffer swapped
464:        response = self._read_response(1)  # ← WILL TIMEOUT because FPGA never sends this
```

---

## Recommended Fixes (Prioritized)

### Option 1: Fix the RTL to Support Streaming Mode (BEST)

Add streaming mode support to `uart_dma_basys3.sv`:

```systemverilog
// New state
localparam STREAM_MODE = 8'd12;

// New registers
reg stream_mode_active;
reg [4:0] instr_buffer_count;  // Track how many instructions buffered

// In command decoder:
case (rx_data)
    // ... existing commands ...
    8'h07: begin
        stream_mode_active <= 1'b1;
        state <= STREAM_MODE;
    end
endcase

// New streaming state:
STREAM_MODE: begin
    if (!stream_mode_active) begin
        state <= IDLE;
    end else if (rx_valid) begin
        // Receive 4-byte instruction
        instr_buffer <= {instr_buffer[23:0], rx_data};
        byte_count <= byte_count + 1;
        
        if (byte_count == 16'd3) begin
            // Write instruction
            instr_wr_en <= 1'b1;
            instr_wr_addr <= instr_buffer_count;
            instr_wr_data <= {instr_buffer[23:0], rx_data};
            instr_buffer_count <= instr_buffer_count + 1;
            byte_count <= 16'd0;
            
            // Send ACK
            if (tx_ready) begin
                tx_valid <= 1'b1;
                if (instr_buffer_count >= 5'd31) begin
                    tx_data <= 8'hFF;  // Buffer full
                end else begin
                    tx_data <= 8'h00;  // Accepted
                end
            end
        end
    end
end
```

### Option 2: Modify Python Driver to Match Existing RTL (QUICK FIX)

Disable streaming mode and use command `0x03` in a loop:

```python
def stream_instructions_legacy(self, instructions):
    """Stream instructions using repeated WRITE_INSTR commands"""
    for idx, instr in enumerate(instructions):
        # Send command 0x03 for each instruction
        self._send_command(UARTCommand.WRITE_INSTR, 0, idx)
        self.ser.write(instr.to_bytes())
        
        # Poll status to ensure it's been written
        time.sleep(0.001)  # Small delay for FPGA to process
```

**Caveat**: This is slow and has no flow control, but it will work with current RTL.

### Option 3: Add Minimal ACK to Existing Commands (COMPROMISE)

Modify WRITE_INSTR state to send ACK:

```systemverilog
WRITE_INSTR: begin
    if (rx_valid) begin
        instr_buffer <= {instr_buffer[23:0], rx_data};
        byte_count <= byte_count + 1;

        if (byte_count == 16'd3) begin
            // Write complete instruction
            instr_wr_en <= 1'b1;
            instr_wr_addr <= addr_lo[4:0];
            instr_wr_data <= {instr_buffer[23:0], rx_data};
            
            // Send ACK (NEW)
            if (tx_ready) begin
                tx_valid <= 1'b1;
                tx_data <= 8'h00;  // Success
            end
            
            state <= IDLE;
        end
    end
end
```

Then modify Python to use command `0x03` but expect ACK:

```python
def stream_instruction_with_ack(self, instr, addr):
    """Stream using WRITE_INSTR (0x03) with ACK"""
    self._send_command(UARTCommand.WRITE_INSTR, 0, addr)
    self.ser.write(instr.to_bytes())
    
    # Now this will work because RTL sends ACK
    ack = self._read_response(1)
    return len(ack) > 0 and ack[0] == 0x00
```

---

## Testing Recommendations (When You Have Hardware)

### Test 1: Basic Command Echo
Add a loopback test command (0x08) that just echoes back what it receives:
```systemverilog
8'h08: begin  // Echo test
    if (tx_ready) begin
        tx_valid <= 1'b1;
        tx_data <= rx_data;  // Echo back
        state <= IDLE;
    end
end
```

### Test 2: Buffer Status Visibility
Modify SEND_STATUS to include instruction buffer count:
```systemverilog
tx_data <= {instr_buffer_count[4:2], instr_wr_addr[4:0]};
```

### Test 3: Incremental Validation
1. ✅ Test baud rate with loopback (0x08 echo command)
2. ✅ Test WRITE_UB with readback (commands 0x01, 0x04)
3. ✅ Test WRITE_INSTR with status check (commands 0x03, 0x06)
4. ✅ Only then enable streaming mode

---

## Additional Issues Found

### Issue 1: Instruction Buffer Addressing
```systemverilog:309
instr_wr_addr <= addr_lo[4:0];
```
- Instruction address comes from command packet (addr_lo)
- For streaming, should auto-increment
- **Fix**: Use internal counter instead of addr_lo for streaming mode

### Issue 2: No FIFO for Instructions
- Current design: 32-entry register array (assumed)
- Streaming workload: Continuous flow
- **Improvement**: Use true FIFO with full/empty flags

### Issue 3: Read Buffer Logic Issue (Minor)
```systemverilog:325-330
read_buffer <= {8'h00, read_buffer[255:8]};  // Right shift
```
- This shifts in zeros at the top
- **Should verify**: Does UB read always provide 32 bytes?

---

## Summary of Required Changes

### For Full Streaming Support:

| Component | Change Required | Priority |
|-----------|----------------|----------|
| RTL: uart_dma_basys3.sv | Add 0x07 command handler | CRITICAL |
| RTL: uart_dma_basys3.sv | Add ACK transmission logic | CRITICAL |
| RTL: uart_dma_basys3.sv | Add buffer occupancy tracking | HIGH |
| RTL: uart_dma_basys3.sv | Add streaming state machine | HIGH |
| Python: tpu_coprocessor_driver.py | (No changes needed if RTL fixed) | N/A |

### For Quick Workaround:

| Component | Change Required | Priority |
|-----------|----------------|----------|
| Python: tpu_coprocessor_driver.py | Remove streaming mode calls | CRITICAL |
| Python: tpu_coprocessor_driver.py | Use loop with 0x03 + delays | CRITICAL |
| Python: test_all_instructions.py | Remove enable_stream_mode() | CRITICAL |

---

## Comparison with Other Projects

### Example 1: GitHub pabennett/uart (VHDL)
- **What they do**: Implement loopback by immediately echoing RX data to TX
- **Lesson**: Always provide immediate feedback to host

### Example 2: GitHub jakubcabal/uart-for-fpga
- **What they do**: Provide `rx_valid` and `tx_ready` handshake signals
- **Lesson**: Your low-level UART modules already do this correctly! The problem is at the protocol layer above.

### Example 3: OpenCores Serial UART
- **What they do**: Provide interrupt flags and status registers
- **Lesson**: Polling status works, but you need to actually read it between commands

---

## Conclusion

The handshaking failure is **not a hardware issue** with your UART modules (which look correct), but rather a **protocol design mismatch** between Python and RTL:

1. ✅ **UART RX/TX modules**: Correct (proper baud rate, synchronizers, state machines)
2. ❌ **Command protocol**: Missing 0x07, no ACK mechanism
3. ❌ **Flow control**: No backpressure signaling
4. ❌ **Streaming mode**: Not implemented in RTL despite being in Python

**Bottom line**: Choose Option 1 (add streaming to RTL) for best performance, or Option 2 (disable streaming in Python) for quickest workaround.

---

## Next Steps

1. **Decision**: Choose fix strategy (RTL or Python)
2. **Implement**: Make changes to chosen component
3. **Test**: When hardware available, validate with incremental tests
4. **Monitor**: Add debug visibility (debug_state, debug_cmd outputs)

When you're ready to implement a fix, I can provide the complete modified code for either option.

