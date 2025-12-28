`timescale 1ns / 1ps

module pe #(
    parameter DATA_WIDTH = 8,
    parameter ACC_WIDTH = 32
)(
    input  logic                    clk,
    input  logic                    rst_n,

    // Control signals (tinytinyTPU compatible)
    input  logic                    en_weight_pass,    // Pass psum through (always during load phase)
    input  logic                    en_weight_capture, // Capture weight from psum (per-PE timing for diagonal)
    input  logic                    use_signed,        // Use signed arithmetic (from MATMUL instruction)

    // Data inputs (tinytinyTPU compatible interface)
    input  logic [DATA_WIDTH-1:0]   act_in,            // Activation input (flows right)
    input  logic [ACC_WIDTH-1:0]    psum_in,           // Partial sum input (flows down)

    // Outputs
    output logic [DATA_WIDTH-1:0]   act_out,           // Activation flows right
    output logic [ACC_WIDTH-1:0]    psum_out           // Partial sum flows down
);

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

// Weight register (loaded from psum input path like tinytinyTPU)
logic [DATA_WIDTH-1:0] weight_reg;

// Signed versions for multiplication
logic signed [DATA_WIDTH-1:0]   act_signed, weight_signed;
logic signed [2*DATA_WIDTH-1:0] product_signed;
logic        [2*DATA_WIDTH-1:0] product_unsigned;
logic signed [ACC_WIDTH-1:0]    product_extended;

// =============================================================================
// SIGNED/UNSIGNED MULTIPLICATION LOGIC
// =============================================================================

// Cast to signed for signed multiplication
assign act_signed    = $signed(act_in);
assign weight_signed = $signed(weight_reg);

// Compute both signed and unsigned products
assign product_signed   = act_signed * weight_signed;
assign product_unsigned = act_in * weight_reg;

// Sign-extend or zero-extend product to accumulator width
assign product_extended = use_signed ?
    {{(ACC_WIDTH-2*DATA_WIDTH){product_signed[2*DATA_WIDTH-1]}}, product_signed} :
    {{(ACC_WIDTH-2*DATA_WIDTH){1'b0}}, product_unsigned};

// =============================================================================
// PE LOGIC (tinytinyTPU compatible)
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        act_out <= {DATA_WIDTH{1'b0}};
        psum_out <= {ACC_WIDTH{1'b0}};
        weight_reg <= {DATA_WIDTH{1'b0}};
    end
    else begin
        if (en_weight_pass) begin
            // Weight loading mode: pass psum through, reset activation
            psum_out <= psum_in;
            act_out <= {DATA_WIDTH{1'b0}};
            // Capture weight only when this PE's capture signal is active
            if (en_weight_capture) begin
                weight_reg <= psum_in[DATA_WIDTH-1:0];  // Extract weight from lower bits
            end
        end
        else begin
            // Compute mode: MAC operation with signed/unsigned support
            act_out <= act_in;
            psum_out <= $signed(psum_in) + product_extended;
        end
    end
end

endmodule
