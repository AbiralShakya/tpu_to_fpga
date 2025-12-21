// =============================================================================
// Normalizer Module (tinytinyTPU compatible)
// Gain/Bias/Shift normalization
// =============================================================================

`timescale 1ns / 1ps

module normalizer (
    input  logic        clk,
    input  logic        rst_n,
    input  logic        valid_in,
    input  logic signed [31:0] data_in,
    input  logic signed [15:0] gain,      // Q8.8 fixed point
    input  logic signed [31:0] bias,
    input  logic [4:0]  shift,            // Right shift amount
    output logic        valid_out,
    output logic signed [31:0] data_out
);

    // Normalization: ((data * gain) >> shift) + bias
    logic signed [47:0] mult_result;
    logic signed [31:0] scaled_result;
    logic signed [31:0] normalized_result;

    assign mult_result = data_in * gain;              // 32x16 -> 48 bits
    assign scaled_result = mult_result >>> shift;     // Right shift
    assign normalized_result = scaled_result + bias;  // Add bias

    // Pipeline stage
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            valid_out <= 1'b0;
            data_out <= 32'sd0;
        end else begin
            valid_out <= valid_in;
            data_out <= normalized_result;
        end
    end

endmodule
