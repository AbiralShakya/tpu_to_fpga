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
    // Row+column specific capture signals for staggered timing (registered pass-through delay)
    input  logic                    en_capture_row0_col0,  // Row 0, Column 0 (pe00)
    input  logic                    en_capture_row0_col1,  // Row 0, Column 1 (pe01)
    input  logic                    en_capture_row0_col2,  // Row 0, Column 2 (pe02)
    input  logic                    en_capture_row1_col0,  // Row 1, Column 0 (pe10) - 1 cycle delay
    input  logic                    en_capture_row1_col1,  // Row 1, Column 1 (pe11) - 1 cycle delay
    input  logic                    en_capture_row1_col2,  // Row 1, Column 2 (pe12) - 1 cycle delay
    input  logic                    en_capture_row2_col0,  // Row 2, Column 0 (pe20) - 2 cycle delay
    input  logic                    en_capture_row2_col1,  // Row 2, Column 1 (pe21) - 2 cycle delay
    input  logic                    en_capture_row2_col2,  // Row 2, Column 2 (pe22) - 2 cycle delay
    input  logic                    use_signed,          // Use signed arithmetic (from MATMUL instruction)

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

// Row1 and Row2 skew registers for systolic timing (tinytinyTPU approach)
// Note: PE connections use individual signals (pe00_01_act, etc.) not 2D arrays
logic [DATA_WIDTH-1:0] row1_skew_reg;
logic [DATA_WIDTH-1:0] row2_delay1, row2_skew_reg;  // Two-stage delay for row2

// CRITICAL FIX: Top row psum_in must be 0 during compute, weight value during loading
logic [ACC_WIDTH-1:0] pe00_psum_in, pe01_psum_in, pe02_psum_in;

// =============================================================================
// SYSTOLIC ARRAY CONNECTIONS (tinytinyTPU compatible)
// =============================================================================

// Row skew registers for systolic timing
// CRITICAL: Each row needs its OWN delay chain, not shared!
// Row1 gets 1-cycle delay, Row2 gets 2-cycle delay for diagonal wavefront
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        row1_skew_reg <= {DATA_WIDTH{1'b0}};
        row2_delay1   <= {DATA_WIDTH{1'b0}};
        row2_skew_reg <= {DATA_WIDTH{1'b0}};
    end else begin
        // Row1: 1-cycle delay
        row1_skew_reg <= row1_in;
        // Row2: 2-cycle delay (through its OWN delay chain, not row1's!)
        row2_delay1   <= row2_in;       // First delay stage
        row2_skew_reg <= row2_delay1;   // Second delay stage
    end
end

// =============================================================================
// TOP ROW PSUM MUX (CRITICAL FIX)
// =============================================================================
// During weight loading (en_weight_pass=1): weights pass through psum path
// During compute (en_weight_pass=0): top row needs psum_in=0 (start of accumulation)
assign pe00_psum_in = en_weight_pass ? {24'b0, col0_in} : 32'b0;
assign pe01_psum_in = en_weight_pass ? {24'b0, col1_in} : 32'b0;
assign pe02_psum_in = en_weight_pass ? {24'b0, col2_in} : 32'b0;

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
    .en_weight_capture(en_capture_row0_col0),  // Row 0, Column 0 capture timing
    .use_signed(use_signed),              // Signed arithmetic control
    .act_in(row0_in),
    .psum_in(pe00_psum_in),               // FIXED: Muxed - weights during load, 0 during compute
    .act_out(pe00_01_act),
    .psum_out(pe00_10_psum)
);

pe pe01 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_row0_col1),  // Row 0, Column 1 capture timing
    .use_signed(use_signed),              // Signed arithmetic control
    .act_in(pe00_01_act),
    .psum_in(pe01_psum_in),               // FIXED: Muxed - weights during load, 0 during compute
    .act_out(pe01_02_act),
    .psum_out(pe01_11_psum)
);

pe pe02 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_row0_col2),  // Row 0, Column 2 capture timing
    .use_signed(use_signed),              // Signed arithmetic control
    .act_in(pe01_02_act),
    .psum_in(pe02_psum_in),               // FIXED: Muxed - weights during load, 0 during compute
    .act_out(),                           // Activation unconnected at edge
    .psum_out(pe02_12_psum)
);

pe pe10 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_row1_col0),  // Row 1, Column 0 capture timing (1 cycle delay)
    .use_signed(use_signed),              // Signed arithmetic control
    .act_in(row1_skew_reg),               // Delayed row1 input
    .psum_in(pe00_10_psum),
    .act_out(pe10_11_act),
    .psum_out(pe10_20_psum)
);

pe pe11 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_row1_col1),  // Row 1, Column 1 capture timing (1 cycle delay)
    .use_signed(use_signed),              // Signed arithmetic control
    .act_in(pe10_11_act),
    .psum_in(pe01_11_psum),
    .act_out(pe11_12_act),
    .psum_out(pe11_21_psum)
);

pe pe12 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_row1_col2),  // Row 1, Column 2 capture timing (1 cycle delay)
    .use_signed(use_signed),              // Signed arithmetic control
    .act_in(pe11_12_act),
    .psum_in(pe02_12_psum),
    .act_out(),                           // Activation unconnected at edge
    .psum_out(pe12_22_psum)
);

pe pe20 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_row2_col0),  // Row 2, Column 0 capture timing (2 cycle delay)
    .use_signed(use_signed),              // Signed arithmetic control
    .act_in(row2_skew_reg),               // Double-delayed row2 input
    .psum_in(pe10_20_psum),
    .act_out(pe20_21_act),
    .psum_out(acc0_out)                   // Output column 0 to accumulator
);

pe pe21 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_row2_col1),  // Row 2, Column 1 capture timing (2 cycle delay)
    .use_signed(use_signed),              // Signed arithmetic control
    .act_in(pe20_21_act),
    .psum_in(pe11_21_psum),
    .act_out(pe21_22_act),
    .psum_out(acc1_out)                   // Output column 1 to accumulator
);

pe pe22 (
    .clk(clk),
    .rst_n(rst_n),
    .en_weight_pass(en_weight_pass),
    .en_weight_capture(en_capture_row2_col2),  // Row 2, Column 2 capture timing (2 cycle delay)
    .use_signed(use_signed),              // Signed arithmetic control
    .act_in(pe21_22_act),
    .psum_in(pe12_22_psum),
    .act_out(),                           // Activation unconnected at edge
    .psum_out(acc2_out)                   // Output column 2 to accumulator (our extension)
);

endmodule
