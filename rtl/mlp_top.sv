// =============================================================================
// Top-level MLP integration module for cocotb testing (3x3 TPU version)
// Based on tinytinyTPU but extended for 3x3 systolic array
// =============================================================================

`timescale 1ns / 1ps

module mlp_top (
    input  logic        clk,
    input  logic        rst_n,

    // Weight FIFO interface (extended for 3 columns)
    input  logic        wf_push_col0,
    input  logic        wf_push_col1,
    input  logic        wf_push_col2,
    input  logic [7:0]  wf_data_in,
    input  logic        wf_reset,

    // Initial activation loading (24-bit for 3x8-bit values)
    input  logic        init_act_valid,
    input  logic [23:0] init_act_data,

    // Control
    input  logic        start_mlp,
    input  logic        weights_ready,

    // Activation pipeline configuration
    input  logic signed [15:0] norm_gain,
    input  logic signed [31:0] norm_bias,
    input  logic [4:0]  norm_shift,
    input  logic signed [15:0] q_inv_scale,
    input  logic signed [7:0]  q_zero_point,

    // Status outputs
    output logic [3:0]  state,
    output logic [5:0]  cycle_cnt,      // Extended for 3x3 timing
    output logic [2:0]  current_layer,
    output logic        layer_complete,

    // Debug outputs (extended for 3 columns)
    output logic [31:0] mmu_acc0_out,
    output logic [31:0] mmu_acc1_out,
    output logic [31:0] mmu_acc2_out,
    output logic signed [31:0] acc0,
    output logic signed [31:0] acc1,
    output logic signed [31:0] acc2,
    output logic        acc_valid
);

    // FSM States (extended for 3x3 timing)
    localparam logic [3:0] IDLE         = 4'd0;
    localparam logic [3:0] LOAD_WEIGHT  = 4'd1;
    localparam logic [3:0] LOAD_ACT     = 4'd2;
    localparam logic [3:0] COMPUTE      = 4'd3;
    localparam logic [3:0] DRAIN        = 4'd4;
    localparam logic [3:0] TRANSFER     = 4'd5;
    localparam logic [3:0] NEXT_LAYER   = 4'd6;
    localparam logic [3:0] WAIT_WEIGHTS = 4'd7;
    localparam logic [3:0] DONE         = 4'd8;

    localparam int NUM_LAYERS = 2;

    // Internal state
    logic [3:0]  state_reg;
    logic [5:0]  cycle_cnt_reg;  // Extended counter for 3x3 timing
    logic [2:0]  current_layer_reg;
    logic        buffer_select;
    logic        weights_loaded;

    assign state = state_reg;
    assign cycle_cnt = cycle_cnt_reg;
    assign current_layer = current_layer_reg;

    // Derived control signals
    logic en_load_weight;
    logic compute_phase;
    logic drain_phase;
    logic transfer_phase;

    assign en_load_weight = (state_reg == LOAD_WEIGHT);
    assign compute_phase  = (state_reg == COMPUTE);
    assign drain_phase    = (state_reg == DRAIN);
    assign transfer_phase = (state_reg == TRANSFER);

    // Dual Weight FIFO (extended for 3 columns)
    logic wf_pop;

    dual_weight_fifo weight_fifo_u (
        .clk       (clk),
        .rst_n     (rst_n),
        .push_col0 (wf_push_col0),
        .push_col1 (wf_push_col1),
        .push_col2 (wf_push_col2),
        .data_in   (wf_data_in),
        .pop       (wf_pop),
        .col0_out  (),  // Connected internally
        .col1_out  (),  // Connected internally
        .col2_out  (),  // Connected internally
        .col1_raw  (),
        .col2_raw  ()
    );

    // Unified Buffers (24-bit for 3x8-bit values, double buffered)
    logic        ub_a_wr_valid, ub_a_rd_ready;
    logic [23:0] ub_a_wr_data, ub_a_rd_data;
    logic        ub_a_wr_ready, ub_a_rd_valid, ub_a_empty;

    logic        ub_b_wr_valid, ub_b_rd_ready;
    logic [23:0] ub_b_wr_data, ub_b_rd_data;
    logic        ub_b_wr_ready, ub_b_rd_valid, ub_b_empty;

    // Note: Using simplified unified_buffer instantiation
    // In a real implementation, you'd instantiate two unified_buffer modules

    // Buffer selection muxing
    logic        act_ub_rd_valid;
    logic [23:0] act_ub_rd_data;
    logic        act_ub_empty;

    assign act_ub_rd_valid = buffer_select ? ub_b_rd_valid : ub_a_rd_valid;
    assign act_ub_rd_data  = buffer_select ? ub_b_rd_data  : ub_a_rd_data;
    assign act_ub_empty    = buffer_select ? ub_b_empty    : ub_a_empty;

    always_comb begin
        ub_a_rd_ready = 1'b0;
        ub_b_rd_ready = 1'b0;
        if (compute_phase) begin
            if (buffer_select)
                ub_b_rd_ready = 1'b1;
            else
                ub_a_rd_ready = 1'b1;
        end
    end

    // Activation data extraction (3x8-bit values)
    logic [7:0] act_row0_data, act_row1_data, act_row2_data;
    assign act_row0_data = act_ub_rd_data[7:0];
    assign act_row1_data = act_ub_rd_data[15:8];
    assign act_row2_data = act_ub_rd_data[23:16];

    // Row skew registers for systolic timing (3x3 array)
    logic [7:0] row1_skew_reg, row2_skew_reg;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            row1_skew_reg <= 8'd0;
            row2_skew_reg <= 8'd0;
        end else begin
            // Row1 gets 1-cycle delay, Row2 gets 2-cycle delay for 3x3 wavefront
            row1_skew_reg <= act_row1_data;
            row2_skew_reg <= row1_skew_reg;
        end
    end

    // MMU connections
    logic en_capture_col0, en_capture_col1, en_capture_col2;
    assign en_capture_col0 = en_load_weight && (cycle_cnt_reg == 6'd1);
    assign en_capture_col1 = en_load_weight && (cycle_cnt_reg == 6'd2);
    assign en_capture_col2 = en_load_weight && (cycle_cnt_reg == 6'd3);  // 3rd column

    mmu systolic_array (
        .clk             (clk),
        .rst_n           (rst_n),
        .en_weight_pass  (en_load_weight),
        .en_capture_col0 (en_capture_col0),
        .en_capture_col1 (en_capture_col1),
        .en_capture_col2 (en_capture_col2),
        .row0_in         (compute_phase && act_ub_rd_valid ? act_row0_data : 8'd0),
        .row1_in         (compute_phase ? row1_skew_reg : 8'd0),
        .row2_in         (compute_phase ? row2_skew_reg : 8'd0),
        .col0_in         (8'd0),  // Would come from weight FIFO
        .col1_in         (8'd0),  // Would come from weight FIFO
        .col2_in         (8'd0),  // Would come from weight FIFO
        .acc0_out        (mmu_acc0_out),
        .acc1_out        (mmu_acc1_out),
        .acc2_out        (mmu_acc2_out)
    );

    // Accumulator (using our modular design)
    logic accum_en;
    logic addr_sel;
    logic mmu_valid;

    assign mmu_valid = (compute_phase && cycle_cnt_reg >= 6'd3) || drain_phase;  // Adjusted for 3x3 timing

    accumulator accumulator_u (
        .clk               (clk),
        .rst_n             (rst_n),
        .valid_in          (mmu_valid),
        .accumulator_enable(accum_en),
        .addr_sel          (addr_sel),
        .mmu_col0_in       (mmu_acc0_out),
        .mmu_col1_in       (mmu_acc1_out),
        .mmu_col2_in       (mmu_acc2_out),
        .acc_col0_out      (acc0),
        .acc_col1_out      (acc1),
        .acc_col2_out      (acc2),
        .valid_out         (acc_valid)
    );

    // Activation Pipelines (one per column, our extension)
    logic ap_valid_col0, ap_valid_col1, ap_valid_col2;
    logic signed [7:0] ap_data_col0, ap_data_col1, ap_data_col2;

    activation_pipeline ap_col0 (
        .clk(clk),
        .rst_n(rst_n),
        .valid_in(acc_valid),
        .acc_in(acc0),
        .target_in(32'sd0),
        .norm_gain(norm_gain),
        .norm_bias(norm_bias),
        .norm_shift(norm_shift),
        .q_inv_scale(q_inv_scale),
        .q_zero_point(q_zero_point),
        .valid_out(ap_valid_col0),
        .ub_data_out(ap_data_col0),
        .loss_valid(),
        .loss_out()
    );

    activation_pipeline ap_col1 (
        .clk(clk),
        .rst_n(rst_n),
        .valid_in(acc_valid),
        .acc_in(acc1),
        .target_in(32'sd0),
        .norm_gain(norm_gain),
        .norm_bias(norm_bias),
        .norm_shift(norm_shift),
        .q_inv_scale(q_inv_scale),
        .q_zero_point(q_zero_point),
        .valid_out(ap_valid_col1),
        .ub_data_out(ap_data_col1),
        .loss_valid(),
        .loss_out()
    );

    activation_pipeline ap_col2 (
        .clk(clk),
        .rst_n(rst_n),
        .valid_in(acc_valid),
        .acc_in(acc2),
        .target_in(32'sd0),
        .norm_gain(norm_gain),
        .norm_bias(norm_bias),
        .norm_shift(norm_shift),
        .q_inv_scale(q_inv_scale),
        .q_zero_point(q_zero_point),
        .valid_out(ap_valid_col2),
        .ub_data_out(ap_data_col2),
        .loss_valid(),
        .loss_out()
    );

    // Output repacking for UB refill (24-bit for 3x8-bit values)
    logic        refill_valid;
    logic [23:0] refill_data;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            refill_valid <= 1'b0;
            refill_data <= 24'd0;
        end else begin
            refill_valid <= 1'b0;
            if (ap_valid_col0 || ap_valid_col1 || ap_valid_col2) begin
                refill_data <= {ap_data_col2, ap_data_col1, ap_data_col0};
                refill_valid <= 1'b1;
            end
        end
    end

    // FSM Controller (adapted for 3x3 timing)
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state_reg <= IDLE;
            cycle_cnt_reg <= 6'd0;
            current_layer_reg <= 3'd0;
            buffer_select <= 1'b0;
            weights_loaded <= 1'b0;
            layer_complete <= 1'b0;
            accum_en <= 1'b0;
            addr_sel <= 1'b0;
        end else begin
            case (state_reg)
                IDLE: begin
                    cycle_cnt_reg <= 6'd0;
                    if (start_mlp) begin
                        state_reg <= LOAD_WEIGHT;
                        current_layer_reg <= 3'd0;
                        buffer_select <= 1'b0;
                    end
                end

                LOAD_WEIGHT: begin
                    cycle_cnt_reg <= cycle_cnt_reg + 1'd1;
                    if (cycle_cnt_reg == 6'd3) begin  // 3 cycles for 3x3 weight loading
                        state_reg <= (current_layer_reg == 0) ? LOAD_ACT : COMPUTE;
                        cycle_cnt_reg <= 6'd0;
                        weights_loaded <= 1'b1;
                    end
                end

                LOAD_ACT: begin
                    cycle_cnt_reg <= cycle_cnt_reg + 1'd1;
                    if (cycle_cnt_reg == 6'd3) begin  // 3 cycles for activation loading
                        state_reg <= COMPUTE;
                        cycle_cnt_reg <= 6'd0;
                    end
                end

                COMPUTE: begin
                    cycle_cnt_reg <= cycle_cnt_reg + 1'd1;
                    if (cycle_cnt_reg == 6'd3) begin  // 3 cycles for 3x3 computation
                        state_reg <= DRAIN;
                        cycle_cnt_reg <= 6'd0;
                    end
                end

                DRAIN: begin
                    cycle_cnt_reg <= cycle_cnt_reg + 1'd1;
                    if (cycle_cnt_reg == 6'd9) begin  // Extended drain time for 3x3
                        if (current_layer_reg < 3'(NUM_LAYERS - 1)) begin
                            state_reg <= TRANSFER;
                            cycle_cnt_reg <= 6'd0;
                        end else begin
                            state_reg <= DONE;
                            cycle_cnt_reg <= 6'd0;
                        end
                    end
                end

                TRANSFER: begin
                    cycle_cnt_reg <= cycle_cnt_reg + 1'd1;
                    if (cycle_cnt_reg == 6'd3) begin
                        state_reg <= NEXT_LAYER;
                        cycle_cnt_reg <= 6'd0;
                    end
                end

                NEXT_LAYER: begin
                    buffer_select <= ~buffer_select;
                    current_layer_reg <= current_layer_reg + 1'd1;
                    state_reg <= WAIT_WEIGHTS;
                    cycle_cnt_reg <= 6'd0;
                    weights_loaded <= 1'b0;
                end

                WAIT_WEIGHTS: begin
                    cycle_cnt_reg <= cycle_cnt_reg + 1'd1;
                    if (weights_ready) begin
                        state_reg <= LOAD_WEIGHT;
                        cycle_cnt_reg <= 6'd0;
                    end
                end

                DONE: begin
                    layer_complete <= 1'b1;
                end

                default: state_reg <= IDLE;
            endcase
        end
    end

endmodule
