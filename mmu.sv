`timescale 1ns / 1ps

module mmu #(
    parameter DATA_WIDTH = 8,
    parameter ACC_WIDTH = 32,
    parameter ARRAY_SIZE = 3  // 3x3 systolic array for better forward pass performance
)(
    input  logic                    clk,
    input  logic                    rst_n,

    // Control signals
    input  logic                    en_weight_pass,
    input  logic                    en_capture_col0,
    input  logic                    en_capture_col1,

// Weight input (from weight FIFO)
    input  logic [DATA_WIDTH*ARRAY_SIZE-1:0] weight_data,  // 24-bit for 3x8-bit weights

// Activation inputs (row-wise)
    input  logic [DATA_WIDTH*ARRAY_SIZE-1:0] act_data,     // 24-bit for 3x8-bit activations

// Partial sum outputs (column-wise to accumulators)
    output logic [ACC_WIDTH*ARRAY_SIZE-1:0]  psum_col0_out,  // Column 0 output
    output logic [ACC_WIDTH*ARRAY_SIZE-1:0]  psum_col1_out,  // Column 1 output
    output logic [ACC_WIDTH*ARRAY_SIZE-1:0]  psum_col2_out   // Column 2 output
);

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

// PE connections - systolic array interconnections
logic [DATA_WIDTH-1:0] pe_act_in  [ARRAY_SIZE][ARRAY_SIZE];  // act[row][col]
logic [DATA_WIDTH-1:0] pe_act_out [ARRAY_SIZE][ARRAY_SIZE];  // act[row][col]
logic [ACC_WIDTH-1:0]  pe_psum_in [ARRAY_SIZE][ARRAY_SIZE];  // psum[row][col]
logic [ACC_WIDTH-1:0]  pe_psum_out[ARRAY_SIZE][ARRAY_SIZE];  // psum[row][col]

// Weight distribution to columns
logic [DATA_WIDTH-1:0] weight_col0, weight_col1, weight_col2;

// =============================================================================
// WEIGHT DISTRIBUTION LOGIC
// =============================================================================

// Split weight data into columns
assign weight_col0 = weight_data[DATA_WIDTH-1:0];              // Weight for column 0
assign weight_col1 = weight_data[2*DATA_WIDTH-1:DATA_WIDTH];   // Weight for column 1
assign weight_col2 = weight_data[3*DATA_WIDTH-1:2*DATA_WIDTH]; // Weight for column 2

// =============================================================================
// SYSTOLIC ARRAY CONNECTIONS
// =============================================================================

// Row 0 (top row)
assign pe_act_in[0][0] = act_data[DATA_WIDTH-1:0];              // Activation for PE[0][0]
assign pe_act_in[0][1] = pe_act_out[0][0];                      // From PE[0][0] to PE[0][1]
assign pe_act_in[0][2] = pe_act_out[0][1];                      // From PE[0][1] to PE[0][2]

// Row 1 (middle row) - delayed by 1 cycle for systolic wavefront
logic [DATA_WIDTH-1:0] act_row1_delayed;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        act_row1_delayed <= {DATA_WIDTH{1'b0}};
    end else begin
        act_row1_delayed <= act_data[2*DATA_WIDTH-1:DATA_WIDTH];   // Activation for row 1
    end
end

assign pe_act_in[1][0] = act_row1_delayed;                      // Delayed activation for PE[1][0]
assign pe_act_in[1][1] = pe_act_out[1][0];                      // From PE[1][0] to PE[1][1]
assign pe_act_in[1][2] = pe_act_out[1][1];                      // From PE[1][1] to PE[1][2]

// Row 2 (bottom row) - delayed by 2 cycles for systolic wavefront
logic [DATA_WIDTH-1:0] act_row2_delayed;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        act_row2_delayed <= {DATA_WIDTH{1'b0}};
    end else begin
        act_row2_delayed <= act_data[3*DATA_WIDTH-1:2*DATA_WIDTH]; // Activation for row 2
    end
end

assign pe_act_in[2][0] = act_row2_delayed;                      // Delayed activation for PE[2][0]
assign pe_act_in[2][1] = pe_act_out[2][0];                      // From PE[2][0] to PE[2][1]
assign pe_act_in[2][2] = pe_act_out[2][1];                      // From PE[2][1] to PE[2][2]

// Column partial sum flow (downward)
assign pe_psum_in[0][0] = {ACC_WIDTH{1'b0}};             // Top-left PE gets 0
assign pe_psum_in[0][1] = {ACC_WIDTH{1'b0}};             // Top-center PE gets 0
assign pe_psum_in[0][2] = {ACC_WIDTH{1'b0}};             // Top-right PE gets 0
assign pe_psum_in[1][0] = pe_psum_out[0][0];             // PE[1][0] gets from PE[0][0]
assign pe_psum_in[1][1] = pe_psum_out[0][1];             // PE[1][1] gets from PE[0][1]
assign pe_psum_in[1][2] = pe_psum_out[0][2];             // PE[1][2] gets from PE[0][2]
assign pe_psum_in[2][0] = pe_psum_out[1][0];             // PE[2][0] gets from PE[1][0]
assign pe_psum_in[2][1] = pe_psum_out[1][1];             // PE[2][1] gets from PE[1][1]
assign pe_psum_in[2][2] = pe_psum_out[1][2];             // PE[2][2] gets from PE[1][2]

// =============================================================================
// INSTANTIATE PROCESSING ELEMENTS
// =============================================================================

// PE[0][0] - Top-left
pe #(
    .DATA_WIDTH(DATA_WIDTH),
    .ACC_WIDTH(ACC_WIDTH)
) pe_00 (
    .clk               (clk),
    .rst_n             (rst_n),
    .en_weight_pass    (en_weight_pass),
    .en_weight_capture (en_capture_col0),     // Column 0 capture
    .weight_in         (weight_col0),
    .act_in            (pe_act_in[0][0]),
    .psum_in           (pe_psum_in[0][0]),
    .act_out           (pe_act_out[0][0]),
    .psum_out          (pe_psum_out[0][0])
);

// PE[0][1] - Top-right
pe #(
    .DATA_WIDTH(DATA_WIDTH),
    .ACC_WIDTH(ACC_WIDTH)
) pe_01 (
    .clk               (clk),
    .rst_n             (rst_n),
    .en_weight_pass    (en_weight_pass),
    .en_weight_capture (en_capture_col1),     // Column 1 capture
    .weight_in         (weight_col1),
    .act_in            (pe_act_in[0][1]),
    .psum_in           (pe_psum_in[0][1]),
    .act_out           (pe_act_out[0][1]),
    .psum_out          (pe_psum_out[0][1])
);

// PE[1][0] - Bottom-left
pe #(
    .DATA_WIDTH(DATA_WIDTH),
    .ACC_WIDTH(ACC_WIDTH)
) pe_10 (
    .clk               (clk),
    .rst_n             (rst_n),
    .en_weight_pass    (en_weight_pass),
    .en_weight_capture (en_capture_col0),     // Column 0 capture
    .weight_in         (weight_col0),
    .act_in            (pe_act_in[1][0]),
    .psum_in           (pe_psum_in[1][0]),
    .act_out           (pe_act_out[1][0]),
    .psum_out          (pe_psum_out[1][0])
);

// PE[1][1] - Bottom-center
pe #(
    .DATA_WIDTH(DATA_WIDTH),
    .ACC_WIDTH(ACC_WIDTH)
) pe_11 (
    .clk               (clk),
    .rst_n             (rst_n),
    .en_weight_pass    (en_weight_pass),
    .en_weight_capture (en_capture_col1),     // Column 1 capture
    .weight_in         (weight_col1),
    .act_in            (pe_act_in[1][1]),
    .psum_in           (pe_psum_in[1][1]),
    .act_out           (pe_act_out[1][1]),
    .psum_out          (pe_psum_out[1][1])
);

// PE[0][2] - Top-right
pe #(
    .DATA_WIDTH(DATA_WIDTH),
    .ACC_WIDTH(ACC_WIDTH)
) pe_02 (
    .clk               (clk),
    .rst_n             (rst_n),
    .en_weight_pass    (en_weight_pass),
    .en_weight_capture (en_capture_col2),     // Column 2 capture
    .weight_in         (weight_col2),
    .act_in            (pe_act_in[0][2]),
    .psum_in           (pe_psum_in[0][2]),
    .act_out           (pe_act_out[0][2]),
    .psum_out          (pe_psum_out[0][2])
);

// PE[1][2] - Middle-right
pe #(
    .DATA_WIDTH(DATA_WIDTH),
    .ACC_WIDTH(ACC_WIDTH)
) pe_12 (
    .clk               (clk),
    .rst_n             (rst_n),
    .en_weight_pass    (en_weight_pass),
    .en_weight_capture (en_capture_col2),     // Column 2 capture
    .weight_in         (weight_col2),
    .act_in            (pe_act_in[1][2]),
    .psum_in           (pe_psum_in[1][2]),
    .act_out           (pe_act_out[1][2]),
    .psum_out          (pe_psum_out[1][2])
);

// PE[2][0] - Bottom-left
pe #(
    .DATA_WIDTH(DATA_WIDTH),
    .ACC_WIDTH(ACC_WIDTH)
) pe_20 (
    .clk               (clk),
    .rst_n             (rst_n),
    .en_weight_pass    (en_weight_pass),
    .en_weight_capture (en_capture_col0),     // Column 0 capture
    .weight_in         (weight_col0),
    .act_in            (pe_act_in[2][0]),
    .psum_in           (pe_psum_in[2][0]),
    .act_out           (pe_act_out[2][0]),
    .psum_out          (pe_psum_out[2][0])
);

// PE[2][1] - Bottom-center
pe #(
    .DATA_WIDTH(DATA_WIDTH),
    .ACC_WIDTH(ACC_WIDTH)
) pe_21 (
    .clk               (clk),
    .rst_n             (rst_n),
    .en_weight_pass    (en_weight_pass),
    .en_weight_capture (en_capture_col1),     // Column 1 capture
    .weight_in         (weight_col1),
    .act_in            (pe_act_in[2][1]),
    .psum_in           (pe_psum_in[2][1]),
    .act_out           (pe_act_out[2][1]),
    .psum_out          (pe_psum_out[2][1])
);

// PE[2][2] - Bottom-right
pe #(
    .DATA_WIDTH(DATA_WIDTH),
    .ACC_WIDTH(ACC_WIDTH)
) pe_22 (
    .clk               (clk),
    .rst_n             (rst_n),
    .en_weight_pass    (en_weight_pass),
    .en_weight_capture (en_capture_col2),     // Column 2 capture
    .weight_in         (weight_col2),
    .act_in            (pe_act_in[2][2]),
    .psum_in           (pe_psum_in[2][2]),
    .act_out           (pe_act_out[2][2]),
    .psum_out          (pe_psum_out[2][2])
);

// =============================================================================
// OUTPUT ASSIGNMENTS
// =============================================================================

// Column outputs to accumulators
assign psum_col0_out = {pe_psum_out[2][0], pe_psum_out[1][0], pe_psum_out[0][0]};  // {row2, row1, row0}
assign psum_col1_out = {pe_psum_out[2][1], pe_psum_out[1][1], pe_psum_out[0][1]};  // {row2, row1, row0}
assign psum_col2_out = {pe_psum_out[2][2], pe_psum_out[1][2], pe_psum_out[0][2]};  // {row2, row1, row0}

endmodule
