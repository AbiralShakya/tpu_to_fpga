`timescale 1ns / 1ps

module test_buffer_propagation;

    reg clk, rst_n;
    reg [31:0] instr_data;
    reg sys_busy, vpu_busy, dma_busy, wt_busy;

    // Monitor buffer selectors
    wire ub_buf_sel, acc_buf_sel, wt_buf_sel;
    wire [1:0] current_stage;

    tpu_controller dut (
        .clk            (clk),
        .rst_n          (rst_n),
        .instr_data     (instr_data),
        .sys_busy       (sys_busy),
        .vpu_busy       (vpu_busy),
        .dma_busy       (dma_busy),
        .wt_busy        (wt_busy),
        .ub_buf_sel     (ub_buf_sel),
        .acc_buf_sel    (acc_buf_sel),
        .wt_buf_sel     (wt_buf_sel),
        .current_stage  (current_stage)
    );

    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    initial begin
        $display("Testing buffer state propagation...");

        rst_n = 0;
        instr_data = 32'h0;
        sys_busy = 0; vpu_busy = 0; dma_busy = 0; wt_busy = 0;
        #20 rst_n = 1;

        // Test buffer state changes through SYNC
        $display("Initial buffer states: ub=%b, acc=%b, wt=%b", ub_buf_sel, acc_buf_sel, wt_buf_sel);

        // Send MATMUL (should use current buffer state)
        instr_data = {6'h10, 8'h00, 8'h20, 8'h04, 2'b00};
        #50;
        $display("After MATMUL: ub=%b, acc=%b, wt=%b", ub_buf_sel, acc_buf_sel, wt_buf_sel);

        // Send SYNC (should toggle all buffers)
        instr_data = {6'h30, 8'h03, 8'h00, 8'h01, 2'b00};
        #50;
        $display("After SYNC: ub=%b, acc=%b, wt=%b", ub_buf_sel, acc_buf_sel, wt_buf_sel);

        // Send another MATMUL (should use new buffer state)
        instr_data = {6'h10, 8'h40, 8'h60, 8'h04, 2'b00};
        #50;
        $display("After second MATMUL: ub=%b, acc=%b, wt=%b", ub_buf_sel, acc_buf_sel, wt_buf_sel);

        $display("Buffer state propagation test complete");
        #10 $finish;
    end

endmodule




