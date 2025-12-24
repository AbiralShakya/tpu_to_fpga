`timescale 1ns / 1ps

// Improved UART DMA with streaming mode and ACK protocol
// Adds command 0x07 (STREAM_INSTR) and ACK mechanism

module uart_dma_basys3_improved #(
    parameter CLOCK_FREQ = 100_000_000,  // 100 MHz
    parameter BAUD_RATE  = 115200
)(
    input wire clk,
    input wire rst_n,

    // UART Interface (to PC)
    input wire  uart_rx,
    output wire uart_tx,

    // To Unified Buffer (dual-port)
    output reg        ub_wr_en,
    output reg [7:0]  ub_wr_addr,
    output reg [255:0] ub_wr_data,    // 32 bytes at once
    output reg        ub_rd_en,
    output reg [7:0]  ub_rd_addr,
    input wire [255:0] ub_rd_data,

    // To Weight Memory
    output reg        wt_wr_en,
    output reg [9:0]  wt_wr_addr,
    output reg [63:0] wt_wr_data,     // 8 bytes at once

    // To Instruction Buffer
    output reg        instr_wr_en,
    output reg [4:0]  instr_wr_addr,
    output reg [31:0] instr_wr_data,

    // To Controller
    output reg        start_execution, // Pulse to start

    // From Datapath (status)
    input wire        sys_busy,
    input wire        sys_done,
    input wire        vpu_busy,
    input wire        vpu_done,
    input wire        ub_busy,
    input wire        ub_done,

    // Debug outputs
    output reg [7:0]  debug_state,
    output reg [7:0]  debug_cmd,
    output reg [15:0] debug_byte_count
);

// ============================================================================
// UART RX/TX Instances (using improved RX with oversampling)
// ============================================================================

wire [7:0] rx_data;
wire rx_valid;
wire rx_ready;
wire rx_framing_error;

uart_rx_improved #(
    .CLOCK_FREQ(CLOCK_FREQ),
    .BAUD_RATE(BAUD_RATE)
) uart_rx_inst (
    .clk(clk),
    .rst_n(rst_n),
    .rx(uart_rx),
    .rx_data(rx_data),
    .rx_valid(rx_valid),
    .framing_error(rx_framing_error),
    .rx_ready(rx_ready)
);

reg [7:0] tx_data;
reg tx_valid;
wire tx_ready;

uart_tx #(
    .CLOCK_FREQ(CLOCK_FREQ),
    .BAUD_RATE(BAUD_RATE)
) uart_tx_inst (
    .clk(clk),
    .rst_n(rst_n),
    .tx(uart_tx),
    .tx_data(tx_data),
    .tx_valid(tx_valid),
    .tx_ready(tx_ready)
);

// ============================================================================
// Command Protocol State Machine
// ============================================================================

localparam IDLE           = 8'd0;
localparam READ_CMD       = 8'd1;
localparam READ_ADDR_HI   = 8'd2;
localparam READ_ADDR_LO   = 8'd3;
localparam READ_LENGTH_HI = 8'd4;
localparam READ_LENGTH_LO = 8'd5;
localparam WRITE_UB       = 8'd6;
localparam WRITE_WT       = 8'd7;
localparam WRITE_INSTR    = 8'd8;
localparam READ_UB        = 8'd9;
localparam SEND_STATUS    = 8'd10;
localparam EXECUTE        = 8'd11;
localparam STREAM_MODE    = 8'd12;  // NEW: Streaming mode state

reg [7:0] state;
reg [7:0] command;
reg [7:0] addr_hi, addr_lo;
reg [15:0] length;
reg [15:0] byte_count;
reg [5:0] byte_index;
reg [23:0] instr_buffer;
reg [255:0] read_buffer;
reg [7:0] read_index;

// Streaming mode registers
reg stream_mode_active;
reg [4:0] instr_buffer_count;  // Track how many instructions buffered

// ============================================================================
// State Machine
// ============================================================================

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= IDLE;
        command <= 8'h00;
        addr_hi <= 8'h00;
        addr_lo <= 8'h00;
        length <= 16'h0000;
        byte_count <= 16'h0000;
        byte_index <= 6'd0;
        instr_buffer <= 24'h000000;
        read_buffer <= 256'h0;
        read_index <= 8'd0;
        
        ub_wr_en <= 1'b0;
        ub_wr_addr <= 8'h00;
        ub_wr_data <= 256'h0;
        ub_rd_en <= 1'b0;
        ub_rd_addr <= 8'h00;
        
        wt_wr_en <= 1'b0;
        wt_wr_addr <= 10'h000;
        wt_wr_data <= 64'h0;
        
        instr_wr_en <= 1'b0;
        instr_wr_addr <= 5'd0;
        instr_wr_data <= 32'h0;
        
        start_execution <= 1'b0;
        
        tx_valid <= 1'b0;
        tx_data <= 8'h00;
        
        stream_mode_active <= 1'b0;
        instr_buffer_count <= 5'd0;
        
        debug_state <= 8'd0;
        debug_cmd <= 8'd0;
        debug_byte_count <= 16'h0000;
    end else begin
        // Defaults
        ub_wr_en <= 1'b0;
        ub_rd_en <= 1'b0;
        wt_wr_en <= 1'b0;
        instr_wr_en <= 1'b0;
        start_execution <= 1'b0;
        tx_valid <= 1'b0;

        debug_state <= state;
        debug_cmd <= command;
        debug_byte_count <= byte_count;

        case (state)
            IDLE: begin
                byte_count <= 16'h0000;
                byte_index <= 6'd0;
                read_index <= 8'd0;

                if (rx_valid && !rx_framing_error) begin
                    command <= rx_data;
                    byte_count <= 16'h0000;
                    byte_index <= 5'd0;

                    case (rx_data)
                        8'h01: state <= READ_ADDR_HI;  // Write UB
                        8'h02: state <= READ_ADDR_HI;  // Write Weight
                        8'h03: state <= READ_ADDR_HI;  // Write Instruction
                        8'h04: state <= READ_ADDR_HI;  // Read UB
                        8'h05: state <= EXECUTE;       // Start execution
                        8'h06: state <= SEND_STATUS;   // Read status
                        8'h07: begin  // NEW: Stream mode command
                            stream_mode_active <= 1'b1;
                            instr_buffer_count <= 5'd0;
                            // Send response: 0x00=ready, 0xFF=not ready, 0x01=buffer select
                            if (tx_ready) begin
                                tx_valid <= 1'b1;
                                if (instr_buffer_count >= 5'd31) begin
                                    tx_data <= 8'hFF;  // Buffer full
                                end else begin
                                    tx_data <= 8'h00;  // Ready
                                end
                            end
                            state <= STREAM_MODE;
                        end
                        default: state <= IDLE;
                    endcase
                end
            end

            // ... (other states remain similar, but WRITE_INSTR needs ACK) ...
            
            WRITE_INSTR: begin
                if (rx_valid && !rx_framing_error) begin
                    instr_buffer <= {instr_buffer[23:0], rx_data};
                    byte_count <= byte_count + 1;

                    if (byte_count == 16'd3) begin
                        // Write complete instruction
                        instr_wr_en <= 1'b1;
                        instr_wr_addr <= addr_lo[4:0];
                        instr_wr_data <= {instr_buffer[23:0], rx_data};
                        
                        // NEW: Send ACK
                        if (tx_ready) begin
                            tx_valid <= 1'b1;
                            tx_data <= 8'h00;  // Success
                        end
                        
                        state <= IDLE;
                    end
                end
            end

            // NEW: Streaming mode state
            STREAM_MODE: begin
                if (!stream_mode_active) begin
                    state <= IDLE;
                end else if (rx_valid && !rx_framing_error) begin
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

            // ... (other states) ...
            
            default: state <= IDLE;
        endcase
    end
end

endmodule

