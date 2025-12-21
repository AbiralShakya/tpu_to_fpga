`timescale 1ns / 1ps

module tpu_controller (
    input  logic        clk,
    input  logic        rst_n,

    // Instruction memory interface (for simplicity, we'll use a direct interface)
    // In real implementation, this would be connected to external memory
    output logic [7:0]  instr_addr,     // Instruction address to memory
    input  logic [31:0] instr_data,     // Instruction data from memory

    // Status inputs from datapath (for hazard detection)
    input  logic        sys_busy,
    input  logic        vpu_busy,
    input  logic        dma_busy,

    // Control outputs to datapath
    output logic        sys_start,
    output logic [7:0]  sys_rows,
    output logic [7:0]  ub_rd_addr,
    output logic        wt_fifo_wr,
    output logic        vpu_start,
    output logic [3:0]  vpu_mode,
    output logic        wt_buf_sel,     // Double-buffering control
    output logic        acc_buf_sel,    // Double-buffering control

    // DMA interface (simplified)
    output logic        dma_start,
    output logic        dma_dir,        // 0: host->TPU, 1: TPU->host
    output logic [7:0]  dma_ub_addr,
    output logic [15:0] dma_length,
    output logic [1:0]  dma_elem_sz,

    // Pipeline status (for debugging/verification)
    output logic        pipeline_stall,
    output logic [1:0]  current_stage   // 2'b01: Stage1, 2'b10: Stage2
);

// =============================================================================
// LOCAL PARAMETERS
// =============================================================================

// ISA Opcodes (6-bit)
localparam OPCODE_WIDTH = 6;
localparam [OPCODE_WIDTH-1:0] MATMUL_OP    = 6'h01;
localparam [OPCODE_WIDTH-1:0] RD_WEIGHT_OP = 6'h02;
localparam [OPCODE_WIDTH-1:0] RELU_OP      = 6'h03;
localparam [OPCODE_WIDTH-1:0] SYNC_OP      = 6'h04;

// Instruction field positions (32-bit instruction format)
// [31:26] opcode (6-bit)
// [25:18] arg1 (8-bit)
// [17:10] arg2 (8-bit)
// [9:2]   arg3 (8-bit)
// [1:0]   flags (2-bit)

// =============================================================================
// STAGE 1: FETCH + DECODE REGISTERS
// =============================================================================

// Program Counter
logic [7:0] pc_reg;
logic       pc_cnt;      // Increment PC
logic       pc_ld;       // Load PC from pc_in

// Instruction Register
logic [31:0] ir_reg;
logic        ir_ld;      // Load IR from instruction memory

// Instruction decoding
logic [OPCODE_WIDTH-1:0] opcode;
logic [7:0]              arg1, arg2, arg3;
logic [1:0]              flags;
logic [3:0]              unit_sel;  // For future expansion

// =============================================================================
// IF/ID PIPELINE REGISTER (45 bits total)
// =============================================================================

logic        if_id_valid;
logic [7:0]  if_id_pc;
logic [OPCODE_WIDTH-1:0] if_id_opcode;
logic [7:0]             if_id_arg1, if_id_arg2, if_id_arg3;
logic [1:0]             if_id_flags;
logic [3:0]             if_id_unit_sel;

// Pipeline control
logic        if_id_stall;
logic        if_id_flush;

// =============================================================================
// STAGE 2: EXECUTE REGISTERS
// =============================================================================

// Control signal generation
logic        exec_valid;
logic [OPCODE_WIDTH-1:0] exec_opcode;
logic [7:0]             exec_arg1, exec_arg2, exec_arg3;
logic [1:0]             exec_flags;

// =============================================================================
// HAZARD DETECTION
// =============================================================================

logic hazard_detected;

// =============================================================================
// PROGRAM COUNTER LOGIC
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        pc_reg <= 8'h00;
    end else if (pc_ld) begin
        pc_reg <= instr_data[7:0];  // Load from instruction data (for jumps)
    end else if (pc_cnt && !if_id_stall) begin
        pc_reg <= pc_reg + 8'h01;
    end
    // Hold PC when stalled
end

// =============================================================================
// INSTRUCTION REGISTER LOGIC
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ir_reg <= 32'h00000000;
    end else if (ir_ld && !if_id_stall) begin
        ir_reg <= instr_data;
    end
    // Hold IR when stalled
end

// =============================================================================
// INSTRUCTION DECODING
// =============================================================================

assign opcode  = ir_reg[31:26];
assign arg1    = ir_reg[25:18];
assign arg2    = ir_reg[17:10];
assign arg3    = ir_reg[9:2];
assign flags   = ir_reg[1:0];
assign unit_sel = 4'h0;  // Reserved for future use

// =============================================================================
// IF/ID PIPELINE REGISTER
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        if_id_valid    <= 1'b0;
        if_id_pc       <= 8'h00;
        if_id_opcode   <= {OPCODE_WIDTH{1'b0}};
        if_id_arg1     <= 8'h00;
        if_id_arg2     <= 8'h00;
        if_id_arg3     <= 8'h00;
        if_id_flags    <= 2'b00;
        if_id_unit_sel <= 4'h0;
    end else if (if_id_flush) begin
        // Flush pipeline on exceptions/jumps
        if_id_valid    <= 1'b0;
        if_id_pc       <= 8'h00;
        if_id_opcode   <= {OPCODE_WIDTH{1'b0}};
        if_id_arg1     <= 8'h00;
        if_id_arg2     <= 8'h00;
        if_id_arg3     <= 8'h00;
        if_id_flags    <= 2'b00;
        if_id_unit_sel <= 4'h0;
    end else if (!if_id_stall) begin
        // Normal pipeline advance
        if_id_valid    <= 1'b1;  // Always valid for now
        if_id_pc       <= pc_reg;
        if_id_opcode   <= opcode;
        if_id_arg1     <= arg1;
        if_id_arg2     <= arg2;
        if_id_arg3     <= arg3;
        if_id_flags    <= flags;
        if_id_unit_sel <= unit_sel;
    end
    // Hold values when stalled
end

// =============================================================================
// HAZARD DETECTION LOGIC
// =============================================================================

assign hazard_detected = sys_busy | vpu_busy | dma_busy;
assign if_id_stall = hazard_detected;
assign pipeline_stall = hazard_detected;

// =============================================================================
// STAGE 2: EXECUTE LOGIC
// =============================================================================

// Pipeline exec signals
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        exec_valid  <= 1'b0;
        exec_opcode <= {OPCODE_WIDTH{1'b0}};
        exec_arg1   <= 8'h00;
        exec_arg2   <= 8'h00;
        exec_arg3   <= 8'h00;
        exec_flags  <= 2'b00;
    end else begin
        exec_valid  <= if_id_valid;
        exec_opcode <= if_id_opcode;
        exec_arg1   <= if_id_arg1;
        exec_arg2   <= if_id_arg2;
        exec_arg3   <= if_id_arg3;
        exec_flags  <= if_id_flags;
    end
end

// =============================================================================
// CONTROL SIGNAL GENERATION
// =============================================================================

// Default values
assign sys_start   = 1'b0;
assign sys_rows    = 8'h00;
assign ub_rd_addr  = 8'h00;
assign wt_fifo_wr  = 1'b0;
assign vpu_start   = 1'b0;
assign vpu_mode    = 4'h0;
assign dma_start   = 1'b0;
assign dma_dir     = 1'b0;
assign dma_ub_addr = 8'h00;
assign dma_length  = 16'h0000;
assign dma_elem_sz = 2'b00;

// Control signal generation based on opcode
always_comb begin
    // Default assignments
    sys_start   = 1'b0;
    sys_rows    = 8'h00;
    ub_rd_addr  = 8'h00;
    wt_fifo_wr  = 1'b0;
    vpu_start   = 1'b0;
    vpu_mode    = 4'h0;
    dma_start   = 1'b0;
    dma_dir     = 1'b0;
    dma_ub_addr = 8'h00;
    dma_length  = 16'h0000;
    dma_elem_sz = 2'b00;

    if (exec_valid) begin
        case (exec_opcode)
            MATMUL_OP: begin
                sys_start  = 1'b1;
                sys_rows   = exec_arg3;
                ub_rd_addr = exec_arg1;
                // arg2 could be used for other parameters in future
            end

            RD_WEIGHT_OP: begin
                wt_fifo_wr = 1'b1;
                // Weight data comes from DMA or other source
            end

            RELU_OP: begin
                vpu_start = 1'b1;
                vpu_mode  = 4'h1;  // ReLU mode
                // Additional VPU parameters could come from args
            end

            SYNC_OP: begin
                // Toggle double-buffering selectors
                wt_buf_sel  = ~wt_buf_sel;   // Swap weight buffers
                acc_buf_sel = ~acc_buf_sel; // Swap accumulator buffers
            end

            default: begin
                // NOP or invalid opcode - no control signals
            end
        endcase
    end
end

// =============================================================================
// PIPELINE CONTROL SIGNALS
// =============================================================================

// Stage 1 control
assign pc_cnt = 1'b1;  // Always try to count (will be gated by stall)
assign pc_ld  = 1'b0;  // No jumps implemented yet
assign ir_ld  = 1'b1;  // Always try to load instruction (gated by stall)
assign if_id_flush = 1'b0;  // No flushes implemented yet

// Instruction address output
assign instr_addr = pc_reg;

// Current stage indicator for debugging
assign current_stage = pipeline_stall ? 2'b00 :  // Stalled
                       (exec_valid ? 2'b10 : 2'b01);  // Stage2 : Stage1

endmodule
