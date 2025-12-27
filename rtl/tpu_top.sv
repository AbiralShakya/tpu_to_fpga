`timescale 1ns / 1ps

module tpu_top (
    input  logic        clk,           // System clock (100MHz)
    input  logic        rst_n,         // Active-low reset

    // Basys 3 Physical Interface
    input  logic [15:0] sw,            // 16 slide switches
    input  logic [4:0]  btn,           // 5 push buttons [BTNU, BTND, BTNL, BTNR, BTNC]

    // 7-Segment Display Interface
    output logic [6:0]  seg,           // 7-segment segments (A-G)
    output logic [3:0]  an,            // 7-segment anodes
    output logic        dp,            // Decimal point

    // Status LEDs
    output logic [15:0] led,           // 16 green LEDs

    // UART Interface (for Basys3 development)
    input  logic        uart_rx,
    output logic        uart_tx,

    // Legacy DMA interface (can be removed when using UART)
    input  logic        dma_start_in,
    input  logic        dma_dir_in,    // 0: host→TPU, 1: TPU→host
    input  logic [7:0]  dma_ub_addr_in,
    input  logic [15:0] dma_length_in,
    input  logic [1:0]  dma_elem_sz_in,
    input  logic [255:0] dma_data_in,  // Host data input
    output logic        dma_busy_out,
    output logic        dma_done_out,
    output logic [255:0] dma_data_out, // Host data output

    // Status outputs
    output logic        tpu_busy,
    output logic        tpu_done,

    // Debug outputs
    output logic [1:0]  pipeline_stage,
    output logic        hazard_detected,
    output logic [7:0]  uart_debug_state,
    output logic [7:0]  uart_debug_cmd,
    output logic [15:0] uart_debug_byte_count,

    // Bank selection debug (active during ISA execution)
    output logic [7:0]  debug_bank_state
);

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

// =============================================================================
// CONTROLLER TO DATAPATH CONTROL SIGNALS (29 signals)
// =============================================================================

// Systolic Array Control (7 signals)
logic        sys_start;
logic [1:0]  sys_mode;
logic [7:0]  sys_rows;
logic        sys_signed;
logic        sys_transpose;
logic [7:0]  sys_acc_addr;
logic        sys_acc_clear;

// Unified Buffer Control (7 signals)
// Controller outputs
logic        ctrl_ub_rd_en;
logic        ctrl_ub_wr_en;
logic [8:0]  ctrl_ub_rd_addr;
logic [8:0]  ctrl_ub_wr_addr;
logic [8:0]  ctrl_ub_rd_count;
logic [8:0]  ctrl_ub_wr_count;
logic        ctrl_ub_buf_sel;

// Multiplexed signals to datapath
logic        ub_rd_en;
logic        ub_wr_en;
logic [8:0]  ub_rd_addr;
logic [8:0]  ub_wr_addr;
logic [8:0]  ub_rd_count;
logic [8:0]  ub_wr_count;
logic        ub_buf_sel;

// Weight FIFO Control (5 signals)
logic        wt_mem_rd_en;
logic [23:0] wt_mem_addr;
logic        wt_fifo_wr;
logic [7:0]  wt_num_tiles;
logic        wt_buf_sel;

// Accumulator Control (4 signals)
logic        acc_wr_en;
logic        acc_rd_en;
logic [7:0]  acc_addr;
logic        acc_buf_sel;

// VPU Control (6 signals)
logic        vpu_start;
logic [3:0]  vpu_mode;
logic [7:0]  vpu_in_addr;
logic [7:0]  vpu_out_addr;
logic [7:0]  vpu_length;
logic [15:0] vpu_param;

// =============================================================================
// DATAPATH STATUS TO CONTROLLER (4 signals)
// =============================================================================

logic        sys_busy;
logic        vpu_busy;
logic        dma_busy;
logic        wt_busy;
logic        ub_busy;  // Unified buffer busy

// =============================================================================
// DATAPATH DATA INTERFACES
// =============================================================================

logic [63:0] wt_fifo_data;  // Full weight row (8 bytes, use bytes 0-2 for 3 columns)
logic [255:0] ub_wr_data;
logic [255:0] ub_rd_data;
logic        ub_rd_valid;  // Unified buffer read data valid
logic [255:0] acc_data_out; // Accumulator output for ST_UB instruction

// =============================================================================
// BASYS3 TEST INTERFACE TO DATAPATH INTERFACES
// =============================================================================

logic        test_ub_wr_en;
logic [8:0]  test_ub_wr_addr;  // 9-bit address: [8]=bank, [7:0]=address
logic [8:0]  test_ub_wr_count; // 9-bit burst count
logic [255:0] test_ub_wr_data;
logic        test_ub_rd_en;
logic [8:0]  test_ub_rd_addr;  // 9-bit address: [8]=bank, [7:0]=address
logic [8:0]  test_ub_rd_count; // 9-bit burst count
logic        test_wt_wr_en;
logic [9:0]  test_wt_wr_addr;
logic [63:0] test_wt_wr_data;
logic        test_instr_wr_en;
logic [4:0]  test_instr_wr_addr;
logic [31:0] test_instr_wr_data;
logic        test_start_execution;

// =============================================================================
// DMA CONTROL SIGNALS (LEGACY)
// =============================================================================

logic        dma_start;
logic        dma_dir;
logic [7:0]  dma_ub_addr;
logic [15:0] dma_length;
logic [1:0]  dma_elem_sz;

// =============================================================================
// PIPELINE STATUS
// =============================================================================

logic        pipeline_stall;
logic [1:0]  current_stage;

// =============================================================================
// INTERNAL CONTROL SIGNALS (not used externally)
// =============================================================================

logic        pc_cnt;          // Internal PC control
logic        pc_ld;           // Internal PC load
logic        ir_ld;           // Internal IR load
logic        if_id_flush;     // Internal pipeline flush
logic        sync_wait;        // Internal sync wait
logic [3:0]  sync_mask;        // Internal sync mask
logic [15:0] sync_timeout;     // Internal sync timeout
logic        cfg_wr_en;        // Internal config write enable
logic [7:0]  cfg_addr;         // Internal config address
logic [15:0] cfg_data;         // Internal config data
logic        halt_req;         // Internal halt request
logic        interrupt_en;     // Internal interrupt enable

// =============================================================================
// INSTRUCTION MEMORY (32 x 32-bit)
// =============================================================================

logic [7:0]  instr_addr;
logic [31:0] instr_data;

(* ram_style = "block" *) logic [31:0] instruction_memory [0:31];

// Instruction memory write port (from UART DMA)
always @(posedge clk) begin
    if (test_instr_wr_en) begin
        instruction_memory[test_instr_wr_addr] <= test_instr_wr_data;
    end
end

// Instruction memory read port (to controller) - combinational read
assign instr_data = instruction_memory[instr_addr[4:0]];

// =============================================================================
// WEIGHT MEMORY (256 x 64-bit = 2KB)
// =============================================================================
// Stores weight rows. Each row is 64 bits (8 bytes).
// For 3x3 matrix: bytes 0,1,2 are the 3 weights per row.
// UART WRITE_WT stores weights, RD_WEIGHT instruction reads them.

(* ram_style = "block" *) logic [63:0] weight_memory [0:255];
logic [63:0] wt_mem_rd_data;  // Data read from weight memory

// Weight memory write port (from UART DMA)
always @(posedge clk) begin
    if (test_wt_wr_en) begin
        weight_memory[test_wt_wr_addr[7:0]] <= test_wt_wr_data;
    end
end

// Weight memory read port (for RD_WEIGHT instruction)
// Combinational read - data available immediately when address is set
// Using lower 8 bits of wt_mem_addr as row address
assign wt_mem_rd_data = weight_memory[wt_mem_addr[7:0]];

// =============================================================================
// CONTROLLER INSTANCE
// =============================================================================

tpu_controller controller (
    .clk            (clk),
    .rst_n          (rst_n),

    // Instruction interface
    .instr_addr     (instr_addr),
    .instr_data     (instr_data),

    // Status inputs from datapath
    .sys_busy       (sys_busy),
    .vpu_busy       (vpu_busy),
    .dma_busy       (dma_busy),
    .wt_busy        (wt_busy),
    .ub_busy        (ub_busy),  // Unified buffer busy (for buffer toggle safety)

    // ========================================================================
    // CONTROL OUTPUTS TO DATAPATH (29 signals)
    // ========================================================================

    // Systolic Array Control
    .sys_start      (sys_start),
    .sys_mode      (sys_mode),
    .sys_rows      (sys_rows),
    .sys_signed    (sys_signed),
    .sys_transpose (sys_transpose),
    .sys_acc_addr  (sys_acc_addr),
    .sys_acc_clear (sys_acc_clear),

    // Unified Buffer Control (from controller)
    .ub_rd_en      (ctrl_ub_rd_en),
    .ub_wr_en      (ctrl_ub_wr_en),
    .ub_rd_addr    (ctrl_ub_rd_addr),
    .ub_wr_addr    (ctrl_ub_wr_addr),
    .ub_rd_count   (ctrl_ub_rd_count),
    .ub_wr_count   (ctrl_ub_wr_count),
    .ub_buf_sel    (ctrl_ub_buf_sel),

    // Weight FIFO Control
    .wt_mem_rd_en  (wt_mem_rd_en),
    .wt_mem_addr   (wt_mem_addr),
    .wt_fifo_wr    (wt_fifo_wr),
    .wt_num_tiles  (wt_num_tiles),
    .wt_buf_sel    (wt_buf_sel),

    // Accumulator Control
    .acc_wr_en     (acc_wr_en),
    .acc_rd_en     (acc_rd_en),
    .acc_addr      (acc_addr),
    .acc_buf_sel   (acc_buf_sel),

    // VPU Control
    .vpu_start     (vpu_start),
    .vpu_mode      (vpu_mode),
    .vpu_in_addr   (vpu_in_addr),
    .vpu_out_addr  (vpu_out_addr),
    .vpu_length    (vpu_length),
    .vpu_param     (vpu_param),

    // DMA Control (passed through to external DMA)
    .dma_start     (dma_start),
    .dma_dir       (dma_dir),
    .dma_ub_addr   (dma_ub_addr),
    .dma_length    (dma_length),
    .dma_elem_sz   (dma_elem_sz),

    // Internal pipeline control (not used externally)
    .pc_cnt        (pc_cnt),
    .pc_ld         (pc_ld),
    .ir_ld         (ir_ld),
    .if_id_flush   (if_id_flush),

    // Sync/Control (internal)
    .sync_wait     (sync_wait),
    .sync_mask     (sync_mask),
    .sync_timeout  (sync_timeout),

    // Configuration Control (internal)
    .cfg_wr_en     (cfg_wr_en),
    .cfg_addr      (cfg_addr),
    .cfg_data      (cfg_data),

    // Halt/Interrupt (internal)
    .halt_req      (halt_req),
    .interrupt_en  (interrupt_en),

    // Pipeline status
    .pipeline_stall (pipeline_stall),
    .current_stage  (current_stage)
);

// =============================================================================
// BASYS3 TEST INTERFACE
// =============================================================================

basys3_test_interface test_interface (
    .clk                (clk),
    .rst_n              (rst_n),

    // Physical Interface - Basys 3
    .sw                 (sw),
    .btn                (btn),

    // 7-Segment Display Interface
    .seg                (seg),
    .an                 (an),
    .dp                 (dp),

    // Status LEDs
    .led                (led),

    // UART Interface (passthrough)
    .uart_rx            (uart_rx),
    .uart_tx            (uart_tx),

    // To Unified Buffer
    .ub_wr_en           (test_ub_wr_en),
    .ub_wr_addr         (test_ub_wr_addr),
    .ub_wr_count        (test_ub_wr_count),
    .ub_wr_data         (test_ub_wr_data),
    .ub_rd_en           (test_ub_rd_en),
    .ub_rd_addr         (test_ub_rd_addr),
    .ub_rd_count        (test_ub_rd_count),
    .ub_rd_data         (ub_rd_data),       // From datapath UB
    .ub_rd_valid        (ub_rd_valid),      // From datapath UB

    // To Weight Memory
    .wt_wr_en           (test_wt_wr_en),
    .wt_wr_addr         (test_wt_wr_addr),
    .wt_wr_data         (test_wt_wr_data),

    // To Instruction Buffer
    .instr_wr_en        (test_instr_wr_en),
    .instr_wr_addr      (test_instr_wr_addr),
    .instr_wr_data      (test_instr_wr_data),

    // To Controller
    .start_execution    (test_start_execution),

    // From Datapath (status)
    .sys_busy           (sys_busy),
    .sys_done           (sys_done),
    .vpu_busy           (vpu_busy),
    .vpu_done           (vpu_done),
    .ub_busy            (ub_busy),         // From datapath
    .ub_done            (ub_done)          // From datapath
);

// =============================================================================
// DATAPATH INSTANCE
// =============================================================================

tpu_datapath datapath (
    .clk            (clk),
    .rst_n          (rst_n),

    // ========================================================================
    // CONTROL INPUTS FROM CONTROLLER (29 signals)
    // ========================================================================

    // Systolic Array Control
    .sys_start      (sys_start),
    .sys_mode       (sys_mode),
    .sys_rows       (sys_rows),
    .sys_signed     (sys_signed),
    .sys_transpose  (sys_transpose),
    .sys_acc_addr   (sys_acc_addr),
    .sys_acc_clear  (sys_acc_clear),

    // Unified Buffer Control
    .ub_rd_en       (ub_rd_en),
    .ub_wr_en       (ub_wr_en),
    .ub_rd_addr     (ub_rd_addr),
    .ub_wr_addr     (ub_wr_addr),
    .ub_rd_count    (ub_rd_count),
    .ub_wr_count    (ub_wr_count),
    .ub_buf_sel     (ub_buf_sel),

    // Weight FIFO Control
    .wt_mem_rd_en   (wt_mem_rd_en),
    .wt_mem_addr    (wt_mem_addr),
    .wt_fifo_wr     (wt_fifo_wr),
    .wt_num_tiles   (wt_num_tiles),
    .wt_buf_sel     (wt_buf_sel),

    // Accumulator Control
    .acc_wr_en      (acc_wr_en),
    .acc_rd_en      (acc_rd_en),
    .acc_addr       (acc_addr),
    .acc_buf_sel    (acc_buf_sel),

    // VPU Control
    .vpu_start      (vpu_start),
    .vpu_mode       (vpu_mode),
    .vpu_in_addr    (vpu_in_addr),
    .vpu_out_addr   (vpu_out_addr),
    .vpu_length     (vpu_length),
    .vpu_param      (vpu_param),

    // Data interfaces
    .wt_fifo_data   (wt_fifo_data),
    .ub_wr_data     (ub_wr_data),
    .ub_rd_data     (ub_rd_data),
    .ub_rd_valid    (ub_rd_valid),  // Unified buffer read data valid
    .acc_data_out   (acc_data_out), // Accumulator output for ST_UB

    // Status outputs to controller
    .sys_busy       (sys_busy),
    .sys_done       (sys_done),  // Connected for UART status reporting
    .vpu_busy       (vpu_busy),
    .vpu_done       (vpu_done),  // Connected for UART status reporting
    .dma_busy       (dma_busy),
    .dma_done       (),  // Not used
    .wt_busy        (wt_busy),
    .ub_busy        (ub_busy),  // Unified buffer busy
    .ub_done        (ub_done)   // Unified buffer done (for UART status)
);

// =============================================================================
// DMA INTERFACE LOGIC (UART takes priority over legacy DMA)
// =============================================================================

// Multiplex between TEST INTERFACE and CONTROLLER
// TEST INTERFACE takes priority when active (for programming)
logic use_test_interface;
assign use_test_interface = test_ub_wr_en | test_ub_rd_en | test_wt_wr_en | test_instr_wr_en;

// Unified Buffer connections (TEST INTERFACE takes priority for programming, otherwise CONTROLLER)
// When controller writes (ST_UB instruction), use accumulator output; when UART writes, use test data
assign ub_wr_data = use_test_interface ? test_ub_wr_data : acc_data_out;
assign ub_wr_en = use_test_interface ? test_ub_wr_en : ctrl_ub_wr_en;
assign ub_rd_en = use_test_interface ? test_ub_rd_en : ctrl_ub_rd_en;
assign ub_wr_addr = use_test_interface ? test_ub_wr_addr : ctrl_ub_wr_addr;  // Already 9-bit
assign ub_rd_addr = use_test_interface ? test_ub_rd_addr : ctrl_ub_rd_addr;  // Already 9-bit
assign ub_wr_count = use_test_interface ? test_ub_wr_count : ctrl_ub_wr_count;
assign ub_rd_count = use_test_interface ? test_ub_rd_count : ctrl_ub_rd_count;
// For UART: Use ub_buf_sel=1 for writes (so wr_bank_sel=0), ub_buf_sel=0 for reads (so rd_bank_sel=0)
// This ensures both access bank 0, overcoming the double-buffering design
assign ub_buf_sel = use_test_interface ? (test_ub_wr_en ? 1'b1 : 1'b0) : ctrl_ub_buf_sel;

// Weight FIFO data (from TEST INTERFACE or legacy DMA)
// Weight data comes from weight memory during RD_WEIGHT execution
// wt_mem_rd_data contains the full 64-bit row read from weight memory
assign wt_fifo_data = wt_mem_rd_data;

// Legacy DMA control signals
assign dma_start = dma_start_in;
assign dma_dir = dma_dir_in;
assign dma_ub_addr = dma_ub_addr_in;
assign dma_length = dma_length_in;
assign dma_elem_sz = dma_elem_sz_in;

// DMA outputs (TEST INTERFACE overrides legacy)
assign dma_data_out = ub_rd_data;  // Always output from UB
assign dma_busy_out = use_test_interface ? 1'b0 : dma_busy;  // TEST INTERFACE is synchronous
assign dma_done_out = 1'b1;  // Simplified - always done

// Controller start signal (from TEST INTERFACE)
logic controller_start;
assign controller_start = test_start_execution;

// =============================================================================
// TOP-LEVEL STATUS
// =============================================================================

assign tpu_busy = sys_busy | vpu_busy | dma_busy | wt_busy;
assign tpu_done = ~tpu_busy;  // Simplified

// =============================================================================
// DEBUG OUTPUTS
// =============================================================================

assign pipeline_stage = current_stage;
assign hazard_detected = pipeline_stall;

// Bank selection debug signal:
// [7]   = use_test_interface (1 = UART controls UB, 0 = controller controls UB)
// [6]   = ub_buf_sel (actual signal to unified_buffer)
// [5]   = ctrl_ub_buf_sel (controller's buffer selection)
// [4]   = ub_busy (unified buffer busy flag)
// [3]   = ub_rd_en (read enable)
// [2]   = ub_wr_en (write enable)
// [1]   = test_ub_rd_en (UART read enable)
// [0]   = test_ub_wr_en (UART write enable)
assign debug_bank_state = {
    use_test_interface,
    ub_buf_sel,
    ctrl_ub_buf_sel,
    ub_busy,
    ub_rd_en,
    ub_wr_en,
    test_ub_rd_en,
    test_ub_wr_en
};

// UART debug signals - directly from test interface would require port changes
// For now, leave unconnected (tied to 0) - basys3_test_interface has them internally
assign uart_debug_state = 8'h00;
assign uart_debug_cmd = 8'h00;
assign uart_debug_byte_count = 16'h0000;

endmodule
