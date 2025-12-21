`timescale 1ns / 1ps

module mmu #(
    parameter DATA_WIDTH = 8,
    parameter ACC_WIDTH = 32,
    parameter ARRAY_SIZE = 3  // 3x3 systolic array for better forward pass performance
)(
    input  logic                    clk,
    input  logic                    rst_n,

    // Control signals (tinytinyTPU compatible)
    input  logic                    en_weight_pass,      // Pass psum through all PEs (always during load phase)
    input  logic                    en_capture_col0,     // Capture enable for column 0 PEs
    input  logic                    en_capture_col1,     // Capture enable for column 1 PEs
    input  logic                    en_capture_col2,     // Capture enable for column 2 PEs (our extension)

    // UB activations input (tinytinyTPU compatible interface)
    input  logic [DATA_WIDTH-1:0]   row0_in,             // Row 0 activation input
    input  logic [DATA_WIDTH-1:0]   row1_in,             // Row 1 activation input
    input  logic [DATA_WIDTH-1:0]   row2_in,             // Row 2 activation input (our extension)

    // Weight FIFO input (tinytinyTPU compatible interface)
    input  logic [DATA_WIDTH-1:0]   col0_in,             // Column 0 weight input
    input  logic [DATA_WIDTH-1:0]   col1_in,             // Column 1 weight input
    input  logic [DATA_WIDTH-1:0]   col2_in,             // Column 2 weight input (our extension)

    // Accumulator output (tinytinyTPU compatible)
    output logic [ACC_WIDTH-1:0]    acc0_out,            // Column 0 to accumulator
    output logic [ACC_WIDTH-1:0]    acc1_out,            // Column 1 to accumulator
    output logic [ACC_WIDTH-1:0]    acc2_out             // Column 2 to accumulator (our extension)
);

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

// PE connections - systolic array interconnections
logic [DATA_WIDTH-1:0] pe_act_in  [ARRAY_SIZE][ARRAY_SIZE];  // act[row][col]
logic [DATA_WIDTH-1:0] pe_act_out [ARRAY_SIZE][ARRAY_SIZE];  // act[row][col]
logic [ACC_WIDTH-1:0]  pe_psum_in [ARRAY_SIZE][ARRAY_SIZE];  // psum[row][col]
logic [ACC_WIDTH-1:0]  pe_psum_out[ARRAY_SIZE][ARRAY_SIZE];  // psum[row][col]

// Row1 and Row2 skew registers for systolic timing (tinytinyTPU approach)
logic [DATA_WIDTH-1:0] row1_skew_reg, row2_skew_reg;

// =============================================================================
// SYSTOLIC ARRAY CONNECTIONS (tinytinyTPU compatible)
// =============================================================================

// Row skew registers for systolic timing
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        row1_skew_reg <= {DATA_WIDTH{1'b0}};
        row2_skew_reg <= {DATA_WIDTH{1'b0}};
    end else begin
        // Row1 gets 1-cycle delay, Row2 gets 2-cycle delay for wavefront
        row1_skew_reg <= row1_in;
        row2_skew_reg <= row1_skew_reg;
    end
end

// PE Layout:
// pe00 -> pe01 -> pe02    Activations flow horizontally (right)
//   |       |       |
//   v       v       v     Partial sums flow vertically (down)
// pe10 -> pe11 -> pe12
//   |       |       |
//   v       v       v
// pe20 -> pe21 -> pe22
//   |       |       |
//   v       v       v
// acc0    acc1    acc2

// Systolic array PE connections
logic [DATA_WIDTH-1:0] pe00_01_act, pe01_02_act, pe10_11_act, pe11_12_act, pe20_21_act, pe21_22_act;
logic [ACC_WIDTH-1:0]  pe00_10_psum, pe01_11_psum, pe02_12_psum, pe10_20_psum, pe11_21_psum, pe12_22_psum;

// =============================================================================
// INSTANTIATE PROCESSING ELEMENTS
// =============================================================================

// PE instantiations (tinytinyTPU compatible)
pe pe00 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_col0),  // Column 0 capture timing
    .act_in(row0_in),
    .psum_in({24'b0, col0_in}),           // Weights loaded through psum path
    .act_out(pe00_01_act),
    .psum_out(pe00_10_psum)
);

pe pe01 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_col1),  // Column 1 capture timing (staggered)
    .act_in(pe00_01_act),
    .psum_in({24'b0, col1_in}),           // Weights loaded through psum path
    .act_out(pe01_02_act),
    .psum_out(pe01_11_psum)
);

pe pe02 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_col2),  // Column 2 capture timing (our extension)
    .act_in(pe01_02_act),
    .psum_in({24'b0, col2_in}),           // Weights loaded through psum path
    .act_out(),                           // Activation unconnected at edge
    .psum_out(pe02_12_psum)
);

pe pe10 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_col0),  // Column 0 capture timing
    .act_in(row1_skew_reg),               // Delayed row1 input
    .psum_in(pe00_10_psum),
    .act_out(pe10_11_act),
    .psum_out(pe10_20_psum)
);

pe pe11 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_col1),  // Column 1 capture timing (staggered)
    .act_in(pe10_11_act),
    .psum_in(pe01_11_psum),
    .act_out(pe11_12_act),
    .psum_out(pe11_21_psum)
);

pe pe12 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_col2),  // Column 2 capture timing (our extension)
    .act_in(pe11_12_act),
    .psum_in(pe02_12_psum),
    .act_out(),                           // Activation unconnected at edge
    .psum_out(pe12_22_psum)
);

pe pe20 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_col0),  // Column 0 capture timing
    .act_in(row2_skew_reg),               // Double-delayed row2 input
    .psum_in(pe10_20_psum),
    .act_out(pe20_21_act),
    .psum_out(acc0_out)                   // Output column 0 to accumulator
);

pe pe21 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_col1),  // Column 1 capture timing (staggered)
    .act_in(pe20_21_act),
    .psum_in(pe11_21_psum),
    .act_out(pe21_22_act),
    .psum_out(acc1_out)                   // Output column 1 to accumulator
);

pe pe22 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_col2),  // Column 2 capture timing (our extension)
    .act_in(pe21_22_act),
    .psum_in(pe12_22_psum),
    .act_out(),                           // Activation unconnected at edge
    .psum_out(acc2_out)                   // Output column 2 to accumulator (our extension)
);

endmodule
