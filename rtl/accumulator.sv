`timescale 1ns / 1ps

module accumulator (
    input  logic        clk,
    input  logic        rst_n,

    input  logic        valid_in,           // MMU data valid
    input  logic        accumulator_enable, // Add to existing sum (1) or overwrite (0)
    input  logic        addr_sel,           // Buffer selection (0/1)

    input  logic [31:0] mmu_col0_in,        // Column 0 from MMU
    input  logic [31:0] mmu_col1_in,        // Column 1 from MMU
    input  logic [31:0] mmu_col2_in,        // Column 2 from MMU (our extension)

    output logic signed [31:0] acc_col0_out, // Column 0 to activation pipeline
    output logic signed [31:0] acc_col1_out, // Column 1 to activation pipeline
    output logic signed [31:0] acc_col2_out, // Column 2 to activation pipeline (our extension)
    output logic        valid_out           // Data valid for activation pipeline
);

// =============================================================================
// MODULAR ACCUMULATOR (tinytinyTPU compatible)
// =============================================================================

logic        aligned_valid;
logic [31:0] aligned_col0, aligned_col1, aligned_col2;

accumulator_align align_u (
    .clk(clk),
    .rst_n(rst_n),
    .valid_in(valid_in),
    .raw_col0(mmu_col0_in),
    .raw_col1(mmu_col1_in),
    .raw_col2(mmu_col2_in),
    .aligned_valid(aligned_valid),
    .align_col0(aligned_col0),
    .align_col1(aligned_col1),
    .align_col2(aligned_col2)
);

accumulator_mem mem_u (
    .clk(clk),
    .rst_n(rst_n),
    .enable(aligned_valid),
    .accumulator_mode(accumulator_enable),
    .buffer_select(addr_sel),
    .in_col0(aligned_col0),
    .in_col1(aligned_col1),
    .in_col2(aligned_col2),
    .valid_out(valid_out),
    .out_col0(acc_col0_out),
    .out_col1(acc_col1_out),
    .out_col2(acc_col2_out)
);

endmodule
