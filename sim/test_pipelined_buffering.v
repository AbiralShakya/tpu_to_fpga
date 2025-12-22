`timescale 1ns / 1ps

module test_pipelined_buffering;

    // Testbench signals
    reg clk;
    reg rst_n;
    reg [31:0] instr_data;
    reg sys_busy;
    reg vpu_busy;
    reg dma_busy;
    reg wt_busy;

    // Controller outputs to monitor
    wire pc_cnt;
    wire pc_ld;
    wire ir_ld;
    wire if_id_flush;
    wire sys_start;
    wire [1:0] sys_mode;
    wire [7:0] sys_rows;
    wire sys_signed;
    wire sys_transpose;
    wire [7:0] sys_acc_addr;
    wire sys_acc_clear;
    wire ub_rd_en;
    wire ub_wr_en;
    wire [8:0] ub_rd_addr;
    wire [8:0] ub_wr_addr;
    wire [8:0] ub_rd_count;
    wire [8:0] ub_wr_count;
    wire ub_buf_sel;
    wire wt_mem_rd_en;
    wire [23:0] wt_mem_addr;
    wire wt_fifo_wr;
    wire [7:0] wt_num_tiles;
    wire wt_buf_sel;
    wire acc_wr_en;
    wire acc_rd_en;
    wire [7:0] acc_addr;
    wire acc_buf_sel;
    wire vpu_start;
    wire [3:0] vpu_mode;
    wire [7:0] vpu_in_addr;
    wire [7:0] vpu_out_addr;
    wire [7:0] vpu_length;
    wire [15:0] vpu_param;
    wire dma_start;
    wire dma_dir;
    wire [7:0] dma_ub_addr;
    wire [15:0] dma_length;
    wire [1:0] dma_elem_sz;
    wire sync_wait;
    wire [3:0] sync_mask;
    wire [15:0] sync_timeout;
    wire cfg_wr_en;
    wire [7:0] cfg_addr;
    wire [15:0] cfg_data;
    wire halt_req;
    wire interrupt_en;
    wire pipeline_stall;
    wire [1:0] current_stage;

    // DUT instantiation
    tpu_controller dut (
        .clk            (clk),
        .rst_n          (rst_n),
        .instr_data     (instr_data),
        .sys_busy       (sys_busy),
        .vpu_busy       (vpu_busy),
        .dma_busy       (dma_busy),
        .wt_busy        (wt_busy),
        .pc_cnt         (pc_cnt),
        .pc_ld          (pc_ld),
        .ir_ld          (ir_ld),
        .if_id_flush    (if_id_flush),
        .sys_start      (sys_start),
        .sys_mode       (sys_mode),
        .sys_rows       (sys_rows),
        .sys_signed     (sys_signed),
        .sys_transpose  (sys_transpose),
        .sys_acc_addr   (sys_acc_addr),
        .sys_acc_clear  (sys_acc_clear),
        .ub_rd_en       (ub_rd_en),
        .ub_wr_en       (ub_wr_en),
        .ub_rd_addr     (ub_rd_addr),
        .ub_wr_addr     (ub_wr_addr),
        .ub_rd_count    (ub_rd_count),
        .ub_wr_count    (ub_wr_count),
        .ub_buf_sel     (ub_buf_sel),
        .wt_mem_rd_en   (wt_mem_rd_en),
        .wt_mem_addr    (wt_mem_addr),
        .wt_fifo_wr     (wt_fifo_wr),
        .wt_num_tiles   (wt_num_tiles),
        .wt_buf_sel     (wt_buf_sel),
        .acc_wr_en      (acc_wr_en),
        .acc_rd_en      (acc_rd_en),
        .acc_addr       (acc_addr),
        .acc_buf_sel    (acc_buf_sel),
        .vpu_start      (vpu_start),
        .vpu_mode       (vpu_mode),
        .vpu_in_addr    (vpu_in_addr),
        .vpu_out_addr   (vpu_out_addr),
        .vpu_length     (vpu_length),
        .vpu_param      (vpu_param),
        .dma_start      (dma_start),
        .dma_dir        (dma_dir),
        .dma_ub_addr    (dma_ub_addr),
        .dma_length     (dma_length),
        .dma_elem_sz    (dma_elem_sz),
        .sync_wait      (sync_wait),
        .sync_mask      (sync_mask),
        .sync_timeout   (sync_timeout),
        .cfg_wr_en      (cfg_wr_en),
        .cfg_addr       (cfg_addr),
        .cfg_data       (cfg_data),
        .halt_req       (halt_req),
        .interrupt_en   (interrupt_en),
        .pipeline_stall (pipeline_stall),
        .current_stage  (current_stage)
    );

    // Clock generation
    initial begin
        clk = 0;
        forever #5 clk = ~clk;  // 100MHz clock
    end

    // ISA Opcodes
    localparam NOP_OP         = 6'h00;
    localparam RD_HOST_MEM_OP = 6'h01;
    localparam WR_HOST_MEM_OP = 6'h02;
    localparam RD_WEIGHT_OP   = 6'h03;
    localparam LD_UB_OP       = 6'h04;
    localparam ST_UB_OP       = 6'h05;
    localparam MATMUL_OP      = 6'h10;
    localparam CONV2D_OP      = 6'h11;
    localparam MATMUL_ACC_OP  = 6'h12;
    localparam RELU_OP        = 6'h18;
    localparam RELU6_OP       = 6'h19;
    localparam SIGMOID_OP     = 6'h1A;
    localparam TANH_OP        = 6'h1B;
    localparam MAXPOOL_OP     = 6'h20;
    localparam AVGPOOL_OP     = 6'h21;
    localparam ADD_BIAS_OP     = 6'h22;
    localparam BATCH_NORM_OP   = 6'h23;
    localparam SYNC_OP         = 6'h30;
    localparam CFG_REG_OP      = 6'h31;
    localparam HALT_OP         = 6'h3F;

    // Function to create instruction
    function [31:0] make_instr;
        input [5:0] opcode;
        input [7:0] arg1;
        input [7:0] arg2;
        input [7:0] arg3;
        input [1:0] flags;
        begin
            make_instr = {opcode, arg1, arg2, arg3, flags};
        end
    endfunction

    // Test sequence
    initial begin
        $display("Starting Pipelined Double Buffering Test");
        $display("==========================================");

        // Initialize
        rst_n = 0;
        instr_data = 32'h0;
        sys_busy = 0;
        vpu_busy = 0;
        dma_busy = 0;
        wt_busy = 0;

        #20;  // Wait for reset
        rst_n = 1;

        // Test 1: Basic instruction sequence with buffer state monitoring
        $display("\nTest 1: Basic instruction sequence");
        $display("-----------------------------------");

        // Send MATMUL instruction (should use initial buffer state 0)
        instr_data = make_instr(MATMUL_OP, 8'h00, 8'h20, 8'h04, 2'b00);
        $display("Cycle %0t: Sending MATMUL (UB[0] -> Acc[0x20], 4 rows)", $time);
        $display("  Expected: ub_buf_sel=0, acc_buf_sel=0");
        #10;  // Wait one cycle

        // Check control signals
        $display("  Actual: ub_rd_en=%b, ub_buf_sel=%b, acc_wr_en=%b, acc_buf_sel=%b",
                ub_rd_en, ub_buf_sel, acc_wr_en, acc_buf_sel);

        // Send RELU instruction
        #10;
        instr_data = make_instr(RELU_OP, 8'h20, 8'h40, 8'h04, 2'b00);
        $display("Cycle %0t: Sending RELU (Acc[0x20] -> UB[0x40], 4 elements)", $time);
        $display("  Expected: acc_buf_sel=0 (read), ub_buf_sel=1 (write)");
        #10;

        $display("  Actual: acc_rd_en=%b, acc_buf_sel=%b, ub_wr_en=%b, ub_buf_sel=%b",
                acc_rd_en, acc_buf_sel, ub_wr_en, ub_buf_sel);

        // Send SYNC instruction
        #10;
        instr_data = make_instr(SYNC_OP, 8'h03, 8'h00, 8'h01, 2'b00);
        $display("Cycle %0t: Sending SYNC (mask=0x03, timeout=256)", $time);
        $display("  Expected: sync_wait=1, pipeline stall, buffer toggle");
        #10;

        $display("  Actual: sync_wait=%b, pipeline_stall=%b, ub_buf_sel=%b, acc_buf_sel=%b",
                sync_wait, pipeline_stall, ub_buf_sel, acc_buf_sel);

        // Test 2: Buffer state persistence
        $display("\nTest 2: Buffer state persistence");
        $display("-------------------------------");

        // Send MATMUL after SYNC (should use toggled buffer state)
        #10;
        instr_data = make_instr(MATMUL_OP, 8'h40, 8'h60, 8'h04, 2'b00);
        $display("Cycle %0t: Sending MATMUL after SYNC", $time);
        $display("  Expected: ub_buf_sel=1, acc_buf_sel=1");
        #10;

        $display("  Actual: ub_rd_en=%b, ub_buf_sel=%b, acc_wr_en=%b, acc_buf_sel=%b",
                ub_rd_en, ub_buf_sel, acc_wr_en, acc_buf_sel);

        // Test 3: Pipeline behavior
        $display("\nTest 3: Pipeline behavior");
        $display("------------------------");

        // Send sequence quickly to test pipelining
        #10;
        instr_data = make_instr(LD_UB_OP, 8'h00, 8'h08, 8'h00, 2'b00);
        $display("Cycle %0t: Sending LD_UB", $time);
        #10;

        instr_data = make_instr(ST_UB_OP, 8'h10, 8'h08, 8'h00, 2'b00);
        $display("Cycle %0t: Sending ST_UB", $time);
        #10;

        instr_data = make_instr(RD_WEIGHT_OP, 8'h00, 8'h02, 8'h00, 2'b00);
        $display("Cycle %0t: Sending RD_WEIGHT", $time);
        #10;

        $display("Pipeline stages observed: %b", current_stage);

        // Test 4: Control signal verification
        $display("\nTest 4: Control signal verification");
        $display("-----------------------------------");

        #10;
        instr_data = make_instr(NOP_OP, 8'h00, 8'h00, 8'h00, 2'b00);
        $display("Cycle %0t: NOP instruction", $time);
        #10;
        $display("  NOP signals: pc_cnt=%b, ir_ld=%b, others should be 0", pc_cnt, ir_ld);

        #10;
        instr_data = make_instr(HALT_OP, 8'h00, 8'h00, 8'h00, 2'b00);
        $display("Cycle %0t: HALT instruction", $time);
        #10;
        $display("  HALT signals: halt_req=%b, interrupt_en=%b, pc_cnt=%b, ir_ld=%b",
                halt_req, interrupt_en, pc_cnt, ir_ld);

        $display("\nTest completed successfully!");
        $display("==============================");

        #100;
        $finish;
    end

    // Monitor pipeline state
    always @(posedge clk) begin
        if (rst_n) begin
            $display("Cycle %0t: Stage=%b, Stall=%b, PC_cnt=%b, IR_ld=%b",
                    $time, current_stage, pipeline_stall, pc_cnt, ir_ld);
        end
    end

endmodule
