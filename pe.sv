`timescale 1ns / 1ps

module pe #(
    parameter DATA_WIDTH = 8,
    parameter ACC_WIDTH = 32
)(
    input  logic                    clk,
    input  logic                    rst_n,

    // Weight loading interface
    input  logic                    en_weight_pass,
    input  logic                    en_weight_capture,
    input  logic [DATA_WIDTH-1:0]   weight_in,

    // Activation input (flows right through systolic array)
    input  logic [DATA_WIDTH-1:0]   act_in,

    // Partial sum input (flows down through systolic array)
    input  logic [ACC_WIDTH-1:0]    psum_in,

    // Outputs
    output logic [DATA_WIDTH-1:0]   act_out,     // Activation flows right
    output logic [ACC_WIDTH-1:0]    psum_out     // Partial sum flows down
);

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

// Weight register
logic [DATA_WIDTH-1:0] weight_reg;

// Multiply-Accumulate result
logic signed [DATA_WIDTH*2-1:0] mult_result;
logic signed [ACC_WIDTH-1:0]    mac_result;

// =============================================================================
// WEIGHT REGISTER
// =============================================================================

// Weight capture logic (systolic weight loading)
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        weight_reg <= {DATA_WIDTH{1'b0}};
    end else if (en_weight_capture) begin
        weight_reg <= weight_in;
    end
end

// =============================================================================
// MULTIPLY-ACCUMULATE LOGIC
// =============================================================================

// Synchronous MAC operation
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        mac_result <= {ACC_WIDTH{1'b0}};
    end else begin
        // MAC: psum_out = psum_in + (act_in * weight_reg)
        mult_result = $signed(act_in) * $signed(weight_reg);
        mac_result  = $signed(psum_in) + $signed(mult_result);
    end
end

// =============================================================================
// OUTPUT ASSIGNMENTS
// =============================================================================

// Activation flows right (registered for systolic timing)
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        act_out <= {DATA_WIDTH{1'b0}};
    end else begin
        act_out <= act_in;
    end
end

// Partial sum output (MAC result flows down)
assign psum_out = mac_result;

endmodule
