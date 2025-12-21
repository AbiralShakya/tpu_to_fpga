`timescale 1ns / 1ps

module tpu_top (
    input  logic        clk,           // System clock (100MHz)
    input  logic        rst_n,         // Active-low reset

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
    output logic [15:0] uart_debug_byte_count
);

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

// Controller to datapath control signals
logic        sys_start;
logic [7:0]  sys_rows;
logic [7:0]  ub_rd_addr;
logic        wt_fifo_wr;
logic        vpu_start;
logic [3:0]  vpu_mode;
logic        wt_buf_sel;
logic        acc_buf_sel;

// Datapath status to controller
logic        sys_busy;
logic        vpu_busy;
logic        dma_busy;

// Datapath data interfaces
logic [15:0] wt_fifo_data;
logic [255:0] ub_wr_data;
logic [255:0] ub_rd_data;
logic        ub_wr_en;
logic        ub_rd_en;

// UART DMA to datapath interfaces
logic        uart_ub_wr_en;
logic [7:0]  uart_ub_wr_addr;
logic [255:0] uart_ub_wr_data;
logic        uart_ub_rd_en;
logic [7:0]  uart_ub_rd_addr;
logic        uart_wt_wr_en;
logic [9:0]  uart_wt_wr_addr;
logic [63:0] uart_wt_wr_data;
logic        uart_instr_wr_en;
logic [4:0]  uart_instr_wr_addr;
logic [31:0] uart_instr_wr_data;
logic        uart_start_execution;

// DMA control signals (legacy)
logic        dma_start;
logic        dma_dir;
logic [7:0]  dma_ub_addr;
logic [15:0] dma_length;
logic [1:0]  dma_elem_sz;

// Pipeline status
logic        pipeline_stall;
logic [1:0]  current_stage;

// =============================================================================
// CONTROLLER INSTANCE
// =============================================================================

tpu_controller controller (
    .clk            (clk),
    .rst_n          (rst_n),

    // Instruction interface (now comes from UART DMA)
    .instr_addr     (),  // Not used with UART DMA
    .instr_data     (32'h00000000),  // Controller uses internal program when UART triggers execution

    // Status inputs
    .sys_busy       (sys_busy),
    .vpu_busy       (vpu_busy),
    .dma_busy       (dma_busy),

    // Control outputs
    .sys_start      (sys_start),
    .sys_rows       (sys_rows),
    .ub_rd_addr     (ub_rd_addr),
    .wt_fifo_wr     (wt_fifo_wr),
    .vpu_start      (vpu_start),
    .vpu_mode       (vpu_mode),
    .wt_buf_sel     (wt_buf_sel),
    .acc_buf_sel    (acc_buf_sel),

    // DMA interface (passed through)
    .dma_start      (dma_start),
    .dma_dir        (dma_dir),
    .dma_ub_addr    (dma_ub_addr),
    .dma_length     (dma_length),
    .dma_elem_sz    (dma_elem_sz),

    // Pipeline status
    .pipeline_stall (pipeline_stall),
    .current_stage  (current_stage)
);

// =============================================================================
// UART DMA CONTROLLER (Basys3 Interface)
// =============================================================================

uart_dma_basys3 uart_dma (
    .clk                (clk),
    .rst_n              (rst_n),

    // UART Interface
    .uart_rx            (uart_rx),
    .uart_tx            (uart_tx),

    // To Unified Buffer
    .ub_wr_en           (uart_ub_wr_en),
    .ub_wr_addr         (uart_ub_wr_addr),
    .ub_wr_data         (uart_ub_wr_data),
    .ub_rd_en           (uart_ub_rd_en),
    .ub_rd_addr         (uart_ub_rd_addr),
    .ub_rd_data         (ub_rd_data),       // From datapath UB

    // To Weight Memory
    .wt_wr_en           (uart_wt_wr_en),
    .wt_wr_addr         (uart_wt_wr_addr),
    .wt_wr_data         (uart_wt_wr_data),

    // To Instruction Buffer
    .instr_wr_en        (uart_instr_wr_en),
    .instr_wr_addr      (uart_instr_wr_addr),
    .instr_wr_data      (uart_instr_wr_data),

    // To Controller
    .start_execution    (uart_start_execution),

    // From Datapath (status)
    .sys_busy           (sys_busy),
    .sys_done           (1'b1),             // Assume done
    .vpu_busy           (vpu_busy),
    .vpu_done           (1'b1),             // Assume done
    .ub_busy            (1'b0),             // Assume not busy
    .ub_done            (1'b1),             // Assume done

    // Debug outputs
    .debug_state        (uart_debug_state),
    .debug_cmd          (uart_debug_cmd),
    .debug_byte_count   (uart_debug_byte_count)
);

// =============================================================================
// DATAPATH INSTANCE
// =============================================================================

tpu_datapath datapath (
    .clk            (clk),
    .rst_n          (rst_n),

    // Control inputs
    .sys_start      (sys_start),
    .sys_rows       (sys_rows),
    .ub_rd_addr     (ub_rd_addr),
    .wt_fifo_wr     (wt_fifo_wr),
    .vpu_start      (vpu_start),
    .vpu_mode       (vpu_mode),
    .wt_buf_sel     (wt_buf_sel),
    .acc_buf_sel    (acc_buf_sel),

    // Data interfaces
    .wt_fifo_data   (wt_fifo_data),
    .ub_wr_data     (ub_wr_data),
    .ub_rd_data     (ub_rd_data),

    // Status outputs
    .sys_busy       (sys_busy),
    .sys_done       (),  // Not used in controller
    .vpu_busy       (vpu_busy),
    .vpu_done       (),  // Not used in controller
    .dma_busy       (dma_busy),
    .dma_done       ()   // Not used in controller
);

// =============================================================================
// DMA INTERFACE LOGIC (UART takes priority over legacy DMA)
// =============================================================================

// Multiplex between UART DMA and legacy DMA
// UART DMA takes priority when active
logic use_uart_dma;
assign use_uart_dma = uart_ub_wr_en | uart_ub_rd_en | uart_wt_wr_en | uart_instr_wr_en;

// Unified Buffer connections (UART DMA takes priority)
assign ub_wr_data = use_uart_dma ? uart_ub_wr_data : dma_data_in;
assign ub_wr_en = use_uart_dma ? uart_ub_wr_en : 1'b0;  // UART controls write enable
assign ub_rd_en = use_uart_dma ? uart_ub_rd_en : 1'b0;  // UART controls read enable

// Weight FIFO data (from UART or legacy DMA)
assign wt_fifo_data = use_uart_dma ? uart_wt_wr_data[15:0] : dma_data_in[15:0];

// Legacy DMA control signals
assign dma_start = dma_start_in;
assign dma_dir = dma_dir_in;
assign dma_ub_addr = dma_ub_addr_in;
assign dma_length = dma_length_in;
assign dma_elem_sz = dma_elem_sz_in;

// DMA outputs (UART DMA overrides legacy)
assign dma_data_out = ub_rd_data;  // Always output from UB
assign dma_busy_out = use_uart_dma ? 1'b0 : dma_busy;  // UART DMA is synchronous
assign dma_done_out = 1'b1;  // Simplified - always done

// Controller start signal (from UART DMA)
logic controller_start;
assign controller_start = uart_start_execution;

// =============================================================================
// TOP-LEVEL STATUS
// =============================================================================

assign tpu_busy = sys_busy | vpu_busy | dma_busy;
assign tpu_done = ~tpu_busy;  // Simplified

// =============================================================================
// DEBUG OUTPUTS
// =============================================================================

assign pipeline_stage = current_stage;
assign hazard_detected = pipeline_stall;

endmodule
