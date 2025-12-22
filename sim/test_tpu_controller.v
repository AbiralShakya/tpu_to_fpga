/**
 * Simple testbench for TPU controller
 * Tests basic instruction decoding without full system connections
 */
`timescale 1ns / 1ps

module test_tpu_controller;

// Clock and reset
logic clk = 0;
logic rst_n;

// Instruction interface
logic [31:0] instr_data;
logic [7:0]  instr_addr;

// Status inputs (tie to 0 for basic testing)
logic        sys_busy = 0;
logic        vpu_busy = 0;
logic        dma_busy = 0;

// Control outputs (leave unconnected for basic test)
logic        sys_start;
logic [7:0]  sys_rows;
logic [7:0]  ub_rd_addr;
logic        wt_fifo_wr;
logic        vpu_start;
logic [3:0]  vpu_mode;
logic        wt_buf_sel;
logic        acc_buf_sel;
logic        dma_start;
logic        dma_dir;
logic [7:0]  dma_ub_addr;
logic [15:0] dma_length;
logic [1:0]  dma_elem_sz;
logic        cfg_wr_en;
logic [7:0]  cfg_addr;
logic [15:0] cfg_data;

// Pipeline status
logic        pipeline_stall;
logic [1:0]  current_stage;

// DUT
tpu_controller dut (
    .clk             (clk),
    .rst_n           (rst_n),
    .instr_addr      (instr_addr),
    .instr_data      (instr_data),
    .sys_busy        (sys_busy),
    .vpu_busy        (vpu_busy),
    .dma_busy        (dma_busy),
    .sys_start       (sys_start),
    .sys_rows        (sys_rows),
    .ub_rd_addr      (ub_rd_addr),
    .wt_fifo_wr      (wt_fifo_wr),
    .vpu_start       (vpu_start),
    .vpu_mode        (vpu_mode),
    .wt_buf_sel      (wt_buf_sel),
    .acc_buf_sel     (acc_buf_sel),
    .dma_start       (dma_start),
    .dma_dir         (dma_dir),
    .dma_ub_addr     (dma_ub_addr),
    .dma_length      (dma_length),
    .dma_elem_sz     (dma_elem_sz),
    .cfg_wr_en       (cfg_wr_en),
    .cfg_addr        (cfg_addr),
    .cfg_data        (cfg_data),
    .pipeline_stall  (pipeline_stall),
    .current_stage   (current_stage)
);

// Clock generation
always #5 clk = ~clk;

// Test sequence
initial begin
    $display("Starting TPU Controller Test...");

    // Initialize
    rst_n = 0;
    instr_data = 32'h00000000;

    #20 rst_n = 1;

    $display("Time: %0t - Reset deasserted", $time);

    // Test NOP instruction
    instr_data = 32'h00000000; // NOP
    #30;
    $display("Time: %0t - NOP instruction: Stage=%b, Stall=%b", $time, current_stage, pipeline_stall);

    // Test MATMUL instruction
    instr_data = {6'h01, 8'h00, 8'h20, 8'h03, 2'b00}; // MATMUL
    #30;
    $display("Time: %0t - MATMUL instruction: Stage=%b, Stall=%b, sys_start=%b", $time, current_stage, pipeline_stall, sys_start);

    // Test RELU instruction
    instr_data = {6'h03, 8'h20, 8'h40, 8'h09, 2'b00}; // RELU
    #30;
    $display("Time: %0t - RELU instruction: Stage=%b, Stall=%b, vpu_start=%b", $time, current_stage, pipeline_stall, vpu_start);

    // Test SYNC instruction
    instr_data = {6'h04, 8'h00, 8'h00, 8'h00, 2'b00}; // SYNC
    #30;
    $display("Time: %0t - SYNC instruction: Stage=%b, Stall=%b", $time, current_stage, pipeline_stall);

    // Test hazard detection
    sys_busy = 1;
    instr_data = {6'h01, 8'h00, 8'h40, 8'h05, 2'b00}; // MATMUL while busy
    #30;
    $display("Time: %0t - Hazard test: Stage=%b, Stall=%b (should be stalled)", $time, current_stage, pipeline_stall);

    sys_busy = 0;
    #30;
    $display("Time: %0t - Hazard cleared: Stage=%b, Stall=%b", $time, current_stage, pipeline_stall);

    $display("TPU Controller test completed!");
    $finish;
end

// Monitor
initial begin
    $monitor("Time: %0t | PC: %h | Stage: %b | Stall: %b | sys_start: %b | vpu_start: %b",
             $time, instr_addr, current_stage, pipeline_stall, sys_start, vpu_start);
end

endmodule
