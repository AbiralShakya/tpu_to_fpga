`timescale 1ns / 1ps

module test_single_instruction;

    // Testbench for individual instruction testing
    reg clk;
    reg rst_n;
    reg [31:0] instr_data;
    reg sys_busy, vpu_busy, dma_busy, wt_busy;

    // Controller outputs
    wire pc_cnt, ir_ld, sys_start, ub_rd_en, ub_wr_en, vpu_start, dma_start;
    wire [8:0] ub_rd_addr, ub_wr_addr;
    wire ub_buf_sel, acc_buf_sel, wt_buf_sel;
    wire pipeline_stall;
    wire [1:0] current_stage;

    // DUT
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

    // Clock
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // Status signals (not busy for instruction testing)
    initial begin
        sys_busy = 0;
        vpu_busy = 0;
        dma_busy = 0;
        wt_busy = 0;
    end

    // Test sequence
    initial begin
        $display("Testing single instruction...");

        // Reset
        rst_n = 0;
        instr_data = 32'h0;
        #20 rst_n = 1;

        // Test the specified instruction
        `ifdef TEST_INSTR
            case(`TEST_INSTR)
                6'h00: instr_data = {6'h00, 8'h00, 8'h00, 8'h00, 2'b00}; // NOP
                6'h10: instr_data = {6'h10, 8'h00, 8'h20, 8'h04, 2'b00}; // MATMUL
                6'h18: instr_data = {6'h18, 8'h20, 8'h40, 8'h04, 2'b00}; // RELU
                6'h30: instr_data = {6'h30, 8'h03, 8'h00, 8'h01, 2'b00}; // SYNC
                default: instr_data = {6'h00, 8'h00, 8'h00, 8'h00, 2'b00}; // NOP
            endcase
        `else
            instr_data = {6'h00, 8'h00, 8'h00, 8'h00, 2'b00}; // Default NOP
        `endif

        // Wait for execution
        #50;

        // Report results
        case(`TEST_INSTR)
            6'h00: $display("NOP: pc_cnt=%b, ir_ld=%b, sys_start=%b", pc_cnt, ir_ld, sys_start);
            6'h10: $display("MATMUL: sys_start=%b, ub_rd_en=%b, ub_rd_addr=%h", sys_start, ub_rd_en, ub_rd_addr);
            6'h18: $display("RELU: vpu_start=%b, ub_wr_en=%b, ub_wr_addr=%h", vpu_start, ub_wr_en, ub_wr_addr);
            6'h30: $display("SYNC: pipeline_stall=%b, ub_buf_sel=%b", pipeline_stall, ub_buf_sel);
            default: $display("Unknown instruction test");
        endcase

        #10 $finish;
    end

endmodule



