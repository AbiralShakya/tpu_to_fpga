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

// Systolic array connections
logic [23:0] weight_to_sys;      // Weights to systolic array (3x8-bit)
logic [23:0] act_to_sys;         // Activations to systolic array (3x8-bit)
logic [95:0] psum_col0_out;      // Column 0 partial sums (3x32-bit)
logic [95:0] psum_col1_out;      // Column 1 partial sums (3x32-bit)
logic [95:0] psum_col2_out;      // Column 2 partial sums (3x32-bit)

// Systolic controller signals
logic        en_weight_pass;
logic        en_capture_col0;
logic        en_capture_col1;
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
    .systolic_active (systolic_active)
);

// =============================================================================
// WEIGHT FIFO (DOUBLE-BUFFERED)
// =============================================================================

weight_fifo weight_fifo_inst (
    .clk             (clk),
    .rst_n           (rst_n),
    .wt_buf_sel      (wt_buf_sel),
    .wt_num_tiles    (8'h10),        // Fixed for now
    .wr_en           (wt_fifo_wr),
    .wr_data         (wt_fifo_data),
    .wr_full         (wt_wr_full),
    .wr_almost_full  (),             // Not connected
    .rd_en           (wt_rd_en),
    .rd_data         (weight_to_sys),
    .rd_empty        (wt_rd_empty),
    .rd_almost_empty (),             // Not connected
    .wt_load_done    (wt_load_done)
);

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
    .weight_data     (weight_to_sys),
    .act_data        (act_to_sys),
    .psum_col0_out   (psum_col0_out),
    .psum_col1_out   (psum_col1_out),
    .psum_col2_out   (psum_col2_out)
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
// VPU (VECTOR PROCESSING UNIT)
// =============================================================================

vpu vpu_inst (
    .clk          (clk),
    .rst_n        (rst_n),
    .vpu_start    (vpu_start),
    .vpu_mode     (vpu_mode),
    .vpu_in_addr  (8'h00),        // Fixed for now
    .vpu_out_addr (8'h00),        // Fixed for now
    .vpu_length   (8'h08),        // Process 8 values
    .vpu_in_data  (acc_rd_data),
    .vpu_out_data (vpu_out_data),
    .vpu_out_valid(vpu_out_valid),
    .vpu_busy     (vpu_busy),
    .vpu_done     (vpu_done)
);

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
// DATA FLOW CONTROL
// =============================================================================

// Systolic array data flow
assign act_to_sys = ub_rd_data[15:0];  // First 16 bits as activations

// Accumulator write control
assign acc_wr_en = systolic_active;
assign acc_wr_addr = 8'h00;  // Fixed address for now
assign acc_wr_data = {psum_col2_out, psum_col1_out, psum_col0_out};  // Concatenate 3 columns

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
