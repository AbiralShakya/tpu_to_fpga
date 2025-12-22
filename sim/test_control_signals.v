`timescale 1ns / 1ps

module test_control_signals;

    // Testbench signals
    reg clk;
    reg rst_n;
    reg [31:0] instr_data;
    reg sys_busy;
    reg vpu_busy;
    reg dma_busy;
    reg wt_busy;

    // Controller outputs
    wire pc_cnt, ir_ld, sys_start, ub_rd_en, ub_wr_en, vpu_start, dma_start;
    wire [8:0] ub_rd_addr, ub_wr_addr;
    wire ub_buf_sel, acc_buf_sel, wt_buf_sel;
    wire pipeline_stall;
    wire [1:0] current_stage;

    // DUT instantiation (minimal connections for testing)
    tpu_controller dut (
        .clk            (clk),
        .rst_n          (rst_n),
        .instr_data     (instr_data),
        .sys_busy       (sys_busy),
        .vpu_busy       (vpu_busy),
        .dma_busy       (dma_busy),
        .wt_busy        (wt_busy),
        .pc_cnt         (pc_cnt),
        .ir_ld          (ir_ld),
        .sys_start      (sys_start),
        .ub_rd_en       (ub_rd_en),
        .ub_wr_en       (ub_wr_en),
        .ub_rd_addr     (ub_rd_addr),
        .ub_wr_addr     (ub_wr_addr),
        .ub_buf_sel     (ub_buf_sel),
        .acc_buf_sel    (acc_buf_sel),
        .wt_buf_sel     (wt_buf_sel),
        .vpu_start      (vpu_start),
        .dma_start      (dma_start),
        .pipeline_stall (pipeline_stall),
        .current_stage  (current_stage)
    );

    // Clock generation
    initial begin
        clk = 0;
        forever #5 clk = ~clk;  // 100MHz clock
    end

    // Instruction creation function
    function [31:0] make_instr;
        input [5:0] opcode;
        input [7:0] arg1, arg2, arg3;
        input [1:0] flags;
        begin
            make_instr = {opcode, arg1, arg2, arg3, flags};
        end
    endfunction

    // Test sequence
    initial begin
        $display("Control Signal Verification Test");
        $display("=================================");

        // Initialize
        rst_n = 0;
        instr_data = 32'h0;
        sys_busy = 0;
        vpu_busy = 0;
        dma_busy = 0;
        wt_busy = 0;

        #20 rst_n = 1;

        // Test NOP
        $display("\nTesting NOP instruction:");
        instr_data = make_instr(6'h00, 8'h00, 8'h00, 8'h00, 2'b00);
        #30;  // Wait for pipeline
        $display("NOP: pc_cnt=%b, ir_ld=%b, sys_start=%b, ub_rd_en=%b",
                pc_cnt, ir_ld, sys_start, ub_rd_en);

        // Test MATMUL
        $display("\nTesting MATMUL instruction:");
        instr_data = make_instr(6'h10, 8'h00, 8'h20, 8'h04, 2'b00);
        #30;
        $display("MATMUL: sys_start=%b, ub_rd_en=%b, ub_rd_addr=0x%h, ub_buf_sel=%b",
                sys_start, ub_rd_en, ub_rd_addr, ub_buf_sel);

        // Test RELU
        $display("\nTesting RELU instruction:");
        instr_data = make_instr(6'h18, 8'h20, 8'h40, 8'h04, 2'b00);
        #30;
        $display("RELU: vpu_start=%b, ub_wr_en=%b, ub_wr_addr=0x%h, ub_buf_sel=%b",
                vpu_start, ub_wr_en, ub_wr_addr, ub_buf_sel);

        // Test SYNC
        $display("\nTesting SYNC instruction:");
        instr_data = make_instr(6'h30, 8'h03, 8'h00, 8'h01, 2'b00);
        #30;
        $display("SYNC: pipeline_stall=%b, ub_buf_sel=%b, acc_buf_sel=%b, wt_buf_sel=%b",
                pipeline_stall, ub_buf_sel, acc_buf_sel, wt_buf_sel);

        // Test DMA read
        $display("\nTesting RD_HOST_MEM instruction:");
        instr_data = make_instr(6'h01, 8'h00, 8'h00, 8'h10, 2'b00);
        #30;
        $display("RD_HOST_MEM: dma_start=%b, ub_buf_sel=%b", dma_start, ub_buf_sel);

        $display("\nControl signal test completed!");
        #50 $finish;
    end

    // Monitor pipeline
    always @(posedge clk) begin
        if (rst_n && current_stage != 2'b00) begin
            $display("Time %0t: Stage=%b, Stall=%b", $time, current_stage, pipeline_stall);
        end
    end

endmodule
