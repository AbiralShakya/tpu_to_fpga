# UART Implementation Improvements Based on Reference Designs

## Reference Projects Analyzed

1. **Shubhayu-Das/UART-basys3** (https://github.com/Shubhayu-Das/UART-basys3)
   - Uses 15x oversampling for RX (samples middle 3 locations, majority vote)
   - Timing is "very fiddly" - changing baud rate breaks everything
   - Works at 921600 baud (not standard 115200)
   - Uses separate RX/TX modules with controller

2. **ECEN220 Lab-09 Exercise 3** (https://ecen220wiki.groups.et.byu.net/labs/lab-09/#exercise-3---top-level-tx-module)
   - Standard UART implementation patterns
   - Proper state machine design

## Current Implementation Analysis

### Your UART RX Module (`rtl/uart_rx.sv`)
- ✅ **Good**: Two-stage synchronizer (lines 29-33)
- ✅ **Good**: Start bit detection at mid-bit (HALF_BIT sampling)
- ⚠️ **Issue**: Only samples once per bit (no oversampling)
- ⚠️ **Issue**: No majority voting for noise immunity
- ⚠️ **Issue**: No framing error detection/reporting

### Your UART TX Module (`rtl/uart_tx.sv`)
- ✅ **Good**: Simple, clean state machine
- ✅ **Good**: Proper idle high state
- ⚠️ **Issue**: No timing validation
- ⚠️ **Issue**: No parity support (though not needed for 8N1)

### Your UART Protocol Module (`rtl/uart_dma_basys3.sv`)
- ❌ **Critical**: Command 0x07 (STREAM_INSTR) not implemented
- ❌ **Critical**: No ACK mechanism for instruction writes
- ⚠️ **Issue**: No flow control
- ⚠️ **Issue**: No error handling

## Recommended Improvements

### 1. Add Oversampling to RX (High Priority)

Based on Shubhayu-Das implementation, add 15x oversampling:

```systemverilog
// Sample each bit 15 times, use middle 3 samples for majority vote
localparam OVERSAMPLE = 15;
localparam SAMPLE_START = 6;  // Middle 3 samples: 6, 7, 8
localparam SAMPLE_END = 8;

reg [3:0] sample_count;
reg [2:0] sample_buffer;

// In DATA state:
if (sample_count < OVERSAMPLE) begin
    sample_count <= sample_count + 1;
    if (sample_count >= SAMPLE_START && sample_count <= SAMPLE_END) begin
        sample_buffer[sample_count - SAMPLE_START] <= rx_sync_2;
    end
end else begin
    // Majority vote
    rx_byte[bit_index] <= (sample_buffer[0] + sample_buffer[1] + sample_buffer[2] >= 2);
    sample_count <= 0;
    // ... advance bit_index
end
```

**Benefits**:
- Noise immunity
- Clock drift tolerance
- More robust at higher baud rates

### 2. Add Framing Error Detection

```systemverilog
output reg framing_error;

// In STOP state:
if (rx_sync_2 == 1'b1) begin
    rx_data <= rx_byte;
    rx_valid <= 1'b1;
    framing_error <= 1'b0;
end else begin
    framing_error <= 1'b1;  // Stop bit not HIGH
    rx_valid <= 1'b0;
end
```

### 3. Fix Protocol Issues (Critical)

#### Add Command 0x07 (STREAM_INSTR)

```systemverilog
localparam STREAM_MODE = 8'd12;

// In command decoder:
8'h07: begin
    stream_mode_active <= 1'b1;
    state <= STREAM_MODE;
    // Send response: 0x00=ready, 0xFF=not ready
    if (tx_ready) begin
        tx_valid <= 1'b1;
        tx_data <= (instr_buffer_count < 5'd31) ? 8'h00 : 8'hFF;
    end
end
```

#### Add ACK to WRITE_INSTR

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
            
            // Send ACK
            if (tx_ready) begin
                tx_valid <= 1'b1;
                tx_data <= 8'h00;  // Success
            end
            
            state <= IDLE;
        end
    end
end
```

### 4. Improve Timing Robustness

#### Use Configurable Baud Rate Generator

```systemverilog
// Calculate exact clock divider
localparam CLKS_PER_BIT = CLOCK_FREQ / BAUD_RATE;
localparam CLKS_PER_BIT_REMAINDER = CLOCK_FREQ % BAUD_RATE;

// Use accumulator for fractional timing
reg [31:0] baud_acc;
always @(posedge clk) begin
    if (state != IDLE) begin
        baud_acc <= baud_acc + {16'd0, CLKS_PER_BIT_REMAINDER};
        if (baud_acc >= BAUD_RATE) begin
            baud_acc <= baud_acc - BAUD_RATE;
            // Advance bit timing
        end
    end
end
```

### 5. Add Flow Control

#### XON/XOFF Support

```systemverilog
localparam XON  = 8'h11;
localparam XOFF = 8'h13;

// In RX handler:
if (rx_data == XOFF) begin
    flow_control_pause <= 1'b1;
end else if (rx_data == XON) begin
    flow_control_pause <= 1'b0;
end
```

## Comparison with Reference Implementation

| Feature | Your Implementation | Shubhayu-Das | Recommended |
|---------|---------------------|--------------|-------------|
| **Oversampling** | ❌ None | ✅ 15x | ✅ 15x with majority vote |
| **Noise Immunity** | ⚠️ Basic | ✅ High | ✅ High |
| **Framing Error** | ❌ None | ⚠️ Implicit | ✅ Explicit flag |
| **Baud Rate Flexibility** | ⚠️ Fixed | ❌ Fiddly | ✅ Configurable |
| **ACK Protocol** | ❌ Missing | ⚠️ Basic | ✅ Full handshaking |
| **Flow Control** | ❌ None | ❌ None | ✅ XON/XOFF |

## Implementation Priority

1. **CRITICAL**: Add command 0x07 and ACK mechanism (blocks streaming mode)
2. **HIGH**: Add oversampling to RX (improves reliability)
3. **HIGH**: Add framing error detection (debugging aid)
4. **MEDIUM**: Improve baud rate timing (flexibility)
5. **LOW**: Add flow control (nice to have)

## Testing Recommendations

Based on Shubhayu-Das project:

1. **Test at different baud rates**: 115200, 230400, 460800, 921600
2. **Test with noise**: Add intentional bit errors
3. **Test flow control**: Send data faster than processing
4. **Test framing errors**: Mismatched baud rates
5. **Test ACK protocol**: Verify handshaking works

## Python Script Improvements

Based on the reference Python script pattern:

```python
# Add retry logic
def send_with_retry(self, data, max_retries=3):
    for attempt in range(max_retries):
        try:
            self.ser.write(data)
            self.ser.flush()  # Force USB transfer
            return True
        except Exception as e:
            if attempt == max_retries - 1:
                raise
            time.sleep(0.01)
    return False

# Add latency compensation
def compensate_latency(self):
    # FT2232HQ has 16ms latency for small packets
    if self.ser.in_waiting == 0:
        time.sleep(0.016)  # Wait for USB latency
```

## Next Steps

1. Implement command 0x07 and ACK mechanism
2. Add oversampling to RX module
3. Test with hardware at 115200 baud
4. Gradually increase baud rate and test
5. Add comprehensive error handling

