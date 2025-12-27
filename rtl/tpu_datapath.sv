`timescale 1ns / 1ps

module tpu_datapath (
    input  logic        clk,
    input  logic        rst_n,

    // =============================================================================
    // CONTROL SIGNALS FROM CONTROLLER (29 signals)
    // =============================================================================

    // Systolic Array Control (7 signals)
    input  logic        sys_start,       // Start systolic operation
    input  logic [1:0]  sys_mode,        // Operation mode (00=MatMul, 01=Conv2D, 10=Accumulate)
    input  logic [7:0]  sys_rows,         // Number of rows to process
    input  logic        sys_signed,      // Signed/unsigned arithmetic
    input  logic        sys_transpose,    // Transpose input matrix
    input  logic [7:0]  sys_acc_addr,    // Accumulator write address
    input  logic        sys_acc_clear,   // Clear accumulator before write

    // Unified Buffer Control (7 signals)
    input  logic        ub_rd_en,        // UB read enable
    input  logic        ub_wr_en,         // UB write enable
    input  logic [8:0]  ub_rd_addr,       // Read address + bank select
    input  logic [8:0]  ub_wr_addr,       // Write address + bank select
    input  logic [8:0]  ub_rd_count,      // Read burst count
    input  logic [8:0]  ub_wr_count,      // Write burst count
    input  logic        ub_buf_sel,      // Bank selection

    // Weight FIFO Control (5 signals)
    input  logic        wt_mem_rd_en,    // Read from weight DRAM
    input  logic [23:0] wt_mem_addr,     // DRAM address
    input  logic        wt_fifo_wr,      // Weight FIFO write enable
    input  logic [7:0]  wt_num_tiles,     // Number of tiles to load
    input  logic        wt_buf_sel,      // Weight buffer selection

    // Accumulator Control (4 signals)
    input  logic        acc_wr_en,       // Accumulator write enable
    input  logic        acc_rd_en,       // Accumulator read enable
    input  logic [7:0]  acc_addr,        // Accumulator address
    input  logic        acc_buf_sel,     // Accumulator buffer selection

    // VPU Control (6 signals)
    input  logic        vpu_start,       // Start VPU operation
    input  logic [3:0]  vpu_mode,        // VPU function selection
    input  logic [7:0]  vpu_in_addr,     // VPU input address
    input  logic [7:0]  vpu_out_addr,    // VPU output address
    input  logic [7:0]  vpu_length,      // Number of elements
    input  logic [15:0] vpu_param,       // VPU operation parameter

    // Data interfaces
    input  logic [15:0] wt_fifo_data,    // Weight data input
    input  logic [255:0] ub_wr_data,     // Unified buffer write data
    output logic [255:0] ub_rd_data,     // Unified buffer read data
    output logic        ub_rd_valid,     // Unified buffer read data valid

    // Accumulator output for ST_UB (systolic array results)
    output logic [255:0] acc_data_out,   // Packed accumulator outputs for UB write

    // Status outputs to controller
    output logic        sys_busy,
    output logic        sys_done,
    output logic        vpu_busy,
    output logic        vpu_done,
    output logic        dma_busy,
    output logic        dma_done,
    output logic        wt_busy,         // Weight FIFO busy
    output logic        ub_busy,         // Unified buffer busy (for buffer toggle safety)
    output logic        ub_done          // Unified buffer done (for UART status)
);

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

// Systolic array connections (tinytinyTPU compatible)
logic [7:0] row0_act, row1_act, row2_act;  // Separate row activations
logic [7:0] col0_wt, col1_wt, col2_wt;     // Separate column weights
logic [31:0] acc0_out, acc1_out, acc2_out; // Direct accumulator outputs

// Latched accumulator outputs for ST_UB instruction
// These capture the PE outputs when sys_done pulses, providing stable data for ST_UB
logic [31:0] acc0_latched, acc1_latched, acc2_latched;

// Systolic controller signals
logic        en_weight_pass;
logic        en_capture_col0;
logic        en_capture_col1;
logic        en_capture_col2;
logic        systolic_active;

// Accumulator connections
logic [63:0] acc_wr_data;
logic [63:0] acc_rd_data;

// VPU connections
logic [255:0] vpu_out_data;
logic         vpu_out_valid;

// Unified buffer connections (double-buffered)
logic         ub_rd_valid;
logic         ub_wr_ready;
// ub_busy and ub_done are declared as output ports above, no need to redeclare

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
    .sys_mode        (sys_mode),
    .sys_rows        (sys_rows),
    .sys_signed      (sys_signed),
    .sys_transpose   (sys_transpose),
    .sys_acc_addr    (sys_acc_addr),
    .sys_acc_clear   (sys_acc_clear),
    .sys_busy        (sys_busy),
    .sys_done        (sys_done),
    .en_weight_pass  (en_weight_pass),
    .en_capture_col0 (en_capture_col0),
    .en_capture_col1 (en_capture_col1),
    .en_capture_col2 (en_capture_col2),
    .systolic_active (systolic_active),
    .acc_wr_en       (acc_wr_en),
    .acc_wr_addr     (acc_addr),
    .acc_clear       ()  // Not used - handled by sys_acc_clear
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

// Weight FIFO control logic
// For now, use the wt_fifo_wr signal directly for all columns
// In a full implementation, this would decode tile IDs and use wt_num_tiles
assign wf_push_col0 = wt_fifo_wr;
assign wf_push_col1 = wt_fifo_wr;
assign wf_push_col2 = wt_fifo_wr;
assign wf_pop = wt_rd_en;

// Weight FIFO busy logic - busy when DRAM read is active or FIFO is loading
assign wt_busy = wt_mem_rd_en || wt_fifo_wr;

// =============================================================================
// SYSTOLIC ARRAY (3x3 MMU)
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
    .wr_addr     (acc_addr),
    .wr_data     (acc_wr_data),
    .rd_en       (acc_rd_en),
    .rd_addr     (acc_addr),
    .rd_data     (acc_rd_data)
);

// Accumulator write data (from systolic array outputs)
assign acc_wr_data = {acc1_out, acc0_out};  // 64-bit: col1 + col0

// =============================================================================
// ACTIVATION PIPELINES (tinytinyTPU compatible)
// One pipeline per column for post-accumulator processing
// =============================================================================

// Activation pipeline configuration (runtime programmable from vpu_param)
logic signed [15:0] norm_gain;
logic signed [31:0] norm_bias;
logic [4:0]  norm_shift;
logic signed [15:0] q_inv_scale;
logic signed [7:0]  q_zero_point;

// Use controller-provided configuration or defaults
assign norm_gain = 16'h0100;     // Gain = 1.0 (Q8.8)
assign norm_bias = 32'sd0;       // No bias
assign norm_shift = 5'd0;        // No shift
assign q_inv_scale = 16'h0100;   // Scale = 1.0 (Q8.8)
assign q_zero_point = 8'sd0;     // Zero point = 0

// Extract individual accumulator outputs for pipelines
logic signed [31:0] acc_col0, acc_col1, acc_col2;
assign acc_col0 = acc_rd_data[31:0];    // Column 0
assign acc_col1 = acc_rd_data[63:32];   // Column 1
assign acc_col2 = 32'b0;                // Column 2 not stored (for now)

// Activation pipelines for each column
logic ap_valid_col0, ap_valid_col1, ap_valid_col2;
logic signed [7:0] ap_data_col0, ap_data_col1, ap_data_col2;
logic loss_valid_col0, loss_valid_col1, loss_valid_col2;
logic signed [31:0] loss_col0, loss_col1, loss_col2;

activation_pipeline ap_col0 (
    .clk(clk),
    .rst_n(rst_n),
    .valid_in(acc_rd_en),  // Trigger when accumulator read happens for any VPU operation
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
    .valid_in(acc_rd_en),  // Trigger when accumulator read happens for any VPU operation
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
    .valid_in(acc_rd_en),  // Trigger when accumulator read happens for any VPU operation
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

// VPU output generation based on mode
always_comb begin
    case (vpu_mode)
        4'h1, 4'h2, 4'h3, 4'h4: begin  // ReLU, ReLU6, Sigmoid, Tanh
            vpu_out_valid = ap_valid_col0 || ap_valid_col1 || ap_valid_col2;
            vpu_out_data = {8'b0, ap_data_col2, ap_data_col1, ap_data_col0};
        end
        4'h5: begin  // ADD_BIAS
            vpu_out_valid = ap_valid_col0 || ap_valid_col1 || ap_valid_col2;
            vpu_out_data = {8'b0, ap_data_col2, ap_data_col1, ap_data_col0};
        end
        4'h6, 4'h7: begin  // MAXPOOL, AVGPOOL
            // Pooling operations work on UB data directly
            vpu_out_valid = ub_rd_valid;
            vpu_out_data = ub_rd_data;  // Processed by pooling logic
        end
        4'h8: begin  // BATCH_NORM
            vpu_out_valid = ap_valid_col0 || ap_valid_col1 || ap_valid_col2;
            vpu_out_data = {8'b0, ap_data_col2, ap_data_col1, ap_data_col0};
        end
        default: begin
            vpu_out_valid = 1'b0;
            vpu_out_data = 256'h0;
        end
    endcase
end

// VPU busy logic
assign vpu_busy = vpu_start && !vpu_done;
assign vpu_done = vpu_out_valid;  // Done when output valid

// =============================================================================
// UNIFIED BUFFER (DOUBLE-BUFFERED)
// =============================================================================

unified_buffer ub (
    .clk             (clk),
    .rst_n           (rst_n),
    .ub_buf_sel      (ub_buf_sel),
    .ub_rd_en        (ub_rd_en),
    .ub_rd_addr      (ub_rd_addr),
    .ub_rd_count     (ub_rd_count),
    .ub_rd_data      (ub_rd_data),
    .ub_rd_valid     (ub_rd_valid),
    .ub_wr_en        (ub_wr_en),
    .ub_wr_addr      (ub_wr_addr),
    .ub_wr_count     (ub_wr_count),
    .ub_wr_data      (ub_wr_data),
    .ub_wr_ready     (ub_wr_ready),
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

// Column weights come from dual_weight_fifo module outputs (connected on lines 143-145)
// No need to assign from wt_fifo_data - that would create a conflict

// =============================================================================
// DATA FLOW CONTROL
// =============================================================================

// Weight FIFO read control (from systolic array)
assign wt_rd_en = systolic_active && !wt_rd_empty;

// DMA status simulation (in real implementation, this would connect to DMA controller)
// For now, DMA operations complete immediately
assign dma_busy = 1'b0;
assign dma_done = 1'b1;

// =============================================================================
// ACCUMULATOR OUTPUT LATCHING FOR ST_UB
// =============================================================================
// Continuously update latches during systolic_active (SYS_COMPUTE phase).
// When computation ends, the latches hold the last valid accumulator outputs.
// This ensures ST_UB reads stable values even after the systolic array stops.

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        acc0_latched <= 32'h0;
        acc1_latched <= 32'h0;
        acc2_latched <= 32'h0;
    end else if (systolic_active) begin
        // Continuously capture accumulator outputs during computation
        // The final values will be the last ones captured before systolic_active goes low
        acc0_latched <= acc0_out;
        acc1_latched <= acc1_out;
        acc2_latched <= acc2_out;
    end
    // Hold values when systolic_active is low - they remain stable for ST_UB
end

// =============================================================================
// ACCUMULATOR OUTPUT FOR ST_UB
// =============================================================================
// Pack the lower 8 bits of each LATCHED accumulator output into consecutive bytes.
// Layout: [255:24]=0 padding, [23:16]=acc2[7:0], [15:8]=acc1[7:0], [7:0]=acc0[7:0]
// This matches the Python expectation of int8 outputs in bytes 0, 1, 2.
// Using latched values ensures stable data when ST_UB executes.
assign acc_data_out = {232'b0, acc2_latched[7:0], acc1_latched[7:0], acc0_latched[7:0]};

endmodule
