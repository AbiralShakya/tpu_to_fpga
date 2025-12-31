module tpu_controller (
    input  logic        clk,
    input  logic        rst_n,
    input  logic        start_execution, // Start signal (resets PC)

    // Instruction memory interface
    output logic [7:0]  instr_addr,      // PC
    input  logic [31:0] instr_data,      // Instruction from ROM

    // Status inputs from Datapath
    input  logic        sys_busy,        // Systolic Array running
    input  logic        sys_done,        // Systolic Array finished
    input  logic        vpu_busy,        // VPU running
    input  logic        dma_busy,        // DMA running
    input  logic        wt_busy,         // Weight FIFO loading
    input  logic        ub_busy,         // Unified Buffer busy
    input  logic        ub_rd_valid,     // UB read data valid (for LD_UB handshaking)

    // =============================================================================
    // CONTROL OUTPUTS TO DATAPATH
    // =============================================================================

    // Systolic Control
    output logic        sys_start,
    output logic [1:0]  sys_mode,        // 00=MatMul, 01=Conv, 10=Acc
    output logic [7:0]  sys_rows,
    output logic        sys_signed,
    output logic [7:0]  sys_acc_addr,    // Accumulator Base Address
    output logic        sys_acc_clear,   // 1 = Wipe accumulator first

    // Unified Buffer (UB) Control
    output logic        ub_rd_en,
    output logic        ub_wr_en,
    output logic [8:0]  ub_rd_addr,      // includes bank bit [8]
    output logic [8:0]  ub_wr_addr,      // includes bank bit [8]
    output logic [8:0]  ub_rd_count,
    output logic [8:0]  ub_wr_count,

    // Weight Control
    output logic        wt_mem_rd_en,    // DRAM Read Enable
    output logic [23:0] wt_mem_addr,     // DRAM Address
    output logic        wt_fifo_wr,      // Push to FIFO
    output logic [7:0]  wt_num_tiles,    // Count

    // Accumulator Control
    output logic        acc_wr_en,       // Direct write (rarely used)
    output logic        acc_rd_en,       // Read for ST_UB/VPU
    output logic [7:0]  acc_addr,
    output logic        acc_buf_sel,     // 0/1 Buffer Select
    output logic [1:0]  st_ub_col_idx,   // 0/1/2 = Capture, 3 = Write

    // VPU Control
    output logic        vpu_start,
    output logic [3:0]  vpu_mode,
    output logic [15:0] vpu_param,

    // DMA / Misc
    output logic        dma_start,
    output logic        dma_dir,
    output logic [7:0]  dma_ub_addr,
    output logic [15:0] dma_length,
    output logic [1:0]  dma_elem_sz,
    output logic        halt_req,        // Done signal
    
    // Legacy Pipeline Control (kept for compatibility, not used in new architecture)
    output logic        pc_cnt,          // Always follows pc_inc
    output logic        pc_ld,           // Not used (no jumps)
    output logic        ir_ld,           // Always 1 in FETCH state
    output logic        if_id_flush,     // Not used (no pipeline)
    
    // Legacy Sync Control (kept for compatibility)
    output logic        sync_wait,       // Not used in new architecture
    output logic [3:0]  sync_mask,       // Not used
    output logic [15:0] sync_timeout,    // Not used
    
    // Legacy Config Control (kept for compatibility, handled internally)
    output logic        cfg_wr_en,       // Not used (handled in S_EXEC_CFG_REG)
    output logic [7:0]  cfg_addr,        // Not used
    output logic [15:0] cfg_data,        // Not used
    
    // Legacy Interrupt (kept for compatibility)
    output logic        interrupt_en,    // Not used
    
    // Legacy Pipeline Status (replaced by fsm_state_debug)
    output logic        pipeline_stall,  // Always 0 (no pipeline)
    output logic [1:0]  current_stage,    // Not used
    
    // Legacy UB Buffer Select (now managed internally via addr[8])
    output logic        ub_buf_sel,      // Not used (bank in addr[8])
    
    // Debug
    output logic [3:0]  fsm_state_debug
);

    // =============================================================================
    // LOCAL PARAMETERS
    // =============================================================================

    // ISA Opcodes (6-bit)
    localparam [5:0] NOP_OP         = 6'h00;
    localparam [5:0] RD_HOST_MEM_OP = 6'h01;
    localparam [5:0] WR_HOST_MEM_OP = 6'h02;
    localparam [5:0] RD_WEIGHT_OP   = 6'h03;
    localparam [5:0] LD_UB_OP       = 6'h04;
    localparam [5:0] ST_UB_OP       = 6'h05;
    localparam [5:0] MATMUL_OP      = 6'h10;
    localparam [5:0] CONV2D_OP      = 6'h11;
    localparam [5:0] MATMUL_ACC_OP  = 6'h12;
    localparam [5:0] RELU_OP        = 6'h18;
    localparam [5:0] RELU6_OP       = 6'h19;
    localparam [5:0] SIGMOID_OP     = 6'h1A;
    localparam [5:0] TANH_OP        = 6'h1B;
    localparam [5:0] MAXPOOL_OP      = 6'h20;
    localparam [5:0] AVGPOOL_OP     = 6'h21;
    localparam [5:0] ADD_BIAS_OP    = 6'h22;
    localparam [5:0] BATCH_NORM_OP   = 6'h23;
    localparam [5:0] SYNC_OP         = 6'h30;
    localparam [5:0] CFG_REG_OP     = 6'h31;
    localparam [5:0] HALT_OP         = 6'h3F;

    // Pipeline latency for ST_UB (BRAM + activation pipeline)
    localparam [7:0] PIPE_LATENCY = 8'd3;  // 1 cycle BRAM + 2 cycles pipeline

    // =============================================================================
    // CONTROLLER FSM STATES
    // =============================================================================
    typedef enum logic [3:0] {
        S_RESET,         // Power-on / Idle
        S_FETCH,         // Read instruction from ROM
        S_DECODE,        // Crack opcode
        S_EXEC_MATMUL,   // Trigger and Wait for Array
        S_EXEC_LD_UB,    // Load Input Activations
        S_EXEC_ST_UB,    // Store Results (The Complex Sequence)
        S_EXEC_WEIGHTS,  // Load Weights
        S_EXEC_VPU,      // Run Activation Function
        S_EXEC_DMA,      // Move Data
        S_EXEC_CFG_REG,  // Configure Register
        S_SYNC,          // Swap Buffers
        S_HALT           // Stop
    } state_t;

    state_t state, next_state;
    assign fsm_state_debug = state;

    // =============================================================================
    // REGISTERS & COUNTERS
    // =============================================================================
    
    // Program Counter
    logic [7:0] pc;
    logic       pc_inc; // Flag to increment PC

    // Instruction Register (Captured at Fetch)
    logic [31:0] ir;
    
    // Executed Instruction Fields (Stored after decode, used in execution)
    logic [5:0] exec_opcode;
    logic [7:0] exec_arg1, exec_arg2, exec_arg3;
    logic [1:0] exec_flags;
    
    // Micro-Sequencer Counter (For multi-cycle steps like ST_UB)
    logic [7:0] micro_cnt;
    logic       reset_micro_cnt;

    // Buffer Select Registers (Managed by SYNC)
    logic cur_acc_buf_sel;
    logic cur_ub_bank_sel;

    // Store accumulator address from MATMUL for ST_UB to read
    logic [7:0] stored_acc_addr_reg;
    logic       stored_acc_buf_sel_reg;
    logic       stored_signed_mode_reg;

    // LD_UB completion tracking (enforce LD_UB -> MATMUL sequence)
    logic ld_ub_completed;

    // Configuration Registers
    logic [15:0] cfg_registers [0:255];

    // Instruction Decoding Aliases
    logic [5:0] opcode;
    logic [7:0] arg1, arg2, arg3;
    logic [1:0] flags;

    assign opcode = ir[31:26];
    assign arg1   = ir[25:18];
    assign arg2   = ir[17:10];
    assign arg3   = ir[9:2];
    assign flags  = ir[1:0];

    // =============================================================================
    // FSM: SEQUENTIAL LOGIC
    // =============================================================================
    
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state              <= S_RESET;
            pc                 <= 8'h00;
            ir                 <= 32'h0;
            micro_cnt          <= 8'h00;
            cur_acc_buf_sel    <= 1'b0;
            cur_ub_bank_sel    <= 1'b0;
            stored_acc_addr_reg <= 8'h00;
            stored_acc_buf_sel_reg <= 1'b0;
            stored_signed_mode_reg <= 1'b0;
            ld_ub_completed    <= 1'b0;
            exec_opcode        <= 6'h00;
            exec_arg1          <= 8'h00;
            exec_arg2          <= 8'h00;
            exec_arg3          <= 8'h00;
            exec_flags         <= 2'b00;
            // Initialize config registers
            for (int i = 0; i < 256; i++) begin
                cfg_registers[i] <= 16'h0000;
            end
        end else begin
            
            // Default counter behavior
            if (reset_micro_cnt) micro_cnt <= 8'h00;
            else micro_cnt <= micro_cnt + 1'b1;

            // PC Management
            if (start_execution && state == S_RESET) begin
                pc <= 8'h00;
            end else if (pc_inc) begin
                pc <= pc + 1'b1;
            end

            // SYNC Buffer Toggling (only when buffers are idle)
            if (state == S_SYNC && micro_cnt == 0 && !ub_busy && !wt_busy) begin
                cur_acc_buf_sel <= ~cur_acc_buf_sel;
                cur_ub_bank_sel <= ~cur_ub_bank_sel;
            end

            // Instruction Fetch
            if (state == S_FETCH) begin
                ir <= instr_data;
            end

            // Instruction Decode - Store fields for execution
            if (state == S_DECODE) begin
                exec_opcode <= opcode;
                exec_arg1   <= arg1;
                exec_arg2   <= arg2;
                exec_arg3   <= arg3;
                exec_flags  <= flags;
            end

            // Store accumulator address when MATMUL starts
            if (state == S_EXEC_MATMUL && micro_cnt == 0) begin
                stored_acc_addr_reg <= exec_arg2;  // sys_acc_addr
                stored_acc_buf_sel_reg <= cur_acc_buf_sel;
                stored_signed_mode_reg <= exec_flags[1];  // sys_signed
            end

            // LD_UB completion tracking
            if (state == S_EXEC_LD_UB && ub_rd_valid) begin
                ld_ub_completed <= 1'b1;
            end else if (state == S_EXEC_MATMUL && sys_done && !sys_busy) begin
                ld_ub_completed <= 1'b0;  // Reset for next MATMUL
            end

            // Configuration Register Write
            if (state == S_EXEC_CFG_REG && micro_cnt == 0) begin
                cfg_registers[exec_arg1] <= {exec_arg2, exec_arg3};
            end

            // State Transition
            state <= next_state;
        end
    end

    // =============================================================================
    // FSM: COMBINATORIAL LOGIC
    // =============================================================================
    always_comb begin
        // Default Outputs
        next_state      = state;
        pc_inc          = 1'b0;
        reset_micro_cnt = 1'b0;
        
        // Signal Defaults
        instr_addr      = pc;
        sys_start       = 1'b0;
        sys_mode        = 2'b00;
        sys_rows        = 8'h00;
        sys_signed      = 1'b0;
        sys_acc_addr    = 8'h00;
        sys_acc_clear   = 1'b0;
        
        ub_rd_en        = 1'b0;
        ub_wr_en        = 1'b0;
        ub_rd_addr      = 9'h000;
        ub_wr_addr      = 9'h000;
        ub_rd_count     = 9'h000;
        ub_wr_count     = 9'h000;

        acc_rd_en       = 1'b0;
        acc_wr_en       = 1'b0;
        acc_addr        = 8'h00;
        st_ub_col_idx   = 2'd0; 
        
        wt_mem_rd_en    = 1'b0;
        wt_mem_addr     = 24'h0;
        wt_fifo_wr      = 1'b0;
        wt_num_tiles    = 8'h0;
        
        vpu_start       = 1'b0;
        vpu_mode        = 4'h0;
        vpu_param       = 16'h0;
        
        dma_start       = 1'b0;
        dma_dir         = 1'b0; // 0=Host->TPU
        dma_ub_addr     = 8'h00;
        dma_length      = 16'h0;
        dma_elem_sz     = 2'b00;
        
        halt_req        = 1'b0;

        // Static Signal Assignments (Buffer Selects)
        acc_buf_sel     = cur_acc_buf_sel;
        
        // Legacy Compatibility Outputs (not used in new architecture)
        pc_cnt          = pc_inc;  // Follow pc_inc
        pc_ld           = 1'b0;    // No jumps in new architecture
        ir_ld           = (state == S_FETCH);  // Load IR in FETCH
        if_id_flush     = 1'b0;    // No pipeline to flush
        sync_wait       = 1'b0;    // Sync handled in S_SYNC state
        sync_mask       = 4'b0000;
        sync_timeout    = 16'h0000;
        cfg_wr_en       = 1'b0;    // Handled internally
        cfg_addr        = 8'h00;
        cfg_data        = 16'h0000;
        interrupt_en    = 1'b0;
        pipeline_stall = 1'b0;    // No pipeline
        current_stage   = 2'b00;   // Not used
        ub_buf_sel      = cur_ub_bank_sel;  // For compatibility
        
        // =========================================================================
        // STATE HANDLER
        // =========================================================================
        case (state)
            
            // ---------------------------------------------------------------------
            S_RESET: begin
                if (start_execution) begin
                    next_state = S_FETCH;
                    reset_micro_cnt = 1'b1;
                end
            end

            // ---------------------------------------------------------------------
            S_FETCH: begin
                // BRAM Read Latency (Instruction Memory) is typically 1 cycle.
                // We assume `instr_data` is valid in the next cycle.
                next_state = S_DECODE;
                reset_micro_cnt = 1'b1;
            end

            // ---------------------------------------------------------------------
            S_DECODE: begin
                // Decode Opcode and Dispatch
                case (opcode)
                    NOP_OP: begin
                        pc_inc = 1'b1;
                        next_state = S_FETCH;
                    end
                    RD_HOST_MEM_OP: next_state = S_EXEC_DMA;
                    WR_HOST_MEM_OP: next_state = S_EXEC_DMA;
                    RD_WEIGHT_OP: next_state = S_EXEC_WEIGHTS;
                    LD_UB_OP: next_state = S_EXEC_LD_UB;
                    ST_UB_OP: next_state = S_EXEC_ST_UB;
                    MATMUL_OP, CONV2D_OP, MATMUL_ACC_OP: begin
                        // Enforce LD_UB -> MATMUL sequence
                        if (!ld_ub_completed) begin
                            // Force LD_UB first (assuming it's in program order)
                            // If not, we could store MATMUL and execute LD_UB here
                            next_state = S_EXEC_LD_UB;
                        end else begin
                            next_state = S_EXEC_MATMUL;
                        end
                    end
                    RELU_OP, RELU6_OP, SIGMOID_OP, TANH_OP,
                    MAXPOOL_OP, AVGPOOL_OP, ADD_BIAS_OP, BATCH_NORM_OP: 
                        next_state = S_EXEC_VPU;
                    SYNC_OP: next_state = S_SYNC;
                    CFG_REG_OP: next_state = S_EXEC_CFG_REG;
                    HALT_OP: next_state = S_HALT;
                    default: begin
                        // Invalid opcode - skip
                        pc_inc = 1'b1;
                        next_state = S_FETCH;
                    end
                endcase
                reset_micro_cnt = 1'b1;
            end

            // ---------------------------------------------------------------------
            S_EXEC_MATMUL: begin
                // Setup signals
                sys_mode     = (exec_opcode == MATMUL_OP) ? 2'b00 : 
                               (exec_opcode == CONV2D_OP) ? 2'b01 : 2'b10;
                sys_rows     = exec_arg3;
                sys_acc_addr = exec_arg2;
                sys_signed   = exec_flags[1];
                
                // Clear Accumulator only on fresh MATMUL/CONV
                sys_acc_clear = (exec_opcode != MATMUL_ACC_OP);

                if (micro_cnt == 0) begin
                    // Cycle 0: Pulse Start
                    sys_start = 1'b1;
                    // Also assert UB read for activations
                    ub_rd_en = 1'b1;
                    ub_rd_addr = {cur_ub_bank_sel, exec_arg1};
                    ub_rd_count = 9'h001;
                end else begin
                    // Wait for completion
                    if (sys_done && !sys_busy) begin
                        pc_inc = 1'b1;
                        next_state = S_FETCH;
                    end
                end
            end

            // ---------------------------------------------------------------------
            S_EXEC_LD_UB: begin
                // Load Unified Buffer -> Datapath Latches
                ub_rd_en    = 1'b1;
                ub_rd_addr  = {cur_ub_bank_sel, exec_arg1};
                ub_rd_count = 9'h001; // Always 1 row

                // Wait for ub_rd_valid (data latched in datapath)
                // Timeout after 3 cycles to prevent deadlock
                if (ub_rd_valid || micro_cnt >= 3) begin
                    pc_inc = 1'b1;
                    next_state = S_FETCH;
                end
            end

            // ---------------------------------------------------------------------
            S_EXEC_ST_UB: begin
                // ATOMIC SEQUENCE: Cannot be interrupted. 
                // Prevents "Zombie Mode" by guaranteeing sequence completion.
                
                case (micro_cnt)
                    8'd0: begin
                        // Step 1: Request Read (triggers passthrough latch in datapath)
                        acc_rd_en = 1'b1;
                        acc_addr = stored_acc_addr_reg;  // From MATMUL
                        acc_buf_sel = stored_acc_buf_sel_reg;
                    end
                    8'd1, 8'd2: begin
                        // Step 2-3: Wait for pipeline latency
                        // BRAM: 1 cycle (acc_rd_data valid on cycle 1)
                        // Activation pipeline: 2 cycles in passthrough mode
                        // Total: 3 cycles from acc_rd_en to ap_valid_col*
                    end
                    8'd3: begin
                        // Step 4: Set col_idx=3 to trigger capture AND write
                        // Datapath captures bytes when: ap_valid_col* && st_ub_col_idx == 3
                        // This happens simultaneously for all 3 columns
                        st_ub_col_idx = 2'd3;
                        ub_wr_en = 1'b1;
                        ub_wr_addr = {1'b0, exec_arg1};  // UB write address (bank 0 for UART)
                        ub_wr_count = 9'h001;
                    end
                    8'd4: begin
                        // Step 5: Complete - passthrough latch cleared when col_idx=3
                        pc_inc = 1'b1;
                        next_state = S_FETCH;
                    end
                endcase
            end

            // ---------------------------------------------------------------------
            S_EXEC_WEIGHTS: begin
                // Load Weights from DRAM -> FIFO
                wt_mem_rd_en = 1'b1;
                wt_mem_addr  = {16'b0, exec_arg1}; // Row index
                wt_fifo_wr   = 1'b1;
                wt_num_tiles = exec_arg2;

                // Wait for transaction to clear
                // Wait for wt_busy to assert then de-assert, or timeout
                if (micro_cnt > 2 && !wt_busy) begin
                    pc_inc = 1'b1;
                    next_state = S_FETCH;
                end else if (micro_cnt > 10) begin
                    // Timeout after 10 cycles
                    pc_inc = 1'b1;
                    next_state = S_FETCH;
                end
            end

            // ---------------------------------------------------------------------
            S_EXEC_VPU: begin
                // Vector Processing Unit
                vpu_start = 1'b1;
                
                // Map opcode to VPU mode
                case (exec_opcode)
                    RELU_OP:      vpu_mode = 4'h1;
                    RELU6_OP:     vpu_mode = 4'h2;
                    SIGMOID_OP:   vpu_mode = 4'h3;
                    TANH_OP:      vpu_mode = 4'h4;
                    ADD_BIAS_OP:  vpu_mode = 4'h5;
                    MAXPOOL_OP:   vpu_mode = 4'h6;
                    AVGPOOL_OP:   vpu_mode = 4'h7;
                    BATCH_NORM_OP: vpu_mode = 4'h8;
                    default:      vpu_mode = 4'h0;
                endcase
                
                // Param selection
                // If flag[0] is set, use Config Register (simplified for now)
                vpu_param = exec_flags[0] ? cfg_registers[{6'b0, exec_flags}] : 16'h0;

                // Also need accumulator read for VPU operations that use accumulator
                if (exec_opcode == RELU_OP || exec_opcode == RELU6_OP || 
                    exec_opcode == SIGMOID_OP || exec_opcode == TANH_OP ||
                    exec_opcode == ADD_BIAS_OP) begin
                    acc_rd_en = 1'b1;
                    acc_addr = exec_arg1;
                    acc_buf_sel = ~cur_acc_buf_sel;  // Read from opposite bank
                end

                // UB read/write for VPU operations
                if (exec_opcode == MAXPOOL_OP || exec_opcode == AVGPOOL_OP ||
                    exec_opcode == BATCH_NORM_OP) begin
                    ub_rd_en = 1'b1;
                    ub_rd_addr = {cur_ub_bank_sel, exec_arg1};
                    ub_rd_count = 9'h001;
                end

                if (exec_opcode != MAXPOOL_OP && exec_opcode != AVGPOOL_OP) begin
                    ub_wr_en = 1'b1;
                    ub_wr_addr = {~cur_ub_bank_sel, exec_arg2};
                    ub_wr_count = 9'h001;
                end

                // Wait for VPU Done
                if (micro_cnt > 0 && !vpu_busy) begin
                    pc_inc = 1'b1;
                    next_state = S_FETCH;
                end
            end

            // ---------------------------------------------------------------------
            S_EXEC_DMA: begin
                dma_start   = 1'b1;
                dma_dir     = (exec_opcode == WR_HOST_MEM_OP); // 1 for Write (TPU->Host)
                dma_ub_addr = exec_arg1;
                dma_length  = {exec_arg2, exec_arg3};
                dma_elem_sz = exec_flags;

                if (micro_cnt > 0 && !dma_busy) begin
                    pc_inc = 1'b1;
                    next_state = S_FETCH;
                end else if (micro_cnt > 100) begin
                    // Timeout after 100 cycles
                    pc_inc = 1'b1;
                    next_state = S_FETCH;
                end
            end

            // ---------------------------------------------------------------------
            S_EXEC_CFG_REG: begin
                // Configuration register write (handled in sequential block)
                if (micro_cnt == 0) begin
                    // Write happens in sequential block
                    pc_inc = 1'b1;
                    next_state = S_FETCH;
                end
            end

            // ---------------------------------------------------------------------
            S_SYNC: begin
                // Toggle buffers (Happens in sequential block on cnt==0)
                // Only toggle if buffers are idle (checked in sequential block)
                if (micro_cnt == 0 && !ub_busy && !wt_busy) begin
                    // Toggle happens in sequential block
                    pc_inc = 1'b1;
                    next_state = S_FETCH;
                end else if (micro_cnt > 0) begin
                    // If buffers were busy, wait
                    if (!ub_busy && !wt_busy) begin
                        pc_inc = 1'b1;
                        next_state = S_FETCH;
                    end else if (micro_cnt > 10) begin
                        // Timeout after 10 cycles
                        pc_inc = 1'b1;
                        next_state = S_FETCH;
                    end
                end
            end

            // ---------------------------------------------------------------------
            S_HALT: begin
                halt_req = 1'b1;
                next_state = S_HALT; // Trap here
            end

        endcase
    end

endmodule
