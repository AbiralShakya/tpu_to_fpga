// =============================================================================
// Activation Function Module (tinytinyTPU compatible)
// =============================================================================

`timescale 1ns / 1ps

module activation_func (
    input  logic        clk,
    input  logic        rst_n,
    input  logic        valid_in,
    input  logic signed [31:0] data_in,
    output logic        valid_out,
    output logic signed [31:0] data_out
);

    // Pipeline stage for timing
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            valid_out <= 1'b0;
            data_out <= 32'sd0;
        end else begin
            valid_out <= valid_in;
            // ReLU activation: max(0, x)
            data_out <= (data_in > 0) ? data_in : 32'sd0;
        end
    end

endmodule
