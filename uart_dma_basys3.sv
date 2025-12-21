`timescale 1ns / 1ps

module uart_dma_basys3 #(
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
// UART RX/TX Instances
// ============================================================================

wire [7:0] rx_data;
wire rx_valid;
wire rx_ready;

uart_rx #(
    .CLOCK_FREQ(CLOCK_FREQ),
    .BAUD_RATE(BAUD_RATE)
) uart_rx_inst (
    .clk(clk),
    .rst_n(rst_n),
    .rx(uart_rx),
    .rx_data(rx_data),
    .rx_valid(rx_valid),
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

reg [7:0] state;
reg [7:0] command;
reg [7:0] addr_hi;
reg [7:0] addr_lo;
reg [15:0] length;
reg [15:0] byte_count;
reg [4:0] byte_index;  // For building up 32-byte writes

// Temporary buffers for accumulating bytes
reg [255:0] ub_buffer;
reg [63:0]  wt_buffer;
reg [31:0]  instr_buffer;

// Read buffer for sending data back
reg [255:0] read_buffer;
reg [7:0]   read_index;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= IDLE;
        command <= 8'h00;
        addr_hi <= 8'h00;
        addr_lo <= 8'h00;
        length <= 16'h0000;
        byte_count <= 16'h0000;
        byte_index <= 5'd0;

        ub_wr_en <= 1'b0;
        ub_rd_en <= 1'b0;
        wt_wr_en <= 1'b0;
        instr_wr_en <= 1'b0;
        start_execution <= 1'b0;

        tx_valid <= 1'b0;

        debug_state <= 8'h00;
        debug_cmd <= 8'h00;
        debug_byte_count <= 16'h0000;

    end else begin
        // Default: clear write enables
        ub_wr_en <= 1'b0;
        wt_wr_en <= 1'b0;
        instr_wr_en <= 1'b0;
        ub_rd_en <= 1'b0;
        start_execution <= 1'b0;
        tx_valid <= 1'b0;

        debug_state <= state;
        debug_cmd <= command;
        debug_byte_count <= byte_count;

        case (state)
            // ================================================================
            // IDLE: Wait for command byte
            // ================================================================
            IDLE: begin
                if (rx_valid) begin
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
                        default: state <= IDLE;
                    endcase
                end
            end

            // ================================================================
            // READ_ADDR_HI: Read high byte of address
            // ================================================================
            READ_ADDR_HI: begin
                if (rx_valid) begin
                    addr_hi <= rx_data;
                    state <= READ_ADDR_LO;
                end
            end

            // ================================================================
            // READ_ADDR_LO: Read low byte of address
            // ================================================================
            READ_ADDR_LO: begin
                if (rx_valid) begin
                    addr_lo <= rx_data;

                    case (command)
                        8'h01: state <= READ_LENGTH_HI;  // Write UB needs length
                        8'h02: state <= READ_LENGTH_HI;  // Write Weight needs length
                        8'h03: state <= WRITE_INSTR;     // Write Instr (fixed 4 bytes)
                        8'h04: state <= READ_LENGTH_HI;  // Read UB needs length
                        default: state <= IDLE;
                    endcase
                end
            end

            // ================================================================
            // READ_LENGTH_HI/LO: Read transfer length
            // ================================================================
            READ_LENGTH_HI: begin
                if (rx_valid) begin
                    length[15:8] <= rx_data;
                    state <= READ_LENGTH_LO;
                end
            end

            READ_LENGTH_LO: begin
                if (rx_valid) begin
                    length[7:0] <= rx_data;

                    case (command)
                        8'h01: state <= WRITE_UB;
                        8'h02: state <= WRITE_WT;
                        8'h04: begin
                            // Start read operation
                            ub_rd_en <= 1'b1;
                            ub_rd_addr <= addr_lo;
                            state <= READ_UB;
                        end
                        default: state <= IDLE;
                    endcase
                end
            end

            // ================================================================
            // WRITE_UB: Write data to Unified Buffer (32 bytes at a time)
            // ================================================================
            WRITE_UB: begin
                if (rx_valid) begin
                    // Accumulate bytes into buffer
                    ub_buffer <= {ub_buffer[247:0], rx_data};
                    byte_index <= byte_index + 1;
                    byte_count <= byte_count + 1;

                    // When we have 32 bytes, write to UB
                    if (byte_index == 5'd31) begin
                        ub_wr_en <= 1'b1;
                        ub_wr_addr <= addr_lo;
                        ub_wr_data <= {ub_buffer[247:0], rx_data};

                        addr_lo <= addr_lo + 1;  // Increment address
                        byte_index <= 5'd0;
                    end

                    // Check if done
                    if (byte_count + 1 >= length) begin
                        // If we have partial buffer, write it
                        if (byte_index != 5'd0) begin
                            ub_wr_en <= 1'b1;
                            ub_wr_addr <= addr_lo;
                            ub_wr_data <= {ub_buffer[247:0], rx_data};
                        end
                        state <= IDLE;
                    end
                end
            end

            // ================================================================
            // WRITE_WT: Write data to Weight Memory (8 bytes at a time)
            // ================================================================
            WRITE_WT: begin
                if (rx_valid) begin
                    // Accumulate bytes into buffer
                    wt_buffer <= {wt_buffer[55:0], rx_data};
                    byte_index <= byte_index + 1;
                    byte_count <= byte_count + 1;

                    // When we have 8 bytes, write to Weight Memory
                    if (byte_index == 5'd7) begin
                        wt_wr_en <= 1'b1;
                        wt_wr_addr <= {addr_hi[1:0], addr_lo};
                        wt_wr_data <= {wt_buffer[55:0], rx_data};

                        addr_lo <= addr_lo + 1;
                        if (addr_lo == 8'hFF) addr_hi <= addr_hi + 1;
                        byte_index <= 5'd0;
                    end

                    // Check if done
                    if (byte_count + 1 >= length) begin
                        // If we have partial buffer, write it
                        if (byte_index != 5'd0) begin
                            wt_wr_en <= 1'b1;
                            wt_wr_addr <= {addr_hi[1:0], addr_lo};
                            wt_wr_data <= {wt_buffer[55:0], rx_data};
                        end
                        state <= IDLE;
                    end
                end
            end

            // ================================================================
            // WRITE_INSTR: Write instruction (4 bytes)
            // ================================================================
            WRITE_INSTR: begin
                if (rx_valid) begin
                    instr_buffer <= {instr_buffer[23:0], rx_data};
                    byte_count <= byte_count + 1;

                    if (byte_count == 16'd3) begin
                        // Write complete instruction
                        instr_wr_en <= 1'b1;
                        instr_wr_addr <= addr_lo[4:0];
                        instr_wr_data <= {instr_buffer[23:0], rx_data};
                        state <= IDLE;
                    end
                end
            end

            // ================================================================
            // READ_UB: Read data from Unified Buffer and send via UART
            // ================================================================
            READ_UB: begin
                // Wait one cycle for UB to output data
                if (byte_count == 16'd0) begin
                    read_buffer <= ub_rd_data;
                    read_index <= 8'd0;
                    byte_count <= 16'd1;
                end else if (tx_ready && read_index < 8'd32) begin
                    // Send one byte at a time
                    tx_valid <= 1'b1;
                    tx_data <= read_buffer[7:0];
                    read_buffer <= {8'h00, read_buffer[255:8]};
                    read_index <= read_index + 1;

                    if (read_index == 8'd31) begin
                        // Check if more to read
                        if (byte_count >= length) begin
                            state <= IDLE;
                        end else begin
                            // Read next block
                            ub_rd_en <= 1'b1;
                            ub_rd_addr <= ub_rd_addr + 1;
                            byte_count <= byte_count + 32;
                            read_index <= 8'd0;
                        end
                    end
                end
            end

            // ================================================================
            // EXECUTE: Start TPU execution
            // ================================================================
            EXECUTE: begin
                start_execution <= 1'b1;
                state <= IDLE;
            end

            // ================================================================
            // SEND_STATUS: Send status byte back to host
            // ================================================================
            SEND_STATUS: begin
                if (tx_ready) begin
                    tx_valid <= 1'b1;
                    tx_data <= {2'b00, ub_done, ub_busy, vpu_done, vpu_busy, sys_done, sys_busy};
                    state <= IDLE;
                end
            end

            default: state <= IDLE;
        endcase
    end
end

endmodule
