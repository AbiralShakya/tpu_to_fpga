`timescale 1ns / 1ps

module test_matrix_workload;

    reg clk, rst_n;
    reg [31:0] instr_data;
    reg sys_busy, vpu_busy, dma_busy, wt_busy;

    wire sys_start, vpu_start;
    wire [7:0] sys_rows;
    wire [1:0] current_stage;

    tpu_controller controller (
        .clk            (clk),
        .rst_n          (rst_n),
        .instr_data     (instr_data),
        .sys_busy       (sys_busy),
        .vpu_busy       (vpu_busy),
        .dma_busy       (dma_busy),
        .wt_busy        (wt_busy),
        .sys_start      (sys_start),
        .sys_rows       (sys_rows),
        .vpu_start      (vpu_start),
        .current_stage  (current_stage)
    );

    // Simplified datapath simulation
    reg [7:0] cycle_count;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sys_busy <= 0;
            vpu_busy <= 0;
            dma_busy <= 0;
            wt_busy <= 0;
            cycle_count <= 0;
        end else begin
            // Simulate operation completion
            if (sys_start) sys_busy <= 1;
            if (vpu_start) vpu_busy <= 1;

            if (sys_busy) cycle_count <= cycle_count + 1;
            if (cycle_count >= sys_rows + 2) begin  // Complete after computation
                sys_busy <= 0;
                cycle_count <= 0;
            end

            if (vpu_busy && cycle_count >= 8) begin  // VPU takes 8 cycles
                vpu_busy <= 0;
                cycle_count <= 0;
            end
        end
    end

    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    initial begin
        $display("Testing matrix multiply workload...");

        rst_n = 0;
        instr_data = 32'h0;
        sys_busy = 0; vpu_busy = 0; dma_busy = 0; wt_busy = 0;
        #20 rst_n = 1;

        // Load weights
        instr_data = {6'h03, 8'h00, 8'h02, 8'h00, 2'b00}; // RD_WEIGHT
        #40;

        // Load input data
        instr_data = {6'h01, 8'h00, 8'h02, 8'h00, 2'b00}; // RD_HOST_MEM
        #40;

        // Perform matrix multiplication (4x4 matrix)
        instr_data = {6'h10, 8'h00, 8'h20, 8'h04, 2'b00}; // MATMUL
        $display("Starting 4x4 matrix multiply...");
        #40;

        // Wait for completion
        wait(!sys_busy);
        $display("Matrix multiply complete after %0d cycles", cycle_count);

        // Apply ReLU activation
        instr_data = {6'h18, 8'h20, 8'h40, 8'h10, 2'b00}; // RELU
        $display("Applying ReLU activation...");
        #40;

        // Wait for completion
        wait(!vpu_busy);
        $display("ReLU activation complete");

        // Send result back
        instr_data = {6'h02, 8'h40, 8'h02, 8'h00, 2'b00}; // WR_HOST_MEM
        #40;

        $display("Matrix multiply workload complete");
        #10 $finish;
    end

endmodule




