`timescale 1ns / 1ps

module tpu_controller (
    input  wire        clk,
    input  wire        rst_n,

    // Instruction memory interface
    output wire [7:0]  instr_addr,     // Instruction address to memory
    input  wire [31:0] instr_data,     // Instruction data from memory

    // Status inputs from datapath (for hazard detection)
    input  wire        sys_busy,
    input  wire        vpu_busy,
    input  wire        dma_busy,
    input  wire        wt_busy,        // Weight FIFO busy

    // =============================================================================
    // CONTROL OUTPUTS TO DATAPATH (46 signals total)
    // =============================================================================

    // Pipeline/Program Control (4 signals)
    output wire        pc_cnt,         // Program counter increment
    output reg         pc_ld,          // Program counter load
    output wire        ir_ld,          // Instruction register load
    output reg         if_id_flush,    // Pipeline flush

    // Systolic Array Control (7 signals)
    output reg         sys_start,       // Start systolic operation
    output reg [1:0]   sys_mode,        // Operation mode (00=MatMul, 01=Conv2D, 10=Accumulate)
    output reg [7:0]   sys_rows,         // Number of rows to process
    output reg         sys_signed,      // Signed/unsigned arithmetic
    output reg         sys_transpose,   // Transpose input matrix
    output reg [7:0]   sys_acc_addr,    // Accumulator write address
    output reg         sys_acc_clear,   // Clear accumulator before write

    // Unified Buffer Control (7 signals)
    output reg         ub_rd_en,        // UB read enable
    output reg         ub_wr_en,        // UB write enable
    output reg [8:0]   ub_rd_addr,      // Read address + bank select
    output reg [8:0]   ub_wr_addr,      // Write address + bank select
    output reg [8:0]   ub_rd_count,      // Read burst count
    output reg [8:0]   ub_wr_count,      // Write burst count
    output reg         ub_buf_sel,      // Bank selection toggle

    // Weight FIFO Control (5 signals)
    output reg         wt_mem_rd_en,    // Read from weight DRAM
    output reg [23:0]  wt_mem_addr,     // DRAM address
    output reg         wt_fifo_wr,      // Weight FIFO write enable
    output reg [7:0]   wt_num_tiles,     // Number of tiles to load
    output reg         wt_buf_sel,      // Weight buffer selection

    // Accumulator Control (4 signals)
    output reg         acc_wr_en,       // Accumulator write enable
    output reg         acc_rd_en,       // Accumulator read enable
    output reg [7:0]   acc_addr,        // Accumulator address
    output reg         acc_buf_sel,     // Accumulator buffer selection

    // VPU Control (6 signals)
    output reg         vpu_start,       // Start VPU operation
    output reg [3:0]   vpu_mode,        // VPU function selection
    output reg [7:0]   vpu_in_addr,     // VPU input address
    output reg [7:0]   vpu_out_addr,    // VPU output address
    output reg [7:0]   vpu_length,      // Number of elements
    output reg [15:0]  vpu_param,       // VPU operation parameter

    // DMA Control (5 signals)
    output reg         dma_start,       // Initiate DMA transfer
    output reg         dma_dir,         // Direction (0=host→TPU, 1=TPU→host)
    output reg [7:0]   dma_ub_addr,     // DMA unified buffer address
    output reg [15:0]  dma_length,      // DMA transfer length
    output reg [1:0]   dma_elem_sz,     // Element size (00=8b, 01=16b, 10=32b)

    // Sync/Control (3 signals)
    output reg         sync_wait,       // Stall pipeline until completion
    output reg [3:0]   sync_mask,       // Units to wait for (bit mask)
    output reg [15:0]  sync_timeout,    // Maximum wait cycles

    // Configuration Control (3 signals)
    output reg         cfg_wr_en,       // Configuration register write enable
    output reg [7:0]   cfg_addr,        // Configuration register address
    output reg [15:0]  cfg_data,        // Data to write

    // Halt/Interrupt (2 signals)
    output reg         halt_req,        // Stop program execution
    output reg         interrupt_en,    // Enable host interrupt

    // Pipeline status (for debugging/verification)
    output wire        pipeline_stall,
    output wire [1:0]  current_stage    // 2'b01: Stage1, 2'b10: Stage2
);

// =============================================================================
// LOCAL PARAMETERS
// =============================================================================

// ISA Opcodes (6-bit) - All 20 instructions
localparam OPCODE_WIDTH = 6;
localparam [OPCODE_WIDTH-1:0] NOP_OP         = 6'h00;
localparam [OPCODE_WIDTH-1:0] RD_HOST_MEM_OP = 6'h01;
localparam [OPCODE_WIDTH-1:0] WR_HOST_MEM_OP = 6'h02;
localparam [OPCODE_WIDTH-1:0] RD_WEIGHT_OP   = 6'h03;
localparam [OPCODE_WIDTH-1:0] LD_UB_OP       = 6'h04;
localparam [OPCODE_WIDTH-1:0] ST_UB_OP       = 6'h05;
localparam [OPCODE_WIDTH-1:0] MATMUL_OP      = 6'h10;
localparam [OPCODE_WIDTH-1:0] CONV2D_OP      = 6'h11;
localparam [OPCODE_WIDTH-1:0] MATMUL_ACC_OP  = 6'h12;
localparam [OPCODE_WIDTH-1:0] RELU_OP        = 6'h18;
localparam [OPCODE_WIDTH-1:0] RELU6_OP       = 6'h19;
localparam [OPCODE_WIDTH-1:0] SIGMOID_OP     = 6'h1A;
localparam [OPCODE_WIDTH-1:0] TANH_OP        = 6'h1B;
localparam [OPCODE_WIDTH-1:0] MAXPOOL_OP     = 6'h20;
localparam [OPCODE_WIDTH-1:0] AVGPOOL_OP     = 6'h21;
localparam [OPCODE_WIDTH-1:0] ADD_BIAS_OP     = 6'h22;
localparam [OPCODE_WIDTH-1:0] BATCH_NORM_OP   = 6'h23;
localparam [OPCODE_WIDTH-1:0] SYNC_OP         = 6'h30;
localparam [OPCODE_WIDTH-1:0] CFG_REG_OP      = 6'h31;
localparam [OPCODE_WIDTH-1:0] HALT_OP         = 6'h3F;

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
reg [7:0] pc_reg;
reg      pc_cnt_internal;  // Internal PC increment signal

// Instruction Register
reg [31:0] ir_reg;
reg       ir_ld_internal;  // Internal IR load signal

// Instruction decoding
wire [OPCODE_WIDTH-1:0] opcode;
wire [7:0]              arg1, arg2, arg3;
wire [1:0]              flags;

// =============================================================================
// IF/ID PIPELINE REGISTER
// =============================================================================

reg        if_id_valid;
reg [7:0]  if_id_pc;
reg [OPCODE_WIDTH-1:0] if_id_opcode;
reg [7:0]             if_id_arg1, if_id_arg2, if_id_arg3;
reg [1:0]             if_id_flags;

// Pipeline control
wire        if_id_stall;

// =============================================================================
// STAGE 2: EXECUTE REGISTERS
// =============================================================================

reg        exec_valid;
reg [OPCODE_WIDTH-1:0] exec_opcode;
reg [7:0]             exec_arg1, exec_arg2, exec_arg3;
reg [1:0]             exec_flags;

// =============================================================================
// BUFFER STATE TRACKING (for toggle operations)
// =============================================================================

reg        wt_buf_sel_reg;   // Current weight buffer selection
reg        acc_buf_sel_reg;  // Current accumulator buffer selection
reg        ub_buf_sel_reg;   // Current unified buffer bank selection

// =============================================================================
// CONFIGURATION REGISTERS
// =============================================================================

reg [15:0] cfg_registers [0:255];  // 256 configuration registers

// =============================================================================
// SYNC STATE MACHINE
// =============================================================================

reg        sync_active;
reg [15:0] sync_counter;
reg [3:0]  sync_wait_mask;

// =============================================================================
// HAZARD DETECTION
// =============================================================================

wire hazard_detected;
wire sync_hazard;

// =============================================================================
// PROGRAM COUNTER LOGIC
// =============================================================================

always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        pc_reg <= 8'h00;
    end else if (pc_ld) begin
        pc_reg <= instr_data[7:0];  // Load from instruction data (for jumps)
    end else if (pc_cnt_internal && !if_id_stall && !sync_hazard) begin
        pc_reg <= pc_reg + 8'h01;
    end
    // Hold PC when stalled
end

// =============================================================================
// INSTRUCTION REGISTER LOGIC
// =============================================================================

always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ir_reg <= 32'h00000000;
    end else if (ir_ld_internal && !if_id_stall) begin
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

// =============================================================================
// IF/ID PIPELINE REGISTER (with buffer state)
// =============================================================================

reg if_id_wt_buf_sel;
reg if_id_acc_buf_sel;
reg if_id_ub_buf_sel;

always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        if_id_valid    <= 1'b0;
        if_id_pc       <= 8'h00;
        if_id_opcode   <= {OPCODE_WIDTH{1'b0}};
        if_id_arg1     <= 8'h00;
        if_id_arg2     <= 8'h00;
        if_id_arg3     <= 8'h00;
        if_id_flags    <= 2'b00;
        if_id_wt_buf_sel  <= 1'b0;
        if_id_acc_buf_sel <= 1'b0;
        if_id_ub_buf_sel  <= 1'b0;
    end else if (if_id_flush) begin
        // Flush pipeline on exceptions/jumps
        if_id_valid    <= 1'b0;
        if_id_pc       <= 8'h00;
        if_id_opcode   <= {OPCODE_WIDTH{1'b0}};
        if_id_arg1     <= 8'h00;
        if_id_arg2     <= 8'h00;
        if_id_arg3     <= 8'h00;
        if_id_flags    <= 2'b00;
        if_id_wt_buf_sel  <= 1'b0;
        if_id_acc_buf_sel <= 1'b0;
        if_id_ub_buf_sel  <= 1'b0;
    end else if (!if_id_stall) begin
        // Normal pipeline advance - capture buffer state at decode time
        if_id_valid    <= 1'b1;
        if_id_pc       <= pc_reg;
        if_id_opcode   <= opcode;
        if_id_arg1     <= arg1;
        if_id_arg2     <= arg2;
        if_id_arg3     <= arg3;
        if_id_flags    <= flags;
        // Capture current buffer state for this instruction
        if_id_wt_buf_sel  <= wt_buf_sel_reg;
        if_id_acc_buf_sel <= acc_buf_sel_reg;
        if_id_ub_buf_sel  <= ub_buf_sel_reg;
    end
    // Hold values when stalled
end

// =============================================================================
// BUFFER STATE REGISTERS (updated only on SYNC execution)
// =============================================================================

always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        wt_buf_sel_reg  <= 1'b0;
        acc_buf_sel_reg <= 1'b0;
        ub_buf_sel_reg  <= 1'b0;
    end else if (exec_valid && (exec_opcode == SYNC_OP)) begin
        // Only update buffer state when SYNC instruction executes
        // This ensures proper pipelining - instructions already in pipeline
        // use their captured buffer state, only future instructions see new state
        wt_buf_sel_reg  <= ~wt_buf_sel_reg;
        acc_buf_sel_reg <= ~acc_buf_sel_reg;
        ub_buf_sel_reg  <= ~ub_buf_sel_reg;
    end
    // Otherwise hold current state
end

// =============================================================================
// CONFIGURATION REGISTERS
// =============================================================================

integer cfg_init_i;

always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        // Initialize all config registers to 0
        for (cfg_init_i = 0; cfg_init_i < 256; cfg_init_i = cfg_init_i + 1) begin
            cfg_registers[cfg_init_i] <= 16'h0000;
        end
    end else if (cfg_wr_en) begin
        cfg_registers[cfg_addr] <= cfg_data;
    end
end

// =============================================================================
// SYNC STATE MACHINE
// =============================================================================

wire [3:0] unit_status;
assign unit_status[0] = !sys_busy;   // Systolic done
assign unit_status[1] = !vpu_busy;   // VPU done
assign unit_status[2] = !dma_busy;    // DMA done
assign unit_status[3] = !wt_busy;     // Weight FIFO done

always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        sync_active    <= 1'b0;
        sync_counter   <= 16'h0000;
        sync_wait_mask <= 4'b0000;
    end else if (sync_wait) begin
        if (!sync_active) begin
            // Start sync wait
            sync_active    <= 1'b1;
            sync_counter   <= 16'h0000;
            sync_wait_mask <= sync_mask;
        end else begin
            // Check if units are done
            if ((unit_status & sync_wait_mask) == sync_wait_mask) begin
                // All required units are done
                sync_active <= 1'b0;
            end else if (sync_counter >= sync_timeout) begin
                // Timeout reached
                sync_active <= 1'b0;
            end else begin
                sync_counter <= sync_counter + 1'b1;
            end
        end
    end else begin
        sync_active <= 1'b0;
    end
end

// =============================================================================
// HAZARD DETECTION LOGIC
// =============================================================================

assign hazard_detected = sys_busy | vpu_busy | dma_busy | wt_busy;
assign sync_hazard = sync_active;
assign if_id_stall = hazard_detected | sync_hazard;
assign pipeline_stall = if_id_stall;

// =============================================================================
// STAGE 2: EXECUTE LOGIC (with pipelined buffer state)
// =============================================================================

reg exec_wt_buf_sel;
reg exec_acc_buf_sel;
reg exec_ub_buf_sel;

always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        exec_valid  <= 1'b0;
        exec_opcode <= {OPCODE_WIDTH{1'b0}};
        exec_arg1   <= 8'h00;
        exec_arg2   <= 8'h00;
        exec_arg3   <= 8'h00;
        exec_flags  <= 2'b00;
        exec_wt_buf_sel  <= 1'b0;
        exec_acc_buf_sel <= 1'b0;
        exec_ub_buf_sel  <= 1'b0;
    end else begin
        // Pipeline advance - propagate buffer state from decode stage
        exec_valid  <= if_id_valid;
        exec_opcode <= if_id_opcode;
        exec_arg1   <= if_id_arg1;
        exec_arg2   <= if_id_arg2;
        exec_arg3   <= if_id_arg3;
        exec_flags  <= if_id_flags;
        // Propagate buffer state captured at decode time
        exec_wt_buf_sel  <= if_id_wt_buf_sel;
        exec_acc_buf_sel <= if_id_acc_buf_sel;
        exec_ub_buf_sel  <= if_id_ub_buf_sel;
    end
end

// =============================================================================
// CONTROL SIGNAL GENERATION - ALL 20 INSTRUCTIONS
// =============================================================================

always @* begin
    // ========================================================================
    // DEFAULT ASSIGNMENTS (all signals default to 0)
    // ========================================================================
    
    // Pipeline/Program Control
    pc_cnt_internal = 1'b1;
    pc_ld           = 1'b0;
    ir_ld_internal  = 1'b1;
    if_id_flush     = 1'b0;

    // Systolic Array Control
    sys_start       = 1'b0;
    sys_mode        = 2'b00;
    sys_rows        = 8'h00;
    sys_signed      = 1'b0;
    sys_transpose   = 1'b0;
    sys_acc_addr    = 8'h00;
    sys_acc_clear   = 1'b0;

    // Unified Buffer Control
    ub_rd_en        = 1'b0;
    ub_wr_en        = 1'b0;
    ub_rd_addr      = 9'h000;
    ub_wr_addr      = 9'h000;
    ub_rd_count     = 9'h000;
    ub_wr_count     = 9'h000;
    ub_buf_sel      = exec_ub_buf_sel;  // Use pipelined buffer state

    // Weight FIFO Control
    wt_mem_rd_en    = 1'b0;
    wt_mem_addr     = 24'h000000;
    wt_fifo_wr      = 1'b0;
    wt_num_tiles    = 8'h00;
    wt_buf_sel      = exec_wt_buf_sel;  // Use pipelined buffer state

    // Accumulator Control
    acc_wr_en       = 1'b0;
    acc_rd_en       = 1'b0;
    acc_addr        = 8'h00;
    acc_buf_sel     = exec_acc_buf_sel;  // Use pipelined buffer state

    // VPU Control
    vpu_start       = 1'b0;
    vpu_mode        = 4'h0;
    vpu_in_addr     = 8'h00;
    vpu_out_addr    = 8'h00;
    vpu_length      = 8'h00;
    vpu_param       = 16'h0000;

    // DMA Control
    dma_start       = 1'b0;
    dma_dir         = 1'b0;
    dma_ub_addr     = 8'h00;
    dma_length      = 16'h0000;
    dma_elem_sz     = 2'b00;

    // Sync/Control
    sync_wait       = 1'b0;
    sync_mask       = 4'b0000;
    sync_timeout    = 16'h0000;

    // Configuration Control
    cfg_wr_en       = 1'b0;
    cfg_addr        = 8'h00;
    cfg_data        = 16'h0000;

    // Halt/Interrupt
    halt_req        = 1'b0;
    interrupt_en    = 1'b0;

    // ========================================================================
    // INSTRUCTION-SPECIFIC CONTROL SIGNAL GENERATION
    // ========================================================================

    if (exec_valid) begin
        case (exec_opcode)
            // ================================================================
            // 0x00: NOP - No Operation
            // ================================================================
            NOP_OP: begin
                // All defaults - just pipeline progression
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x01: RD_HOST_MEM - Read from Host Memory
            // ================================================================
            RD_HOST_MEM_OP: begin
                dma_start       = 1'b1;
                dma_dir         = 1'b0;  // Host → TPU
                dma_ub_addr     = exec_arg1;
                dma_length      = {exec_arg2, exec_arg3};
                dma_elem_sz     = exec_flags;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x02: WR_HOST_MEM - Write to Host Memory
            // ================================================================
            WR_HOST_MEM_OP: begin
                dma_start       = 1'b1;
                dma_dir         = 1'b1;  // TPU → Host
                dma_ub_addr     = exec_arg1;
                dma_length      = {exec_arg2, exec_arg3};
                dma_elem_sz     = exec_flags;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x03: RD_WEIGHT - Read Weight Data
            // ================================================================
            RD_WEIGHT_OP: begin
                wt_mem_rd_en    = 1'b1;
                wt_mem_addr      = exec_arg1 * 24'd65536;  // Tile base address
                wt_fifo_wr      = 1'b1;
                wt_num_tiles    = exec_arg2;
                wt_buf_sel      = exec_flags[0];
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x04: LD_UB - Load from Unified Buffer
            // ================================================================
            LD_UB_OP: begin
                ub_rd_en        = 1'b1;
                ub_rd_addr      = {exec_ub_buf_sel, exec_arg1};
                ub_rd_count     = {1'b0, exec_arg2};
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x05: ST_UB - Store to Unified Buffer
            // ================================================================
            ST_UB_OP: begin
                ub_wr_en        = 1'b1;
                ub_wr_addr      = {~exec_ub_buf_sel, exec_arg1};
                ub_wr_count     = {1'b0, exec_arg2};
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x10: MATMUL - Matrix Multiplication
            // ================================================================
            MATMUL_OP: begin
                sys_start       = 1'b1;
                sys_mode        = 2'b00;  // Matrix multiply mode
                sys_rows        = exec_arg3;
                sys_signed      = exec_flags[1];
                sys_transpose   = exec_flags[0];
                sys_acc_addr    = exec_arg2;
                sys_acc_clear   = 1'b1;
                ub_rd_en        = 1'b1;
                ub_rd_addr      = {exec_ub_buf_sel, exec_arg1};
                ub_rd_count     = 9'h001;
                acc_wr_en       = 1'b1;
                acc_addr        = exec_arg2;
                acc_buf_sel     = exec_acc_buf_sel;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x11: CONV2D - 2D Convolution
            // ================================================================
            CONV2D_OP: begin
                sys_start       = 1'b1;
                sys_mode        = 2'b01;  // Convolution mode
                sys_rows        = exec_arg3;  // Derived from kernel/stride
                sys_signed      = exec_flags[1];
                sys_transpose   = exec_flags[0];
                sys_acc_addr    = exec_arg2;
                sys_acc_clear   = 1'b1;
                ub_rd_en        = 1'b1;
                ub_rd_addr      = {exec_ub_buf_sel, exec_arg1};
                ub_rd_count     = 9'h001;
                acc_wr_en       = 1'b1;
                acc_addr        = exec_arg2;
                acc_buf_sel     = exec_acc_buf_sel;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x12: MATMUL_ACC - Accumulate Matrix Multiplication
            // ================================================================
            MATMUL_ACC_OP: begin
                sys_start       = 1'b1;
                sys_mode        = 2'b10;  // Accumulate mode
                sys_rows        = exec_arg3;
                sys_signed      = exec_flags[1];
                sys_transpose   = exec_flags[0];
                sys_acc_addr    = exec_arg2;
                sys_acc_clear   = 1'b0;  // Don't clear - accumulate!
                ub_rd_en        = 1'b1;
                ub_rd_addr      = {exec_ub_buf_sel, exec_arg1};
                ub_rd_count     = 9'h001;
                acc_wr_en       = 1'b1;
                acc_addr        = exec_arg2;
                acc_buf_sel     = exec_acc_buf_sel;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x18: RELU - Rectified Linear Unit
            // ================================================================
            RELU_OP: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h1;   // ReLU mode
                vpu_in_addr     = exec_arg1;
                vpu_out_addr    = exec_arg2;
                vpu_length      = exec_arg3;
                vpu_param       = exec_flags[0] ? cfg_registers[{6'b0, exec_flags}] : 16'h0000;
                acc_rd_en       = 1'b1;
                acc_addr        = exec_arg1;
                acc_buf_sel     = ~exec_acc_buf_sel;  // Read from opposite bank
                ub_wr_en        = 1'b1;
                ub_wr_addr      = {~exec_ub_buf_sel, exec_arg2};
                ub_wr_count     = 9'h001;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x19: RELU6 - ReLU6 Activation
            // ================================================================
            RELU6_OP: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h2;   // ReLU6 mode
                vpu_in_addr     = exec_arg1;
                vpu_out_addr    = exec_arg2;
                vpu_length      = exec_arg3;
                vpu_param       = exec_flags[0] ? cfg_registers[{6'b0, exec_flags}] : 16'h0000;
                acc_rd_en       = 1'b1;
                acc_addr        = exec_arg1;
                acc_buf_sel     = ~exec_acc_buf_sel;
                ub_wr_en        = 1'b1;
                ub_wr_addr      = {~exec_ub_buf_sel, exec_arg2};
                ub_wr_count     = 9'h001;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x1A: SIGMOID - Sigmoid Activation
            // ================================================================
            SIGMOID_OP: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h3;   // Sigmoid mode
                vpu_in_addr     = exec_arg1;
                vpu_out_addr    = exec_arg2;
                vpu_length      = exec_arg3;
                vpu_param       = exec_flags[0] ? cfg_registers[{6'b0, exec_flags}] : 16'h0000;
                acc_rd_en       = 1'b1;
                acc_addr        = exec_arg1;
                acc_buf_sel     = ~exec_acc_buf_sel;
                ub_wr_en        = 1'b1;
                ub_wr_addr      = {~exec_ub_buf_sel, exec_arg2};
                ub_wr_count     = 9'h001;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x1B: TANH - Hyperbolic Tangent
            // ================================================================
            TANH_OP: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h4;   // Tanh mode
                vpu_in_addr     = exec_arg1;
                vpu_out_addr    = exec_arg2;
                vpu_length      = exec_arg3;
                vpu_param       = exec_flags[0] ? cfg_registers[{6'b0, exec_flags}] : 16'h0000;
                acc_rd_en       = 1'b1;
                acc_addr        = exec_arg1;
                acc_buf_sel     = ~exec_acc_buf_sel;
                ub_wr_en        = 1'b1;
                ub_wr_addr      = {~exec_ub_buf_sel, exec_arg2};
                ub_wr_count     = 9'h001;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x20: MAXPOOL - Max Pooling
            // ================================================================
            MAXPOOL_OP: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h6;   // Max pool mode
                vpu_in_addr     = exec_arg1;
                vpu_out_addr    = exec_arg2;
                ub_rd_en        = 1'b1;
                ub_rd_addr      = {exec_ub_buf_sel, exec_arg1};
                ub_rd_count     = 9'h001;
                ub_wr_en        = 1'b1;
                ub_wr_addr      = {~exec_ub_buf_sel, exec_arg2};
                ub_wr_count     = 9'h001;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x21: AVGPOOL - Average Pooling
            // ================================================================
            AVGPOOL_OP: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h7;   // Avg pool mode
                vpu_in_addr     = exec_arg1;
                vpu_out_addr    = exec_arg2;
                ub_rd_en        = 1'b1;
                ub_rd_addr      = {exec_ub_buf_sel, exec_arg1};
                ub_rd_count     = 9'h001;
                ub_wr_en        = 1'b1;
                ub_wr_addr      = {~exec_ub_buf_sel, exec_arg2};
                ub_wr_count     = 9'h001;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x22: ADD_BIAS - Add Bias Vector
            // ================================================================
            ADD_BIAS_OP: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h5;   // Bias add mode
                vpu_in_addr     = exec_arg1;
                vpu_out_addr    = exec_arg2;
                vpu_length      = exec_arg3;
                acc_rd_en       = 1'b1;
                acc_addr        = exec_arg1;
                acc_buf_sel     = ~exec_acc_buf_sel;
                ub_rd_en        = 1'b1;
                ub_rd_addr      = {exec_ub_buf_sel, exec_arg2};  // Read bias from UB
                ub_rd_count     = 9'h001;
                ub_wr_en        = 1'b1;
                ub_wr_addr      = {~exec_ub_buf_sel, exec_arg2};
                ub_wr_count     = 9'h001;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x23: BATCH_NORM - Batch Normalization
            // ================================================================
            BATCH_NORM_OP: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h8;   // Batch norm mode
                vpu_in_addr     = exec_arg1;
                vpu_out_addr    = exec_arg2;
                vpu_param       = {8'h00, exec_arg3};  // Config register base index
                ub_rd_en        = 1'b1;
                ub_rd_addr      = {exec_ub_buf_sel, exec_arg1};
                ub_rd_count     = 9'h001;
                ub_wr_en        = 1'b1;
                ub_wr_addr      = {~exec_ub_buf_sel, exec_arg2};
                ub_wr_count     = 9'h001;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x30: SYNC - Synchronize Operations
            // ================================================================
            SYNC_OP: begin
                sync_wait       = 1'b1;
                sync_mask       = exec_arg1[3:0];
                sync_timeout    = {exec_arg2, exec_arg3};
                // Toggle all buffer selectors (use pipelined state)
                // The actual toggle happens in buffer state register update logic
                wt_buf_sel      = ~exec_wt_buf_sel;
                acc_buf_sel     = ~exec_acc_buf_sel;
                ub_buf_sel      = ~exec_ub_buf_sel;
                pc_cnt_internal = 1'b0;  // Stall PC!
                ir_ld_internal  = 1'b1;  // Continue loading but don't execute
            end

            // ================================================================
            // 0x31: CFG_REG - Configure Register
            // ================================================================
            CFG_REG_OP: begin
                cfg_wr_en       = 1'b1;
                cfg_addr        = exec_arg1;
                cfg_data        = {exec_arg2, exec_arg3};
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x3F: HALT - Program Termination
            // ================================================================
            HALT_OP: begin
                halt_req        = 1'b1;
                interrupt_en    = 1'b1;
                pc_cnt_internal = 1'b0;  // Stop PC increment
                ir_ld_internal  = 1'b0;  // Stop loading new instructions
            end

            // ================================================================
            // DEFAULT: Invalid opcode
            // ================================================================
            default: begin
                // NOP behavior for invalid opcodes
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end
        endcase
    end
end

// =============================================================================
// OUTPUT ASSIGNMENTS
// =============================================================================

// pc_cnt and ir_ld are assigned in the control signal generation always block
assign instr_addr = pc_reg;

// Output assignments
assign pc_cnt = pc_cnt_internal;
assign ir_ld = ir_ld_internal;

// Current stage indicator for debugging
assign current_stage = pipeline_stall ? 2'b00 :  // Stalled
                       (exec_valid ? 2'b10 : 2'b01);  // Stage2 : Stage1

endmodule
