`timescale 1ns / 1ps

module tpu_datapath (
    input  logic        clk,
    input  logic        rst_n,

    // Control signals from controller
    input  logic        sys_start,
    input  logic [7:0]  sys_rows,
    input  logic [7:0]  ub_rd_addr,
    input  logic        wt_fifo_wr,
    input  logic        vpu_start,
    input  logic [3:0]  vpu_mode,
    input  logic        wt_buf_sel,
    input  logic        acc_buf_sel,

    // Data interfaces
    input  logic [15:0] wt_fifo_data,  // Weight data input
    input  logic [255:0] ub_wr_data,   // Unified buffer write data
    output logic [255:0] ub_rd_data,   // Unified buffer read data

    // Status outputs to controller
    output logic        sys_busy,
    output logic        sys_done,
    output logic        vpu_busy,
    output logic        vpu_done,
    output logic        dma_busy,
    output logic        dma_done
);

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

// Systolic array connections (tinytinyTPU compatible)
logic [7:0] row0_act, row1_act, row2_act;  // Separate row activations
logic [7:0] col0_wt, col1_wt, col2_wt;     // Separate column weights
logic [31:0] acc0_out, acc1_out, acc2_out; // Direct accumulator outputs

// Systolic controller signals
logic        en_weight_pass;
logic        en_capture_col0;
logic        en_capture_col1;
logic        en_capture_col2;
logic        systolic_active;

// Accumulator connections
logic        acc_wr_en;
logic [7:0]  acc_wr_addr;
logic [63:0] acc_wr_data;
logic        acc_rd_en;
logic [7:0]  acc_rd_addr;
logic [63:0] acc_rd_data;

// VPU connections
logic [255:0] vpu_out_data;
logic         vpu_out_valid;

// Unified buffer connections
logic         ub_port_a_rd_en;
logic [7:0]   ub_port_a_addr;
logic [7:0]   ub_port_a_count;
logic         ub_port_a_valid;

logic         ub_port_b_wr_en;
logic [7:0]   ub_port_b_addr;
logic [7:0]   ub_port_b_count;
logic         ub_port_b_ready;

logic         ub_busy;
logic         ub_done;

// Weight FIFO connections
logic         wt_rd_en;
logic         wt_rd_empty;
logic         wt_wr_full;
logic         wt_load_done;

// =============================================================================
// SYSTOLIC CONTROLLER
// =============================================================================

systolic_controller systolic_ctrl (
    .clk             (clk),
    .rst_n           (rst_n),
    .sys_start       (sys_start),
    .sys_rows        (sys_rows),
    .sys_busy        (sys_busy),
    .sys_done        (sys_done),
    .en_weight_pass  (en_weight_pass),
    .en_capture_col0 (en_capture_col0),
    .en_capture_col1 (en_capture_col1),
    .en_capture_col2 (en_capture_col2),
    .systolic_active (systolic_active)
);

// =============================================================================
// WEIGHT FIFO (DOUBLE-BUFFERED)
// =============================================================================

// Dual Weight FIFO (tinytinyTPU compatible)
logic wf_push_col0, wf_push_col1, wf_push_col2;
logic wf_pop;

dual_weight_fifo weight_fifo_inst (
    .clk       (clk),
    .rst_n     (rst_n),
    .push_col0 (wf_push_col0),
    .push_col1 (wf_push_col1),
    .push_col2 (wf_push_col2),
    .data_in   (wt_fifo_data[7:0]),  // Use lower 8 bits for shared bus
    .pop       (wf_pop),
    .col0_out  (col0_wt),
    .col1_out  (col1_wt),
    .col2_out  (col2_wt),
    .col1_raw  (),                  // Not used
    .col2_raw  ()                   // Not used
);

// Weight FIFO control logic (simplified for now)
assign wf_push_col0 = wt_fifo_wr && (wt_fifo_data[9:8] == 2'b00);  // Tile ID in bits 9:8
assign wf_push_col1 = wt_fifo_wr && (wt_fifo_data[9:8] == 2'b01);
assign wf_push_col2 = wt_fifo_wr && (wt_fifo_data[9:8] == 2'b10);
assign wf_pop = wt_rd_en;

// =============================================================================
// SYSTOLIC ARRAY (2x2 MMU)
// =============================================================================

mmu systolic_array (
    .clk             (clk),
    .rst_n           (rst_n),
    .en_weight_pass  (en_weight_pass),
    .en_capture_col0 (en_capture_col0),
    .en_capture_col1 (en_capture_col1),
    .en_capture_col2 (en_capture_col2),
    .row0_in         (row0_act),
    .row1_in         (row1_act),
    .row2_in         (row2_act),
    .col0_in         (col0_wt),
    .col1_in         (col1_wt),
    .col2_in         (col2_wt),
    .acc0_out        (acc0_out),
    .acc1_out        (acc1_out),
    .acc2_out        (acc2_out)
);

// =============================================================================
// ACCUMULATORS (DOUBLE-BUFFERED)
// =============================================================================

accumulator accumulators (
    .clk         (clk),
    .rst_n       (rst_n),
    .acc_buf_sel (acc_buf_sel),
    .wr_en       (acc_wr_en),
    .wr_addr     (acc_wr_addr),
    .wr_data     (acc_wr_data),
    .rd_en       (acc_rd_en),
    .rd_addr     (acc_rd_addr),
    .rd_data     (acc_rd_data),
    .acc_busy    ()  // Not connected
);

// =============================================================================
// ACTIVATION PIPELINES (tinytinyTPU compatible)
// One pipeline per column for post-accumulator processing
// =============================================================================

// Activation pipeline configuration (runtime programmable)
logic signed [15:0] norm_gain;
logic signed [31:0] norm_bias;
logic [4:0]  norm_shift;
logic signed [15:0] q_inv_scale;
logic signed [7:0]  q_zero_point;

// Default configuration values
assign norm_gain = 16'h0100;     // Gain = 1.0 (Q8.8)
assign norm_bias = 32'sd0;       // No bias
assign norm_shift = 5'd0;        // No shift
assign q_inv_scale = 16'h0100;   // Scale = 1.0 (Q8.8)
assign q_zero_point = 8'sd0;     // Zero point = 0

// Extract individual accumulator outputs for pipelines
logic signed [31:0] acc_col0, acc_col1, acc_col2;
assign acc_col0 = acc_rd_data[31:0];    // Column 0
assign acc_col1 = acc_rd_data[63:32];   // Column 1
assign acc_col2 = acc_rd_data[95:64];   // Column 2

// Activation pipelines for each column
logic ap_valid_col0, ap_valid_col1, ap_valid_col2;
logic signed [7:0] ap_data_col0, ap_data_col1, ap_data_col2;
logic loss_valid_col0, loss_valid_col1, loss_valid_col2;
logic signed [31:0] loss_col0, loss_col1, loss_col2;

activation_pipeline ap_col0 (
    .clk(clk),
    .rst_n(rst_n),
    .valid_in(acc_rd_en),  // Trigger when accumulator read happens
    .acc_in(acc_col0),
    .target_in(32'sd0),    // No loss computation for inference
    .norm_gain(norm_gain),
    .norm_bias(norm_bias),
    .norm_shift(norm_shift),
    .q_inv_scale(q_inv_scale),
    .q_zero_point(q_zero_point),
    .valid_out(ap_valid_col0),
    .ub_data_out(ap_data_col0),
    .loss_valid(loss_valid_col0),
    .loss_out(loss_col0)
);

activation_pipeline ap_col1 (
    .clk(clk),
    .rst_n(rst_n),
    .valid_in(acc_rd_en),
    .acc_in(acc_col1),
    .target_in(32'sd0),
    .norm_gain(norm_gain),
    .norm_bias(norm_bias),
    .norm_shift(norm_shift),
    .q_inv_scale(q_inv_scale),
    .q_zero_point(q_zero_point),
    .valid_out(ap_valid_col1),
    .ub_data_out(ap_data_col1),
    .loss_valid(loss_valid_col1),
    .loss_out(loss_col1)
);

activation_pipeline ap_col2 (
    .clk(clk),
    .rst_n(rst_n),
    .valid_in(acc_rd_en),
    .acc_in(acc_col2),
    .target_in(32'sd0),
    .norm_gain(norm_gain),
    .norm_bias(norm_bias),
    .norm_shift(norm_shift),
    .q_inv_scale(q_inv_scale),
    .q_zero_point(q_zero_point),
    .valid_out(ap_valid_col2),
    .ub_data_out(ap_data_col2),
    .loss_valid(loss_valid_col2),
    .loss_out(loss_col2)
);

// Combine activation pipeline outputs
assign vpu_out_valid = ap_valid_col0 || ap_valid_col1 || ap_valid_col2;
assign vpu_out_data = {8'b0, ap_data_col2, ap_data_col1, ap_data_col0};  // Pack 3 bytes
assign vpu_busy = 1'b0;  // Simplified - always ready
assign vpu_done = vpu_out_valid;  // Done when output valid

// =============================================================================
// UNIFIED BUFFER (DUAL-PORT)
// =============================================================================

unified_buffer ub (
    .clk             (clk),
    .rst_n           (rst_n),
    .port_a_rd_en    (ub_port_a_rd_en),
    .port_a_addr     (ub_port_a_addr),
    .port_a_count    (ub_port_a_count),
    .port_a_data     (ub_rd_data),
    .port_a_valid    (ub_port_a_valid),
    .port_b_wr_en    (ub_port_b_wr_en),
    .port_b_addr     (ub_port_b_addr),
    .port_b_count    (ub_port_b_count),
    .port_b_data     (ub_wr_data),
    .port_b_ready    (ub_port_b_ready),
    .ub_busy         (ub_busy),
    .ub_done         (ub_done)
);

// =============================================================================
// DATA EXTRACTION FOR SYSTOLIC ARRAY (tinytinyTPU compatible)
// =============================================================================

// Extract row activations from unified buffer (assuming 3 rows × 8-bit)
assign row0_act = ub_rd_data[7:0];    // Row 0 activation
assign row1_act = ub_rd_data[15:8];   // Row 1 activation
assign row2_act = ub_rd_data[23:16];  // Row 2 activation

// Extract column weights from weight FIFO (assuming 3 weights × 8-bit)
assign col0_wt = wt_fifo_data[7:0];   // Column 0 weight
assign col1_wt = wt_fifo_data[15:8];  // Column 1 weight
assign col2_wt = wt_fifo_data[23:16]; // Column 2 weight

// =============================================================================
// DATA FLOW CONTROL
// =============================================================================

// Systolic array data flow
assign act_to_sys = ub_rd_data[15:0];  // First 16 bits as activations

// Accumulator write control
assign acc_wr_en = systolic_active;
assign acc_wr_addr = 8'h00;  // Fixed address for now
assign acc_wr_data = {acc2_out, acc1_out, acc0_out};  // Direct from MMU outputs

// Accumulator read control (for VPU)
assign acc_rd_en = vpu_start;
assign acc_rd_addr = 8'h00;  // Fixed address for now

// Weight FIFO read control
assign wt_rd_en = systolic_active && !wt_rd_empty;

// Unified buffer read control
assign ub_port_a_rd_en = sys_start;
assign ub_port_a_addr = ub_rd_addr;
assign ub_port_a_count = sys_rows;  // Read number of rows

// Unified buffer write control (from VPU)
assign ub_port_b_wr_en = vpu_out_valid;
assign ub_port_b_addr = 8'h00;  // Fixed address for now
assign ub_port_b_count = 8'h01; // Single write for now

// DMA status (simplified - always ready)
assign dma_busy = 1'b0;
assign dma_done = 1'b1;

endmodule
