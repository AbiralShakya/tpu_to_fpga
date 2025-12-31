// =============================================================================
// Activation Pipeline (tinytinyTPU compatible)
// Full post-accumulator pipeline: Activation -> Normalization -> Loss -> Quantization
// =============================================================================

`timescale 1ns / 1ps

module activation_pipeline (
    input  logic        clk,
    input  logic        rst_n,
    input  logic        valid_in,
    input  logic signed [31:0] acc_in,       // from accumulator
    input  logic signed [31:0] target_in,    // optional target for loss (set to 0 if unused)

    // Configuration
    input  logic        passthrough,     // 1 = passthrough mode (no ReLU, no norm, no quant, just clamp)
    input  logic        unsigned_mode,  // 1 = unsigned clamp [0,255], 0 = signed clamp [-128,127]
    input  logic signed [15:0] norm_gain,
    input  logic signed [31:0] norm_bias,
    input  logic [4:0]  norm_shift,
    input  logic signed [15:0] q_inv_scale, // 1/S in Q8.8
    input  logic signed [7:0] q_zero_point,

    // Outputs
    output logic        valid_out,
    output logic signed [7:0] ub_data_out,   // to unified buffer
    output logic        loss_valid,
    output logic signed [31:0] loss_out
);

    // Stage 1: Activation
    logic        s1_valid;
    logic signed [31:0] s1_data;
    logic signed [31:0] target_d1;

    activation_func act_u (
        .clk(clk),
        .rst_n(rst_n),
        .valid_in(valid_in),
        .data_in(acc_in),
        .passthrough(passthrough),
        .valid_out(s1_valid),
        .data_out(s1_data)
    );

    // Align target with pipeline (normalizer adds one cycle)
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            target_d1 <= 32'sd0;
        else if (valid_in)
            target_d1 <= target_in;
    end

    // Stage 2: Normalization
    logic        s2_valid;
    logic signed [31:0] s2_data;

    normalizer norm_u (
        .clk(clk),
        .rst_n(rst_n),
        .valid_in(s1_valid),
        .data_in(s1_data),
        .gain(norm_gain),
        .bias(norm_bias),
        .shift(norm_shift),
        .valid_out(s2_valid),
        .data_out(s2_data)
    );

    // Stage 3a: Loss (in parallel with quantization)
    loss_block loss_u (
        .clk(clk),
        .rst_n(rst_n),
        .valid_in(s2_valid),
        .data_in(s2_data),
        .target_in(target_d1),
        .valid_out(loss_valid),
        .loss_out(loss_out)
    );

    // Stage 3b: Quantization to UB (affine quantization) or Passthrough Clamping
    logic signed [7:0] ub_q_reg;
    logic valid_reg;

    logic signed [47:0] mult;
    logic signed [47:0] mult_rounded;
    logic signed [31:0] scaled;
    logic signed [31:0] biased;
    logic signed [31:0] clamp_input;  // Input to clamping logic

    // Passthrough mode: skip normalization and quantization, just clamp
    // Normal mode: apply normalization and quantization
    assign clamp_input = passthrough ? s1_data : s2_data;

    // Quantization (only in normal mode)
    assign mult = s2_data * q_inv_scale;              // 32x16 -> 48
    assign mult_rounded = mult + 48'sd128;            // +0.5 * 2^8 for nearest
    assign scaled = 32'(mult_rounded >>> 8);          // back to Q0
    assign biased = scaled + {{24{q_zero_point[7]}}, q_zero_point};

    // Clamping: signed [-128,127] or unsigned [0,255]
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            valid_reg <= 1'b0;
            ub_q_reg  <= 8'sd0;
        end else begin
            valid_reg <= passthrough ? s1_valid : s2_valid;
            if (passthrough) begin
                // Passthrough mode: direct clamping
                if (unsigned_mode) begin
                    // Unsigned: clamp to [0, 255]
                    // CRITICAL: Use signed comparison for negative check, then unsigned for upper bound
                    if ($signed(clamp_input) < 0)
                        ub_q_reg <= 8'd0;  // Negative values clamp to 0
                    else if (clamp_input > 32'd255)
                        ub_q_reg <= 8'd255;  // Values > 255 clamp to 255
                    else
                        ub_q_reg <= clamp_input[7:0];  // Direct truncation for [0, 255]
                end else begin
                    // Signed: clamp to [-128, 127]
                    if (clamp_input > 32'sd127)
                        ub_q_reg <= 8'sd127;
                    else if (clamp_input < -32'sd128)
                        ub_q_reg <= -8'sd128;
                    else
                        ub_q_reg <= clamp_input[7:0];
                end
            end else begin
                // Normal mode: quantization with signed clamping
                if (biased > 127)
                    ub_q_reg <= 8'sd127;
                else if (biased < -128)
                    ub_q_reg <= -8'sd128;
                else
                    ub_q_reg <= biased[7:0];
            end
        end
    end

    assign valid_out   = valid_reg;
    assign ub_data_out = ub_q_reg;

endmodule
