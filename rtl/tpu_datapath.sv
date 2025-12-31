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

    // Accumulator Control (5 signals)
    input  logic        acc_wr_en,       // Accumulator write enable
    input  logic        acc_rd_en,       // Accumulator read enable
    input  logic [7:0]  acc_addr,        // Accumulator address
    input  logic        acc_buf_sel,     // Accumulator buffer selection
    input  logic [1:0]  st_ub_col_idx,   // ST_UB column index (0/1/2=capture, 3=write)

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
logic signed [31:0] acc0_out, acc1_out, acc2_out; // Direct accumulator outputs (signed wires from PE/MMU)

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
logic        sc_acc_wr_en;    // Accumulator write enable from systolic controller (writes all 3 columns)
logic [7:0]  sc_acc_wr_addr;  // Accumulator write address from systolic controller
logic        sc_acc_clear;    // Accumulator clear from systolic controller

// Accumulator connections (96-bit wide word: all 3 columns per address)
logic [95:0] acc_wr_data;
logic [95:0] acc_rd_data;
logic        acc_clear_busy;     // High while accumulator clear is in progress (256 cycles)
logic        acc_clear_complete; // Pulses high when clear finishes (for race-free handshaking)

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
logic [7:0]   weight_load_counter;  // From systolic controller for FIFO pop gating

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
    .acc_clear_busy  (acc_clear_busy),     // Wait for sequential clear (256 cycles)
    .acc_clear_complete (acc_clear_complete), // Pulses high when clear finishes
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
    .acc_clear       (sc_acc_clear),    // Accumulator clear from systolic controller
    .weight_load_cnt (weight_load_counter) // Weight load counter for FIFO pop gating
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
// CRITICAL: Delay FIFO writes by 1 cycle to compensate for BRAM read latency
// wt_mem has synchronous read - data arrives 1 cycle after wt_mem_rd_en
logic wt_fifo_wr_delayed;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        wt_fifo_wr_delayed <= 1'b0;
    end else begin
        wt_fifo_wr_delayed <= wt_fifo_wr;
    end
end

assign wf_push_col0 = wt_fifo_wr_delayed;
assign wf_push_col1 = wt_fifo_wr_delayed;
assign wf_push_col2 = wt_fifo_wr_delayed;

// CRITICAL FIX: Pop FIFO for all 7 cycles of weight loading
// The 2-cycle skew pipeline in col2 needs 5 pops (3 weights + 2 skew cycles)
// So we pop for cycles 1-7 to allow the skew pipeline to flush
// The FIFO internally limits actual pops to 3 weights per column
assign wf_pop = wt_rd_en && (weight_load_counter >= 8'h01) && (weight_load_counter <= 8'h07);

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

// Accumulator write data: Pack all 3 columns into 96-bit word
// Format: {Col2[31:0], Col1[31:0], Col0[31:0]}
// Write all 3 columns at once to a single address
assign acc_wr_data = sc_acc_wr_en ? 
    {acc2_out, acc1_out, acc0_out} :  // Pack all 3 columns for systolic writes
    {32'h0, 32'h0, acc0_out};          // Default for non-systolic writes (pad with zeros)

