`timescale 1ns / 1ps

module uart_rx #(
    parameter CLOCK_FREQ = 100_000_000,
    parameter BAUD_RATE  = 115200
)(
    input wire clk,
    input wire rst_n,
    input wire rx,
    output reg [7:0] rx_data,
    output reg rx_valid,
    output wire rx_ready
);

localparam CLKS_PER_BIT = CLOCK_FREQ / BAUD_RATE;
localparam HALF_BIT = CLKS_PER_BIT / 2;

localparam IDLE  = 2'd0;
localparam START = 2'd1;
localparam DATA  = 2'd2;
localparam STOP  = 2'd3;

reg [1:0] state;
reg [15:0] clk_count;
reg [2:0] bit_index;
reg [7:0] rx_byte;

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
        bit_index <= 3'd0;
        rx_data <= 8'd0;
        rx_valid <= 1'b0;
        rx_byte <= 8'd0;
    end else begin
        rx_valid <= 1'b0;  // Default

        case (state)
            IDLE: begin
                clk_count <= 16'd0;
                bit_index <= 3'd0;

                if (rx_sync_2 == 1'b0) begin  // Start bit detected
                    state <= START;
                end
            end

            START: begin
                if (clk_count == HALF_BIT - 1) begin
                    if (rx_sync_2 == 1'b0) begin  // Confirm start bit
                        clk_count <= 16'd0;
                        state <= DATA;
                    end else begin
                        state <= IDLE;  // False start
                    end
                end else begin
                    clk_count <= clk_count + 1;
                end
            end

            DATA: begin
                if (clk_count == CLKS_PER_BIT - 1) begin
                    clk_count <= 16'd0;
                    rx_byte[bit_index] <= rx_sync_2;

                    if (bit_index == 3'd7) begin
                        bit_index <= 3'd0;
                        state <= STOP;
                    end else begin
                        bit_index <= bit_index + 1;
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
