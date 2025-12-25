`timescale 1ns / 1ps

//==============================================================================
// DEAD SIMPLE UART LOOPBACK TEST
//==============================================================================
// This module just echoes back whatever it receives
// If this doesn't work, the problem is UART RX/TX or clock/reset
//==============================================================================

module uart_loopback_test (
    input  logic clk,      // 100MHz from W5
    input  logic rst_n,    // From R2 (SW15)
    input  logic uart_rx,  // From A18
    output logic uart_tx,  // To B18
    output logic [15:0] led // Debug LEDs
);

    // UART RX signals
    wire [7:0] rx_data;
    wire rx_valid;
    wire rx_ready;

    // UART TX signals
    reg [7:0] tx_data;
    reg tx_valid;
    wire tx_ready;

    // Instantiate UART RX
    uart_rx #(
        .CLOCK_FREQ(100_000_000),
        .BAUD_RATE(115200)
    ) rx_inst (
        .clk(clk),
        .rst_n(rst_n),
        .rx(uart_rx),
        .rx_data(rx_data),
        .rx_valid(rx_valid),
        .rx_ready(rx_ready)
    );

    // Instantiate UART TX
    uart_tx #(
        .CLOCK_FREQ(100_000_000),
        .BAUD_RATE(115200)
    ) tx_inst (
        .clk(clk),
        .rst_n(rst_n),
        .tx(uart_tx),
        .tx_data(tx_data),
        .tx_valid(tx_valid),
        .tx_ready(tx_ready)
    );

    // Simple loopback: echo received byte immediately
    // Also increment it by 1 so we can see it's actually working
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            tx_data <= 8'h00;
            tx_valid <= 1'b0;
        end else begin
            tx_valid <= 1'b0; // Default

            if (rx_valid && tx_ready) begin
                tx_data <= rx_data + 8'h01;  // Echo back byte+1
                tx_valid <= 1'b1;
            end
        end
    end

    // LED debug: show last received byte
    reg [7:0] last_rx;
    reg [7:0] rx_count;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            last_rx <= 8'h00;
            rx_count <= 8'h00;
        end else begin
            if (rx_valid) begin
                last_rx <= rx_data;
                rx_count <= rx_count + 1;
            end
        end
    end

    // LEDs show: [rx_count[7:0], last_rx[7:0]]
    assign led = {rx_count, last_rx};

endmodule
