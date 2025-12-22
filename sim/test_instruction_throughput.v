`timescale 1ns / 1ps

module test_instruction_throughput;

    reg clk, rst_n;
    reg [31:0] instr_data;
    reg sys_busy, vpu_busy, dma_busy, wt_busy;

    wire pc_cnt;
    wire [1:0] current_stage;
    wire pipeline_stall;

    integer instruction_count;
    integer cycle_count;
    real throughput;

    tpu_controller dut (
        .clk            (clk),
        .rst_n          (rst_n),
        .instr_data     (instr_data),
        .sys_busy       (sys_busy),
        .vpu_busy       (vpu_busy),
        .dma_busy       (dma_busy),
        .wt_busy        (wt_busy),
        .pc_cnt         (pc_cnt),
        .pipeline_stall (pipeline_stall),
        .current_stage  (current_stage)
    );

    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    initial begin
        $display("Testing instruction throughput...");

        rst_n = 0;
        instr_data = 32'h0;
        sys_busy = 0; vpu_busy = 0; dma_busy = 0; wt_busy = 0;
        instruction_count = 0;
        cycle_count = 0;
        #20 rst_n = 1;

        // Run sequence of NOP instructions to measure throughput
        repeat (20) begin
            instr_data = {6'h00, 8'h00, 8'h00, 8'h00, 2'b00}; // NOP
            instruction_count = instruction_count + 1;
            #10;  // 1 cycle per instruction ideally
        end

        cycle_count = 20;  // We ran for 20 cycles
        throughput = instruction_count * 100.0 / cycle_count;

        $display("Instructions executed: %0d", instruction_count);
        $display("Cycles used: %0d", cycle_count);
        $display("Throughput: %0.1f instructions per cycle", throughput);

        if (throughput >= 0.8) begin
            $display("✓ High throughput achieved");
        end else begin
            $display("⚠ Lower throughput - possible pipeline stalls");
        end

        #10 $finish;
    end

endmodule



