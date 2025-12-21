// =============================================================================
// Loss Computation Block (tinytinyTPU compatible)
// L1 loss: |predicted - target|
// =============================================================================

`timescale 1ns / 1ps

module loss_block (
    input  logic        clk,
    input  logic        rst_n,
    input  logic        valid_in,
    input  logic signed [31:0] data_in,    // Predicted value
    input  logic signed [31:0] target_in,  // Target value
    output logic        valid_out,
    output logic signed [31:0] loss_out    // L1 loss
);

    // L1 loss: |predicted - target|
    logic signed [31:0] diff;
    logic signed [31:0] abs_diff;

    assign diff = data_in - target_in;
    assign abs_diff = (diff >= 0) ? diff : -diff;

    // Pipeline stage
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            valid_out <= 1'b0;
            loss_out <= 32'sd0;
        end else begin
            valid_out <= valid_in;
            loss_out <= abs_diff;
        end
    end

endmodule
