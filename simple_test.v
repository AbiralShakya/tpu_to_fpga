// =============================================================================
// Simple Test for TPU Components
// =============================================================================

`timescale 1ns / 1ps

module simple_test;

// Clock and reset
reg clk = 0;
reg rst_n = 0;

// Generate clock
always #5 clk = ~clk;

// Test signals for PE
reg [7:0] act_in = 0;
reg [7:0] weight_in = 0;
reg [31:0] psum_in = 0;
reg en_weight_pass = 0;
reg en_weight_capture = 0;

wire [7:0] act_out;
wire [31:0] psum_out;

// Instantiate a simple PE (without DSP48E1)
pe_simple pe_inst (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_weight_capture),
    .weight_in(weight_in),
    .act_in(act_in),
    .psum_in(psum_in),
    .act_out(act_out),
    .psum_out(psum_out)
);

// Test sequence
initial begin
    $display("Starting simple TPU PE test...");
    $dumpfile("simple_test.vcd");
    $dumpvars(0, simple_test);

    // Reset
    rst_n = 0;
    #10;
    rst_n = 1;
    #10;

    // Load weight
    weight_in = 8'h05;  // Weight = 5
    en_weight_capture = 1;
    #10;
    en_weight_capture = 0;
    #10;

    // Test MAC operation
    act_in = 8'h0A;     // Activation = 10
    psum_in = 32'h0000; // Previous sum = 0
    en_weight_pass = 1;
    #10;

    $display("Input: act=%d, weight=%d, psum_in=%d", act_in, weight_in, psum_in);
    $display("Output: act_out=%d, psum_out=%d", act_out, psum_out);
    $display("Expected: 10 * 5 + 0 = 50");

    // Test another operation
    act_in = 8'h03;     // Activation = 3
    psum_in = 32'h0020; // Previous sum = 32
    #10;

    $display("Input: act=%d, weight=%d, psum_in=%d", act_in, weight_in, psum_in);
    $display("Output: act_out=%d, psum_out=%d", act_out, psum_out);
    $display("Expected: 3 * 5 + 32 = 47");

    #10;
    $finish;
end

endmodule

// Simple PE implementation without Xilinx primitives
module pe_simple (
    input clk,
    input rst_n,
    input en_weight_pass,
    input en_weight_capture,
    input [7:0] weight_in,
    input [7:0] act_in,
    input [31:0] psum_in,
    output reg [7:0] act_out,
    output reg [31:0] psum_out
);

    reg [7:0] weight_reg;

    // Weight register
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            weight_reg <= 8'b0;
        end else if (en_weight_capture) begin
            weight_reg <= weight_in;
        end
    end

    // MAC operation: psum_out = psum_in + (act_in * weight_reg)
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            psum_out <= 32'b0;
            act_out <= 8'b0;
        end else if (en_weight_pass) begin
            psum_out <= psum_in + (act_in * weight_reg);
            act_out <= act_in;
        end
    end

endmodule
