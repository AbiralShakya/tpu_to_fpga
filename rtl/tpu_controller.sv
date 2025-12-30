`timescale 1ns / 1ps

module tpu_controller (
    input  logic       clk,
    input  logic       rst_n,
    input  logic       start_execution, // Start program execution

    // Instruction memory interface
    output logic [7:0] instr_addr,     // Instruction address to memory
    input  logic [31:0] instr_data,    // Instruction data from memory

    // Status inputs from datapath (for hazard detection)
    input  logic       sys_busy,
    input  logic       sys_done,       // Systolic array operation complete
    input  logic       vpu_busy,
    input  logic       dma_busy,
    input  logic       wt_busy,        // Weight FIFO busy
    input  logic       ub_busy,        // Unified buffer busy (for buffer toggle safety)

    // =============================================================================
    // CONTROL OUTPUTS TO DATAPATH (40 signals total)
    // =============================================================================

    // Pipeline/Program Control (4 signals)
    output logic       pc_cnt,         // Program counter increment
    output logic       pc_ld,          // Program counter load
    output logic       ir_ld,          // Instruction register load
    output logic       if_id_flush,    // Pipeline flush

    // Systolic Array Control (6 signals)
    output logic       sys_start,       // Start systolic operation
    output logic [1:0] sys_mode,        // Operation mode (00=MatMul, 01=Conv2D, 10=Accumulate)
    output logic [7:0] sys_rows,        // Number of rows to process
    output logic       sys_signed,      // Signed/unsigned arithmetic
    output logic [7:0] sys_acc_addr,    // Accumulator write address
    output logic       sys_acc_clear,   // Clear accumulator before write

    // Unified Buffer Control (7 signals)
    output logic       ub_rd_en,        // UB read enable
    output logic       ub_wr_en,        // UB write enable
    output logic [8:0] ub_rd_addr,      // Read address + bank select
    output logic [8:0] ub_wr_addr,      // Write address + bank select
    output logic [8:0] ub_rd_count,     // Read burst count
    output logic [8:0] ub_wr_count,     // Write burst count
    output logic       ub_buf_sel,      // Bank selection toggle

    // Weight FIFO Control (4 signals)
    output logic       wt_mem_rd_en,    // Read from weight DRAM
    output logic [23:0] wt_mem_addr,    // DRAM address (used by tpu_top for weight_memory indexing)
    output logic       wt_fifo_wr,      // Weight FIFO write enable
    output logic [7:0] wt_num_tiles,    // Number of tiles to load

    // Accumulator Control (5 signals)
    output logic       acc_wr_en,       // Accumulator write enable
    output logic       acc_rd_en,       // Accumulator read enable
    output logic [7:0] acc_addr,        // Accumulator address
    output logic       acc_buf_sel,     // Accumulator buffer selection
    output logic [1:0] st_ub_col_idx,   // ST_UB column index (0=col0, 1=col1, 2=col2, 3=write)

    // VPU Control (3 signals)
    output logic       vpu_start,       // Start VPU operation
    output logic [3:0] vpu_mode,        // VPU function selection
    output logic [15:0] vpu_param,      // VPU operation parameter

    // DMA Control (5 signals)
    output logic       dma_start,       // Initiate DMA transfer
    output logic       dma_dir,         // Direction (0=host→TPU, 1=TPU→host)
    output logic [7:0] dma_ub_addr,     // DMA unified buffer address
    output logic [15:0] dma_length,     // DMA transfer length
    output logic [1:0] dma_elem_sz,     // Element size (00=8b, 01=16b, 10=32b)

    // Sync/Control (3 signals)
    output logic       sync_wait,       // Stall pipeline until completion
    output logic [3:0] sync_mask,       // Units to wait for (bit mask)
    output logic [15:0] sync_timeout,   // Maximum wait cycles

    // Configuration Control (3 signals)
    output logic       cfg_wr_en,       // Configuration register write enable
    output logic [7:0] cfg_addr,        // Configuration register address
    output logic [15:0] cfg_data,       // Data to write

    // Halt/Interrupt (2 signals)
    output logic       halt_req,        // Stop program execution
    output logic       interrupt_en,    // Enable host interrupt

    // Pipeline status (for debugging/verification)
    output logic       pipeline_stall,
    output logic [1:0] current_stage    // 2'b01: Stage1, 2'b10: Stage2
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
logic [7:0] pc_reg;
logic       pc_cnt_internal;  // Internal PC increment signal

// Instruction Register
logic [31:0] ir_reg;
logic        ir_ld_internal;  // Internal IR load signal

// Instruction decoding
logic [OPCODE_WIDTH-1:0] opcode;
logic [7:0]              arg1, arg2, arg3;
logic [1:0]              flags;

// =============================================================================
// IF/ID PIPELINE REGISTER
// =============================================================================

logic      if_id_valid;
logic [7:0] if_id_pc;
logic [OPCODE_WIDTH-1:0] if_id_opcode;
logic [7:0]              if_id_arg1, if_id_arg2, if_id_arg3;
logic [1:0]              if_id_flags;

// Pipeline control
logic      if_id_stall;

// =============================================================================
// STAGE 2: EXECUTE REGISTERS
// =============================================================================

logic      exec_valid;
logic [OPCODE_WIDTH-1:0] exec_opcode;
logic [7:0]              exec_arg1, exec_arg2, exec_arg3;
logic [1:0]              exec_flags;

// =============================================================================
// BUFFER STATE TRACKING (for toggle operations)
// =============================================================================

logic      wt_buf_sel_reg;   // Current weight buffer selection
logic      acc_buf_sel_reg;  // Current accumulator buffer selection
logic      ub_buf_sel_reg;   // Current unified buffer bank selection

// Store accumulator address from last MATMUL for ST_UB to read
logic [7:0] stored_acc_addr_reg;  // Accumulator base address from MATMUL
logic      stored_acc_buf_sel_reg; // Accumulator buffer selection from MATMUL
logic      stored_acc_signed_reg; // Signed/unsigned mode from MATMUL

// ST_UB byte accumulation registers (collect 3 bytes before writing to UB)
logic [7:0] st_ub_byte0_reg;  // Accumulator column 0 result
logic [7:0] st_ub_byte1_reg;  // Accumulator column 1 result
logic [7:0] st_ub_byte2_reg;  // Accumulator column 2 result

// =============================================================================
// CONFIGURATION REGISTERS
// =============================================================================

logic [15:0] cfg_registers [0:255];  // 256 configuration registers

// =============================================================================
// ST_UB STATE MACHINE (Multi-cycle read from accumulator)
// =============================================================================

typedef enum logic [2:0] {
    ST_UB_IDLE = 3'b000,
    ST_UB_READ_COL0 = 3'b001,      // Read acc0
    ST_UB_WAIT_COL0 = 3'b010,      // Wait for acc0 data, capture byte0, read acc1
    ST_UB_WAIT_COL1 = 3'b011,      // Wait for acc1 data, capture byte1, read acc2
    ST_UB_WAIT_COL2 = 3'b100,      // Wait for acc2 data, capture byte2
    ST_UB_WRITE = 3'b101           // Write all 3 bytes to UB
} st_ub_state_t;

st_ub_state_t st_ub_state;

// =============================================================================
// SYNC STATE MACHINE
// =============================================================================

logic      sync_active;
logic [15:0] sync_counter;
logic [3:0]  sync_wait_mask;

// =============================================================================
// HAZARD DETECTION
// =============================================================================

logic hazard_detected;
logic sync_hazard;

// =============================================================================
// PROGRAM COUNTER LOGIC
// =============================================================================

always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        pc_reg <= 8'h00;
    end else if (start_execution) begin
        // CRITICAL: Reset PC to 0 when start_execution is asserted
        // This allows UART to program instructions, then trigger execution
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
    end else if (start_execution) begin
        // CRITICAL: Clear IR on start_execution to prevent old instruction
        // from polluting the first pipeline cycle. Without this, the old
        // instruction (e.g., HALT from previous run) gets decoded and executed
        // before the new program's first instruction.
        ir_reg <= 32'h00000000;  // NOP
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

logic if_id_wt_buf_sel;
logic if_id_acc_buf_sel;
logic if_id_ub_buf_sel;

// Execute stage buffer selections (declared early for use in sequential blocks)
logic exec_wt_buf_sel;
logic exec_acc_buf_sel;
logic exec_ub_buf_sel;

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
    end else if (if_id_flush || start_execution) begin
        // Flush pipeline on exceptions/jumps OR when restarting execution
        // CRITICAL: Clear pipeline when start_execution to avoid executing stale instructions
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

// Buffer toggle safety: Only toggle when no operations are active on buffers
// This prevents corruption from toggling buffers mid-operation
logic buffers_idle;
assign buffers_idle = !ub_busy && !wt_busy;  // Both unified buffer and weight FIFO must be idle

always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        wt_buf_sel_reg  <= 1'b0;
        acc_buf_sel_reg <= 1'b0;
        ub_buf_sel_reg  <= 1'b0;
        stored_acc_addr_reg <= 8'h00;
        stored_acc_buf_sel_reg <= 1'b0;
        stored_acc_signed_reg <= 1'b0;
        st_ub_state <= ST_UB_IDLE;
    end else if (start_execution) begin
        // Reset buffer selection to 0 when starting execution
        // This ensures a clean state for the new program
        wt_buf_sel_reg  <= 1'b0;
        acc_buf_sel_reg <= 1'b0;
        ub_buf_sel_reg  <= 1'b0;
        stored_acc_addr_reg <= 8'h00;
        stored_acc_buf_sel_reg <= 1'b0;
        stored_acc_signed_reg <= 1'b0;
        st_ub_state <= ST_UB_IDLE;
    end else if (exec_valid && (exec_opcode == 6'h30) && buffers_idle) begin
        // Only update buffer state when SYNC instruction executes AND buffers are idle
        // This ensures proper pipelining - instructions already in pipeline
        // use their captured buffer state, only future instructions see new state
        // CRITICAL: Wait for buffers to be idle to prevent data corruption
        wt_buf_sel_reg  <= ~wt_buf_sel_reg;
        acc_buf_sel_reg <= ~acc_buf_sel_reg;
        ub_buf_sel_reg  <= ~ub_buf_sel_reg;
    end else if (exec_valid && (exec_opcode == 6'h10)) begin
        // Store accumulator address, buffer, and signed mode for ST_UB to read
        stored_acc_addr_reg <= exec_arg2;
        stored_acc_buf_sel_reg <= exec_acc_buf_sel;
        stored_acc_signed_reg <= exec_flags[1];  // sys_signed flag
    end else if (exec_valid && (exec_opcode == 6'h05)) begin
        // ST_UB state transitions (handled in sequential block below)
    end else begin
        // Reset ST_UB state when not executing ST_UB
        st_ub_state <= ST_UB_IDLE;
    end
    // Otherwise hold current state (including if buffers are busy)
end

// =============================================================================
// ST_UB STATE TRANSITIONS (Sequential)
// =============================================================================

always @ (posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        st_ub_state <= ST_UB_IDLE;
    end else if (start_execution) begin
        st_ub_state <= ST_UB_IDLE;
    end else if (exec_valid && (exec_opcode == 6'h05)) begin
        // State transitions based on current state
        case (st_ub_state)
            ST_UB_IDLE: st_ub_state <= ST_UB_READ_COL0;
            ST_UB_READ_COL0: st_ub_state <= ST_UB_WAIT_COL0;
            ST_UB_WAIT_COL0: st_ub_state <= ST_UB_WAIT_COL1;
            ST_UB_WAIT_COL1: st_ub_state <= ST_UB_WAIT_COL2;
            ST_UB_WAIT_COL2: st_ub_state <= ST_UB_WRITE;
            ST_UB_WRITE: st_ub_state <= ST_UB_IDLE;
            default: st_ub_state <= ST_UB_IDLE;
        endcase
    end else begin
        // Reset when not executing ST_UB
        st_ub_state <= ST_UB_IDLE;
    end
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

logic [3:0] unit_status;
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

// CRITICAL FIX: DO NOT include ub_busy in hazard detection!
// ub_busy is combinatorial (ub_busy = ub_rd_en | ub_wr_en)
// Including it creates a DEADLOCK:
//   1. Instruction sets ub_rd_en=1
//   2. ub_busy goes high (combinatorial)
//   3. hazard_detected goes high
//   4. Pipeline stalls
//   5. Controller can't advance to de-assert ub_rd_en
//   6. System stuck forever!
// The controller's internal state machines (like st_ub_state) already
// handle multi-cycle UB operations correctly.
assign hazard_detected = sys_busy | vpu_busy | dma_busy | wt_busy;
assign sync_hazard = sync_active;
assign if_id_stall = hazard_detected | sync_hazard;
assign pipeline_stall = if_id_stall;

// =============================================================================
// STAGE 2: EXECUTE LOGIC (with pipelined buffer state)
// =============================================================================

// exec_wt_buf_sel, exec_acc_buf_sel, exec_ub_buf_sel declared earlier

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
    end else if (start_execution) begin
        // CRITICAL: Flush execute stage when restarting execution
        exec_valid  <= 1'b0;
        exec_opcode <= {OPCODE_WIDTH{1'b0}};
        exec_arg1   <= 8'h00;
        exec_arg2   <= 8'h00;
        exec_arg3   <= 8'h00;
        exec_flags  <= 2'b00;
        exec_wt_buf_sel  <= 1'b0;
        exec_acc_buf_sel <= 1'b0;
        exec_ub_buf_sel  <= 1'b0;
    end else if (!if_id_stall) begin
        // CRITICAL: Only advance exec stage when NOT stalled
        // This ensures each instruction executes exactly once
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
    // When stalled, exec stage HOLDS its current values
    // The instruction already in exec continues executing (or waiting)
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

    // Accumulator Control
    acc_wr_en       = 1'b0;
    acc_rd_en       = 1'b0;
    acc_addr        = 8'h00;
    acc_buf_sel     = exec_acc_buf_sel;  // Use pipelined buffer state
    st_ub_col_idx   = 2'd0;              // Default: not in ST_UB write phase

    // VPU Control
    vpu_start       = 1'b0;
    vpu_mode        = 4'h0;
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
            6'h00: begin
                // All defaults - just pipeline progression
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x01: RD_HOST_MEM - Read from Host Memory
            // ================================================================
            6'h01: begin
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
            6'h02: begin
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
            // arg1 = weight row address (0, 1, 2 for 3x3 array)
            // Reads one row from weight memory and pushes to weight FIFOs
            // For 3x3 array, issue 3 RD_WEIGHT instructions (rows 0, 1, 2)
            6'h03: begin
                wt_mem_rd_en    = 1'b1;
                wt_mem_addr     = {16'b0, exec_arg1};  // Direct row addressing
                wt_fifo_wr      = 1'b1;
                wt_num_tiles    = exec_arg2;
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x04: LD_UB - Load from Unified Buffer
            // ================================================================
            6'h04: begin
                ub_rd_en        = 1'b1;
                ub_rd_addr      = {exec_ub_buf_sel, exec_arg1};
                ub_rd_count     = {1'b0, exec_arg2};
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x05: ST_UB - Store to Unified Buffer
            // ================================================================
            // Multi-cycle: Read 3 accumulator addresses, accumulate bytes, write once
            // For 3x3 MATMUL: Results are at stored_acc_addr_reg, stored_acc_addr_reg+1, stored_acc_addr_reg+2
            // exec_arg1 = UB write address (always bank 0 for UART compatibility)
            // exec_arg2 = count (number of bytes, should be 3 for 3x3 results)
            // NOTE: Hazard detection already prevents ST_UB from executing while sys_busy=1
            6'h05: begin
                case (st_ub_state)
                    ST_UB_IDLE: begin
                        // Start: Read acc0 from accumulator
                        acc_rd_en = 1'b1;
                        acc_addr = stored_acc_addr_reg;  // Base address from MATMUL
                        acc_buf_sel = stored_acc_buf_sel_reg;
                        st_ub_col_idx = 2'd0;  // Mark this read as col0
                        pc_cnt_internal = 1'b0;
                        ir_ld_internal = 1'b0;
                    end
                    ST_UB_READ_COL0: begin
                        // CRITICAL FIX: Set idx=1 when reading addr+1
                        // Data from acc0 appears this cycle (idx_prev=0, captures to byte0)
                        // Issue read for acc1 (set idx=1 so next cycle idx_prev=1)
                        acc_rd_en = 1'b1;
                        acc_addr = stored_acc_addr_reg + 1;  // acc1 address
                        acc_buf_sel = stored_acc_buf_sel_reg;
                        st_ub_col_idx = 2'd1;  // Mark this read as col1
                        pc_cnt_internal = 1'b0;
                        ir_ld_internal = 1'b0;
                    end
                    ST_UB_WAIT_COL0: begin
                        // CRITICAL FIX: Set idx=2 when reading addr+2
                        // Data from acc1 appears this cycle (idx_prev=1, captures to byte1)
                        // Issue read for acc2 (set idx=2 so next cycle idx_prev=2)
                        acc_rd_en = 1'b1;
                        acc_addr = stored_acc_addr_reg + 2;  // acc2 address
                        acc_buf_sel = stored_acc_buf_sel_reg;
                        st_ub_col_idx = 2'd2;  // Mark this read as col2
                        pc_cnt_internal = 1'b0;
                        ir_ld_internal = 1'b0;
                    end
                    ST_UB_WAIT_COL1: begin
                        // Data from acc2 appears this cycle (idx_prev=2, captures to byte2)
                        // No more reads needed
                        st_ub_col_idx = 2'd2;  // Keep at col2 (no more captures)
                        pc_cnt_internal = 1'b0;
                        ir_ld_internal = 1'b0;
                    end
                    ST_UB_WAIT_COL2: begin
                        // All 3 bytes captured, prepare to write
                        st_ub_col_idx = 2'd2;  // Keep at col2
                        pc_cnt_internal = 1'b0;
                        ir_ld_internal = 1'b0;
                    end
                    ST_UB_WRITE: begin
                        // Write all 3 bytes to UB as single 256-bit word
                        ub_wr_en = 1'b1;
                        ub_wr_addr = {1'b0, exec_arg1};  // Single address, bank 0
                        ub_wr_count = 9'h001;
                        st_ub_col_idx = 2'd3;  // Indicate write phase
                        pc_cnt_internal = 1'b1;  // Instruction complete
                        ir_ld_internal = 1'b1;
                    end
                    default: begin
                        st_ub_col_idx = 2'd0;
                        pc_cnt_internal = 1'b0;
                        ir_ld_internal = 1'b0;
                    end
                endcase
            end  // Close 6'h05: begin

            // ================================================================
            // 0x10: MATMUL - Matrix Multiplication
            // ================================================================
            6'h10: begin
                sys_start       = 1'b1;
                sys_mode        = 2'b00;  // Matrix multiply mode
                sys_rows        = exec_arg3;
                sys_signed      = exec_flags[1];
                sys_acc_addr    = exec_arg2;
                sys_acc_clear   = 1'b1;
                ub_rd_en        = 1'b1;
                ub_rd_addr      = {exec_ub_buf_sel, exec_arg1};
                ub_rd_count     = 9'h001;
                acc_wr_en       = 1'b1;
                acc_addr        = exec_arg2;
                acc_buf_sel     = exec_acc_buf_sel;
                // Store accumulator address and buffer for ST_UB to read (handled in sequential block)
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x11: CONV2D - 2D Convolution
            // ================================================================
            6'h11: begin
                sys_start       = 1'b1;
                sys_mode        = 2'b01;  // Convolution mode
                sys_rows        = exec_arg3;  // Derived from kernel/stride
                sys_signed      = exec_flags[1];
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
            6'h12: begin
                sys_start       = 1'b1;
                sys_mode        = 2'b10;  // Accumulate mode
                sys_rows        = exec_arg3;
                sys_signed      = exec_flags[1];
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
            6'h18: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h1;   // ReLU mode
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
            6'h19: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h2;   // ReLU6 mode
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
            6'h1A: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h3;   // Sigmoid mode
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
            6'h1B: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h4;   // Tanh mode
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
            6'h20: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h6;   // Max pool mode
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
            6'h21: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h7;   // Avg pool mode
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
            6'h22: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h5;   // Bias add mode
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
            6'h23: begin
                vpu_start       = 1'b1;
                vpu_mode        = 4'h8;   // Batch norm mode
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
            6'h30: begin
                sync_wait       = 1'b1;
                sync_mask       = exec_arg1[3:0];
                sync_timeout    = {exec_arg2, exec_arg3};
                // Toggle buffer selectors that have output ports
                // NOTE: wt_buf_sel removed - weight FIFO uses push/pop, not buffer selection
                acc_buf_sel     = ~exec_acc_buf_sel;
                ub_buf_sel      = ~exec_ub_buf_sel;
                pc_cnt_internal = 1'b0;  // Stall PC!
                ir_ld_internal  = 1'b1;  // Continue loading but don't execute
            end

            // ================================================================
            // 0x31: CFG_REG - Configure Register
            // ================================================================
            6'h31: begin
                cfg_wr_en       = 1'b1;
                cfg_addr        = exec_arg1;
                cfg_data        = {exec_arg2, exec_arg3};
                pc_cnt_internal = 1'b1;
                ir_ld_internal  = 1'b1;
            end

            // ================================================================
            // 0x3F: HALT - Program Termination
            // ================================================================
            6'h3F: begin
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
