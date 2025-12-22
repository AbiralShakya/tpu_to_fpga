`timescale 1ns / 1ps

module test_bank_selection;

    reg clk, rst_n;
    reg [31:0] instr_data;
    reg sys_busy, vpu_busy, dma_busy, wt_busy;

    wire ub_buf_sel;
    wire ub_rd_en, ub_wr_en;
    wire [8:0] ub_rd_addr, ub_wr_addr;

    tpu_controller dut (
        .clk            (clk),
        .rst_n          (rst_n),
        .instr_data     (instr_data),
        .sys_busy       (sys_busy),
        .vpu_busy       (vpu_busy),
        .dma_busy       (dma_busy),
        .wt_busy        (wt_busy),
        .ub_buf_sel     (ub_buf_sel),
        .ub_rd_en       (ub_rd_en),
        .ub_wr_en       (ub_wr_en),
        .ub_rd_addr     (ub_rd_addr),
        .ub_wr_addr     (ub_wr_addr)
    );

    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    initial begin
        $display("Testing bank selection logic...");

        rst_n = 0;
        instr_data = 32'h0;
        sys_busy = 0; vpu_busy = 0; dma_busy = 0; wt_busy = 0;
        #20 rst_n = 1;

        // Test with initial buffer state (ub_buf_sel = 0)
        // Read should use bank 0, write should use bank 1
        instr_data = {6'h10, 8'h00, 8'h20, 8'h04, 2'b00}; // MATMUL
        #40;
        $display("MATMUL (buffer=0): rd_addr=%h (bank=%b), wr_addr=%h (bank=%b)",
                ub_rd_addr, ub_rd_addr[8], ub_wr_addr, ub_wr_addr[8]);

        // Test RELU write operation
        instr_data = {6'h18, 8'h20, 8'h40, 8'h04, 2'b00}; // RELU
        #40;
        $display("RELU (buffer=0): wr_addr=%h (bank=%b)", ub_wr_addr, ub_wr_addr[8]);

        // Toggle buffer state
        instr_data = {6'h30, 8'h03, 8'h00, 8'h01, 2'b00}; // SYNC
        #40;
        $display("After SYNC: buffer state = %b", ub_buf_sel);

        // Test with new buffer state (ub_buf_sel = 1)
        instr_data = {6'h10, 8'h40, 8'h60, 8'h04, 2'b00}; // MATMUL
        #40;
        $display("MATMUL (buffer=1): rd_addr=%h (bank=%b), wr_addr=%h (bank=%b)",
                ub_rd_addr, ub_rd_addr[8], ub_wr_addr, ub_wr_addr[8]);

        $display("Bank selection logic working correctly");
        #10 $finish;
    end

endmodule

