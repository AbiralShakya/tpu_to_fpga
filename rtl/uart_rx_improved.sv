`timescale 1ns / 1ps

// Improved UART RX with oversampling for noise immunity
// Based on Shubhayu-Das/UART-basys3 implementation
// Uses 15x oversampling with majority vote on middle 3 samples

module uart_rx_improved #(
    parameter CLOCK_FREQ = 100_000_000,
    parameter BAUD_RATE  = 115200,
    parameter OVERSAMPLE = 15  // Sample each bit 15 times
)(
    input wire clk,
    input wire rst_n,
    input wire rx,
    output reg [7:0] rx_data,
    output reg rx_valid,
    output reg framing_error,
    output wire rx_ready
);

localparam CLKS_PER_BIT = CLOCK_FREQ / BAUD_RATE;
localparam CLKS_PER_SAMPLE = CLKS_PER_BIT / OVERSAMPLE;
localparam HALF_BIT = CLKS_PER_BIT / 2;

// Sample middle 3 locations for majority vote (samples 6, 7, 8 out of 15)
localparam SAMPLE_START = 6;
localparam SAMPLE_END = 8;

localparam IDLE  = 2'd0;
localparam START = 2'd1;
localparam DATA  = 2'd2;
localparam STOP  = 2'd3;

reg [1:0] state;
reg [15:0] clk_count;
reg [3:0] sample_count;  // Count samples within a bit period
reg [2:0] bit_index;
reg [7:0] rx_byte;
reg [2:0] sample_buffer;  // Store middle 3 samples for majority vote

// Synchronize RX input (prevent metastability)
reg rx_sync_1, rx_sync_2;
always @(posedge clk) begin
    rx_sync_1 <= rx;
    rx_sync_2 <= rx_sync_1;
end

assign rx_ready = (state == IDLE);

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= IDLE;
        clk_count <= 16'd0;
        sample_count <= 4'd0;
        bit_index <= 3'd0;
        rx_data <= 8'd0;
        rx_valid <= 1'b0;
        framing_error <= 1'b0;
        rx_byte <= 8'd0;
        sample_buffer <= 3'd0;
    end else begin
        rx_valid <= 1'b0;  // Default
        framing_error <= 1'b0;  // Default

        case (state)
            IDLE: begin
                clk_count <= 16'd0;
                sample_count <= 4'd0;
                bit_index <= 3'd0;

                if (rx_sync_2 == 1'b0) begin  // Start bit detected
                    state <= START;
                    clk_count <= 16'd0;
                end
            end

            START: begin
                // Wait until mid-bit to confirm start bit
                if (clk_count == HALF_BIT - 1) begin
                    if (rx_sync_2 == 1'b0) begin  // Confirm start bit
                        clk_count <= 16'd0;
                        sample_count <= 4'd0;
                        state <= DATA;
                    end else begin
                        state <= IDLE;  // False start
                    end
                end else begin
                    clk_count <= clk_count + 1;
                end
            end

            DATA: begin
                // Sample at CLKS_PER_SAMPLE intervals
                if (clk_count == CLKS_PER_SAMPLE - 1) begin
                    clk_count <= 16'd0;
                    
                    // Store middle 3 samples
                    if (sample_count >= SAMPLE_START && sample_count <= SAMPLE_END) begin
                        sample_buffer[sample_count - SAMPLE_START] <= rx_sync_2;
                    end
                    
                    sample_count <= sample_count + 1;
                    
                    // After all samples, do majority vote
                    if (sample_count == OVERSAMPLE - 1) begin
                        // Majority vote: if 2 or 3 samples are 1, bit is 1
                        rx_byte[bit_index] <= (sample_buffer[0] + sample_buffer[1] + sample_buffer[2] >= 2);
                        sample_count <= 4'd0;

                        if (bit_index == 3'd7) begin
                            bit_index <= 3'd0;
                            state <= STOP;
                        end else begin
                            bit_index <= bit_index + 1;
                        end
                    end
                end else begin
                    clk_count <= clk_count + 1;
                end
            end

            STOP: begin
                if (clk_count == CLKS_PER_BIT - 1) begin
                    clk_count <= 16'd0;
                    if (rx_sync_2 == 1'b1) begin  // Valid stop bit
                        rx_data <= rx_byte;
                        rx_valid <= 1'b1;
                        framing_error <= 1'b0;
                    end else begin
                        // Framing error: stop bit not HIGH
                        framing_error <= 1'b1;
                        rx_valid <= 1'b0;
                    end
                    state <= IDLE;
                end else begin
                    clk_count <= clk_count + 1;
                end
            end

            default: state <= IDLE;
        endcase
    end
end

endmodule