accumulator accumulators (
    .clk            (clk),
    .rst_n          (rst_n),
    .acc_buf_sel    (acc_buf_sel),
    .clear          (sc_acc_clear),         // Clear accumulator from systolic controller
    .clear_busy     (acc_clear_busy),       // High while clearing (256 cycles)
    .clear_complete (acc_clear_complete),   // Pulses high when clear finishes
    .wr_en          (acc_wr_en_combined),   // Combined write enable
    .wr_addr        (acc_wr_addr_combined), // Use systolic controller address when active
    .wr_data        (acc_wr_data),
    .rd_en          (acc_rd_en),
    .rd_addr        (acc_addr),
    .rd_data        (acc_rd_data)
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
// Accumulator stores all 3 columns per address: {Col2[31:0], Col1[31:0], Col0[31:0]}
logic signed [31:0] acc_col0, acc_col1, acc_col2;
assign acc_col0 = acc_rd_data[31:0];    // Column 0 (LSB)
assign acc_col1 = acc_rd_data[63:32];   // Column 1 (middle)
assign acc_col2 = acc_rd_data[95:64];  // Column 2 (MSB)

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

// ST_UB uses passthrough mode (no ReLU, no normalization, just clamping)
// Passthrough must be active when data enters pipeline and remain stable for pipeline latency
// Latch passthrough when acc_rd_en is asserted (ST_UB read), clear when ST_UB completes
logic st_ub_passthrough, st_ub_passthrough_reg;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        st_ub_passthrough_reg <= 1'b0;
    end else begin
        // Set when ST_UB reads (acc_rd_en asserted), clear when ST_UB write completes (st_ub_col_idx == 3)
        if (acc_rd_en) begin
            st_ub_passthrough_reg <= 1'b1;  // Latch passthrough when read starts
        end else if (st_ub_col_idx == 2'd3) begin
            st_ub_passthrough_reg <= 1'b0;  // Clear when write completes
        end
    end
end
assign st_ub_passthrough = st_ub_passthrough_reg;

activation_pipeline ap_col0 (
    .clk(clk),
    .rst_n(rst_n),
    .valid_in(acc_rd_en_delayed),  // Delayed by 1 cycle to match accumulator registered output
    .acc_in(acc_col0),
    .target_in(32'sd0),    // No loss computation for inference
    .passthrough(st_ub_passthrough),  // Passthrough for ST_UB
    .unsigned_mode(!stored_signed_mode),  // Unsigned if not signed mode
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
    .passthrough(st_ub_passthrough),  // Passthrough for ST_UB
    .unsigned_mode(!stored_signed_mode),  // Unsigned if not signed mode
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
    .passthrough(st_ub_passthrough),  // Passthrough for ST_UB
    .unsigned_mode(!stored_signed_mode),  // Unsigned if not signed mode
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
// ST_UB reads from accumulator memory
// For 3x3 MATMUL: All 3 columns stored at single address in 96-bit word
// Format: {Col2[31:0], Col1[31:0], Col0[31:0]}
// Extract and clamp each column to 8-bit for int8 output

// CRITICAL: BRAM already provides 1-cycle registered read latency!
// accumulator.sv uses: always_ff @(posedge clk) if (rd_en) rd_data <= memory[addr];
// So acc_rd_data is ALREADY delayed by 1 cycle. Don't add another delay!
logic acc_rd_en_prev;  // Track when read was issued

// Store signed mode from last MATMUL (needed for clamping)
logic stored_signed_mode;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        stored_signed_mode <= 1'b0;
    end else if (sys_start && (sys_mode == 2'b00)) begin  // MATMUL operation
        stored_signed_mode <= sys_signed;  // Store signed mode from MATMUL
    end
end

// ST_UB byte accumulation: Use activation pipeline outputs (passthrough mode)
// Activation pipelines process all 3 columns and output clamped values
// Pipeline delay: activation (1 cycle) -> normalization (1 cycle) -> quantization (1 cycle) = 3 cycles total
// But in passthrough mode: activation (1 cycle, passthrough) -> quantization (1 cycle, direct clamp) = 2 cycles
logic [7:0] st_ub_byte0, st_ub_byte1, st_ub_byte2;

// Capture activation pipeline outputs when valid during ST_UB write phase
// The pipelines are in passthrough mode, so they just clamp the values
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        st_ub_byte0 <= 8'h00;
        st_ub_byte1 <= 8'h00;
        st_ub_byte2 <= 8'h00;
    end else begin
        // Capture from activation pipelines when valid and ST_UB is in write phase
        if (ap_valid_col0 && (st_ub_col_idx == 2'd3)) begin
            st_ub_byte0 <= ap_data_col0;
        end
        if (ap_valid_col1 && (st_ub_col_idx == 2'd3)) begin
            st_ub_byte1 <= ap_data_col1;
        end
        if (ap_valid_col2 && (st_ub_col_idx == 2'd3)) begin
            st_ub_byte2 <= ap_data_col2;
        end
    end
end

// For non-ST_UB operations, keep acc_rd_data in a register for other uses
logic [95:0] acc_rd_data_reg;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        acc_rd_data_reg <= 96'h0;
    end else if (acc_rd_en_prev) begin
        acc_rd_data_reg <= acc_rd_data;
    end
end

// Extract 32-bit column value from 96-bit accumulator read (for non-ST_UB operations)
// ST_UB reads all 3 columns at once, extracts based on st_ub_col_idx_prev
logic [31:0] acc_result_selected;
assign acc_result_selected = (st_ub_col_idx_prev == 2'd0) ? acc_rd_data[31:0] :
                             (st_ub_col_idx_prev == 2'd1) ? acc_rd_data[63:32] :
                             acc_rd_data[95:64];

logic signed [31:0] acc_result_signed;
logic [31:0] acc_result_unsigned;
logic [7:0] acc_result_clamped;

// CRITICAL: Use acc_rd_data directly (BRAM output), not acc_rd_data_reg (adds extra delay)
assign acc_result_signed = $signed(acc_result_selected);
assign acc_result_unsigned = acc_result_selected;

// Clamp based on signed/unsigned mode
// Signed: clamp to [-128, 127]
// Unsigned: clamp to [0, 255]
assign acc_result_clamped = stored_signed_mode ?
    // Signed mode: clamp to [-128, 127]
    ((acc_result_signed > 32'sd127) ? 8'd127 :
     (acc_result_signed < -32'sd128) ? 8'd128 :  // 128 = -128 in 2's complement
     acc_result_selected[7:0]) :
    // Unsigned mode: clamp to [0, 255]
    ((acc_result_unsigned > 32'd255) ? 8'd255 :
     acc_result_selected[7:0]);

// For ST_UB, write all 3 bytes as a single 256-bit word
// When st_ub_col_idx == 3 (write phase), output accumulated bytes [byte2, byte1, byte0]
// Otherwise output single byte (for compatibility with other operations)
assign acc_data_out = (st_ub_col_idx == 2'd3) ?
    {232'b0, st_ub_byte2, st_ub_byte1, st_ub_byte0} :  // ST_UB write: all 3 bytes at [23:0]
    {248'b0, acc_result_clamped};                      // Other ops: single byte at [7:0]

endmodule
