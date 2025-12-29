`timescale 1ns / 1ps

module tpu_datapath (
    input  logic        clk,
    input  logic        rst_n,

    // =============================================================================
    // CONTROL SIGNALS FROM CONTROLLER (22 signals)
    // =============================================================================

    // Systolic Array Control (6 signals)
    input  logic        sys_start,       // Start systolic operation
    input  logic [1:0]  sys_mode,        // Operation mode (00=MatMul, 01=Conv2D, 10=Accumulate)
    input  logic [7:0]  sys_rows,         // Number of rows to process
    input  logic        sys_signed,      // Signed/unsigned arithmetic
    input  logic [7:0]  sys_acc_addr,    // Accumulator write address
    input  logic        sys_acc_clear,   // Clear accumulator before write

    // Unified Buffer Control (6 signals)
    input  logic        ub_rd_en,        // UB read enable
    input  logic        ub_wr_en,         // UB write enable
    input  logic [8:0]  ub_rd_addr,       // Read address + bank select
    input  logic [8:0]  ub_wr_addr,       // Write address + bank select
    input  logic [8:0]  ub_rd_count,      // Read burst count
    input  logic [8:0]  ub_wr_count,      // Write burst count

    // Weight FIFO Control (3 signals)
    input  logic        wt_mem_rd_en,    // Read from weight DRAM
    input  logic        wt_fifo_wr,      // Weight FIFO write enable
    input  logic [7:0]  wt_num_tiles,     // Number of tiles to load

    // Accumulator Control (4 signals)
    input  logic        acc_wr_en,       // Accumulator write enable
    input  logic        acc_rd_en,       // Accumulator read enable
    input  logic [7:0]  acc_addr,        // Accumulator address
    input  logic        acc_buf_sel,     // Accumulator buffer selection

    // VPU Control (3 signals)
    input  logic        vpu_start,       // Start VPU operation
    input  logic [3:0]  vpu_mode,        // VPU function selection
    input  logic [15:0] vpu_param,       // VPU operation parameter

    // Data interfaces
    input  logic [63:0] wt_fifo_data,    // Weight row data (64 bits = 8 bytes)
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
    output logic        ub_done,         // Unified buffer done (for UART status)

    // DEBUG OUTPUTS - for MATMUL troubleshooting
    output logic [7:0]  debug_col0_wt, debug_col1_wt, debug_col2_wt,
    output logic [7:0]  debug_row0_act_latched, debug_row1_act_latched, debug_row2_act_latched,
    output logic        debug_systolic_active,
    output logic        debug_en_weight_pass,
    output logic [31:0] debug_acc0_latched, debug_acc1_latched, debug_acc2_latched
);

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

// Systolic array connections (tinytinyTPU compatible)
logic [7:0] row0_act, row1_act, row2_act;  // Separate row activations
logic [7:0] col0_wt, col1_wt, col2_wt;     // Separate column weights
logic [31:0] acc0_out, acc1_out, acc2_out; // Direct accumulator outputs

// CRITICAL FIX: Latch activations when UB read completes, hold during MATMUL
logic [7:0] row0_act_latched, row1_act_latched, row2_act_latched;

// Latched accumulator outputs for ST_UB instruction
// These capture the PE outputs when sys_done pulses, providing stable data for ST_UB
logic [31:0] acc0_latched, acc1_latched, acc2_latched;

// Systolic controller signals
logic        en_weight_pass;
logic        en_weight_pass_prev;  // Previous cycle value to detect transitions
logic        weight_load_start;    // Asserted when weight loading sequence begins
// Row+column specific capture signals
logic        en_capture_row0_col0, en_capture_row0_col1, en_capture_row0_col2;
logic        en_capture_row1_col0, en_capture_row1_col1, en_capture_row1_col2;
logic        en_capture_row2_col0, en_capture_row2_col1, en_capture_row2_col2;
logic        systolic_active;

// Systolic controller accumulator outputs (separate from input ports to avoid multi-driver)
logic        sc_acc_wr_en;    // Accumulator write enable from systolic controller
logic [7:0]  sc_acc_wr_addr;  // Accumulator write address from systolic controller
logic        sc_acc_wr_col01; // Write acc0+acc1 (even addresses)
logic        sc_acc_wr_col2;  // Write acc2 (odd addresses)

// Accumulator connections
logic [63:0] acc_wr_data;
logic [63:0] acc_rd_data;

// VPU connections
logic [255:0] vpu_out_data;
logic         vpu_out_valid;

// Unified buffer connections (double-buffered)
// NOTE: ub_rd_valid is declared as OUTPUT PORT above - do NOT redeclare here!
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
    .sys_acc_addr    (sys_acc_addr),
    .sys_acc_clear   (sys_acc_clear),
    .sys_busy        (sys_busy),
    .sys_done        (sys_done),
    .en_weight_pass  (en_weight_pass),
    .en_capture_row0_col0 (en_capture_row0_col0),
    .en_capture_row0_col1 (en_capture_row0_col1),
    .en_capture_row0_col2 (en_capture_row0_col2),
    .en_capture_row1_col0 (en_capture_row1_col0),
    .en_capture_row1_col1 (en_capture_row1_col1),
    .en_capture_row1_col2 (en_capture_row1_col2),
    .en_capture_row2_col0 (en_capture_row2_col0),
    .en_capture_row2_col1 (en_capture_row2_col1),
    .en_capture_row2_col2 (en_capture_row2_col2),
    .systolic_active (systolic_active),
    .acc_wr_en       (sc_acc_wr_en),    // Use internal signal, not input port
    .acc_wr_addr     (sc_acc_wr_addr),  // Use internal signal, not input port
    .acc_wr_col01    (sc_acc_wr_col01), // Column selection: acc0+acc1
    .acc_wr_col2     (sc_acc_wr_col2),  // Column selection: acc2
    .acc_clear       ()  // Not used - handled by sys_acc_clear
);

// =============================================================================
// WEIGHT FIFO (DOUBLE-BUFFERED)
// =============================================================================

// Dual Weight FIFO (tinytinyTPU compatible)
logic wf_push_col0, wf_push_col1, wf_push_col2;
logic wf_pop;

dual_weight_fifo weight_fifo_inst (
    .clk              (clk),
    .rst_n            (rst_n),
    .push_col0        (wf_push_col0),
    .push_col1        (wf_push_col1),
    .push_col2        (wf_push_col2),
    .data_in_col0     (wt_fifo_data[7:0]),    // Byte 0 -> Column 0 weights
    .data_in_col1     (wt_fifo_data[15:8]),   // Byte 1 -> Column 1 weights
    .data_in_col2     (wt_fifo_data[23:16]),  // Byte 2 -> Column 2 weights
    .pop              (wf_pop),
    .weight_load_start(weight_load_start),    // Signal to reset delay counters
    .col0_out         (col0_wt),
    .col1_out         (col1_wt),
    .col2_out         (col2_wt),
    .col1_raw         (),                     // Not used
    .col2_raw         ()                      // Not used
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
    .en_capture_row0_col0 (en_capture_row0_col0),
    .en_capture_row0_col1 (en_capture_row0_col1),
    .en_capture_row0_col2 (en_capture_row0_col2),
    .en_capture_row1_col0 (en_capture_row1_col0),
    .en_capture_row1_col1 (en_capture_row1_col1),
    .en_capture_row1_col2 (en_capture_row1_col2),
    .en_capture_row2_col0 (en_capture_row2_col0),
    .en_capture_row2_col1 (en_capture_row2_col1),
    .en_capture_row2_col2 (en_capture_row2_col2),
    .use_signed      (sys_signed),            // Signed arithmetic control from controller
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

// Accumulator write control: OR systolic controller output with direct controller input
// This allows both MATMUL (via systolic_ctrl) and direct writes to work
logic acc_wr_en_combined;
assign acc_wr_en_combined = sc_acc_wr_en | acc_wr_en;

// Accumulator write address: Use systolic controller address when active, else controller address
logic [7:0] acc_wr_addr_combined;
assign acc_wr_addr_combined = sc_acc_wr_en ? sc_acc_wr_addr : acc_addr;

// Accumulator write data selection: Handle all 3 columns (acc0, acc1, acc2)
// Since accumulator is 64-bit, we pad each 32-bit result to 64 bits
// For 3x3 MATMUL: Write acc0 (addr), acc1 (addr+1), acc2 (addr+2) separately
// acc_wr_col01 is true for acc0 (offset 0) and acc1 (offset 1)
// acc_wr_col2 is true for acc2 (offset 2)
// Use address LSB to distinguish acc0 (LSB=0) from acc1 (LSB=1)
logic [7:0] acc_wr_addr_offset;
assign acc_wr_addr_offset = sc_acc_wr_addr - sys_acc_addr;

logic [63:0] acc_wr_data_col0, acc_wr_data_col1, acc_wr_data_col2;
assign acc_wr_data_col0 = {32'h0, acc0_out};      // 64-bit: acc0 padded
assign acc_wr_data_col1 = {32'h0, acc1_out};      // 64-bit: acc1 padded
assign acc_wr_data_col2 = {32'h0, acc2_out};      // 64-bit: acc2 padded

// Select data based on address offset (which column we're writing)
assign acc_wr_data = sc_acc_wr_en ? 
    ((acc_wr_addr_offset[1:0] == 2'd0) ? acc_wr_data_col0 :
     (acc_wr_addr_offset[1:0] == 2'd1) ? acc_wr_data_col1 :
     acc_wr_data_col2) :
    {acc1_out, acc0_out};  // Default for non-systolic writes

accumulator accumulators (
    .clk         (clk),
    .rst_n       (rst_n),
    .acc_buf_sel (acc_buf_sel),
    .wr_en       (acc_wr_en_combined),  // Combined write enable
    .wr_addr     (acc_wr_addr_combined), // Use systolic controller address when active
    .wr_data     (acc_wr_data),
    .rd_en       (acc_rd_en),
    .rd_addr     (acc_addr),
    .rd_data     (acc_rd_data)
);

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

// Delay valid signal by 1 cycle to match accumulator's registered read output
// Accumulator read is synchronous: when acc_rd_en is asserted, data appears on next cycle
logic acc_rd_en_delayed;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        acc_rd_en_delayed <= 1'b0;
    end else begin
        acc_rd_en_delayed <= acc_rd_en;
    end
end

// Activation pipelines for each column
logic ap_valid_col0, ap_valid_col1, ap_valid_col2;
logic signed [7:0] ap_data_col0, ap_data_col1, ap_data_col2;
logic loss_valid_col0, loss_valid_col1, loss_valid_col2;
logic signed [31:0] loss_col0, loss_col1, loss_col2;

activation_pipeline ap_col0 (
    .clk(clk),
    .rst_n(rst_n),
    .valid_in(acc_rd_en_delayed),  // Delayed by 1 cycle to match accumulator registered output
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
    .valid_in(acc_rd_en_delayed),  // Delayed by 1 cycle to match accumulator registered output
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
    .valid_in(acc_rd_en_delayed),  // Delayed by 1 cycle to match accumulator registered output
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
    .ub_rd_en        (ub_rd_en),
    .ub_rd_addr      (ub_rd_addr),
    .ub_rd_data      (ub_rd_data),
    .ub_rd_valid     (ub_rd_valid),
    .ub_wr_en        (ub_wr_en),
    .ub_wr_addr      (ub_wr_addr),
    .ub_wr_data      (ub_wr_data),
    .ub_wr_ready     (ub_wr_ready),
    .ub_busy         (ub_busy),
    .ub_done         (ub_done)
);

// =============================================================================
// DATA EXTRACTION FOR SYSTOLIC ARRAY (tinytinyTPU compatible)
// =============================================================================

// CRITICAL FIX: Latch activations from UB when valid, hold during MATMUL
// The UB outputs data for only 1 cycle after LD_UB, but MATMUL needs it for multiple cycles
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        row0_act_latched <= 8'b0;
        row1_act_latched <= 8'b0;
        row2_act_latched <= 8'b0;
    end else if (ub_rd_valid) begin
        // Latch when UB read completes
        row0_act_latched <= ub_rd_data[7:0];
        row1_act_latched <= ub_rd_data[15:8];
        row2_act_latched <= ub_rd_data[23:16];
    end
    // Hold values otherwise (for MATMUL to use)
end

// Use latched activations for systolic array
assign row0_act = row0_act_latched;
assign row1_act = row1_act_latched;
assign row2_act = row2_act_latched;

// Column weights come from dual_weight_fifo module outputs (connected on lines 143-145)
// No need to assign from wt_fifo_data - that would create a conflict

// =============================================================================
// DATA FLOW CONTROL
// =============================================================================

// Weight FIFO read control (from systolic array)
// CRITICAL: Pop weights during en_weight_pass (SYS_LOAD_WEIGHTS), not during systolic_active (SYS_COMPUTE)
// Note: wt_rd_empty is unused (FIFO has no empty flag), so we just use en_weight_pass
assign wt_rd_en = en_weight_pass;

// Detect start of weight loading sequence (transition from 0 to 1)
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        en_weight_pass_prev <= 1'b0;
    end else begin
        en_weight_pass_prev <= en_weight_pass;
    end
end
assign weight_load_start = en_weight_pass && !en_weight_pass_prev;

// DMA status simulation (in real implementation, this would connect to DMA controller)
// For now, DMA operations complete immediately
assign dma_busy = 1'b0;
assign dma_done = 1'b1;

// =============================================================================
// ACCUMULATOR OUTPUT LATCHING FOR ST_UB
// =============================================================================
// Capture final accumulator outputs at the end of SYS_COMPUTE phase.
// This ensures ST_UB reads the final accumulated results, not intermediate values.

logic systolic_active_prev;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        systolic_active_prev <= 1'b0;
    end else begin
        systolic_active_prev <= systolic_active;
    end
end

// CRITICAL FIX: Latch accumulator outputs continuously during SYS_COMPUTE
// This ensures we capture the final stable values, not just on the falling edge
// The PE outputs are registered, so they're stable each cycle during computation
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        acc0_latched <= 32'h0;
        acc1_latched <= 32'h0;
        acc2_latched <= 32'h0;
    end else if (systolic_active) begin
        // Continuously update during computation - final values will be the last ones
        acc0_latched <= acc0_out;
        acc1_latched <= acc1_out;
        acc2_latched <= acc2_out;
    end
    // Hold values after computation completes for ST_UB to read
end

// DEBUG OUTPUT ASSIGNMENTS
assign debug_col0_wt = col0_wt;
assign debug_col1_wt = col1_wt;
assign debug_col2_wt = col2_wt;
assign debug_row0_act_latched = row0_act_latched;
assign debug_row1_act_latched = row1_act_latched;
assign debug_row2_act_latched = row2_act_latched;
assign debug_systolic_active = systolic_active;
assign debug_en_weight_pass = en_weight_pass;
assign debug_acc0_latched = acc0_latched;
assign debug_acc1_latched = acc1_latched;
assign debug_acc2_latched = acc2_latched;

// =============================================================================
// ACCUMULATOR OUTPUT FOR ST_UB
// =============================================================================
// CRITICAL FIX: Read from accumulator memory, not latched PE outputs
// For 3x3 MATMUL: Results are stored at addresses 0, 1, 2
// Each 64-bit accumulator word contains one 32-bit result in the lower 32 bits
// Extract the lower 8 bits of each result for int8 output
// Layout: [255:24]=0 padding, [23:16]=acc2[7:0], [15:8]=acc1[7:0], [7:0]=acc0[7:0]

// CRITICAL FIX: Use latched PE outputs for ST_UB
// The latched values (acc0_latched, acc1_latched, acc2_latched) capture the final
// PE outputs when SYS_COMPUTE completes. These should match what's in accumulator memory.
// For now, use these latched values directly for ST_UB.
// TODO: In the future, ST_UB could read from accumulator memory for more flexibility,
// but that requires multi-cycle reads which is more complex.
assign acc_data_out = {232'b0, acc2_latched[7:0], acc1_latched[7:0], acc0_latched[7:0]};

endmodule
