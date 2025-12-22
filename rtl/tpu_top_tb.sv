`timescale 1ns / 1ps

module tpu_top_tb;

// =============================================================================
// CLOCK AND RESET
// =============================================================================

logic clk = 0;
logic rst_n;

// Clock generation (100MHz)
always #5 clk = ~clk;

// =============================================================================
// DUT SIGNALS
// =============================================================================

// Instruction memory interface
logic [31:0] instr_data_in;
logic [7:0]  instr_addr_out;

// DMA interface
logic        dma_start_in = 0;
logic        dma_dir_in = 0;
logic [7:0]  dma_ub_addr_in = 0;
logic [15:0] dma_length_in = 0;
logic [1:0]  dma_elem_sz_in = 0;
logic [255:0] dma_data_in = 0;
logic        dma_busy_out;
logic        dma_done_out;
logic [255:0] dma_data_out;

// Status outputs
logic        tpu_busy;
logic        tpu_done;

// Debug outputs
logic [1:0]  pipeline_stage;
logic        hazard_detected;

// =============================================================================
// INSTRUCTION MEMORY MODEL
// =============================================================================

// Simple instruction memory (32 entries)
logic [31:0] instr_memory [0:31];

// Initialize instruction memory with a 2-layer MLP program (3x3 array)
initial begin
    // Program: Load 9 weight tiles -> MATMUL (3 rows) -> RELU -> SYNC -> Load 9 more tiles -> MATMUL (3 rows) -> RELU (3x3 array)
    instr_memory[0]  = {6'h02, 8'h00, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 0
    instr_memory[1]  = {6'h02, 8'h01, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 1
    instr_memory[2]  = {6'h02, 8'h02, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 2
    instr_memory[3]  = {6'h02, 8'h03, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 3
    instr_memory[4]  = {6'h02, 8'h04, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 4
    instr_memory[5]  = {6'h02, 8'h05, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 5
    instr_memory[6]  = {6'h02, 8'h06, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 6
    instr_memory[7]  = {6'h02, 8'h07, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 7
    instr_memory[8]  = {6'h02, 8'h08, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 8
    instr_memory[9]  = {6'h01, 8'h00, 8'h00, 8'h03, 2'b00}; // MATMUL (rows=3)
    instr_memory[10] = {6'h03, 8'h00, 8'h00, 8'h00, 2'b00}; // RELU
    instr_memory[11] = {6'h04, 8'h00, 8'h00, 8'h00, 2'b00}; // SYNC/SWAP
    instr_memory[12] = {6'h02, 8'h09, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 9
    instr_memory[13] = {6'h02, 8'h0A, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 10
    instr_memory[14] = {6'h02, 8'h0B, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 11
    instr_memory[15] = {6'h02, 8'h0C, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 12
    instr_memory[16] = {6'h02, 8'h0D, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 13
    instr_memory[17] = {6'h02, 8'h0E, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 14
    instr_memory[18] = {6'h02, 8'h0F, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 15
    instr_memory[19] = {6'h02, 8'h10, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 16
    instr_memory[20] = {6'h02, 8'h11, 8'h00, 8'h00, 2'b00}; // RD_WEIGHT tile 17
    instr_memory[21] = {6'h01, 8'h00, 8'h00, 8'h03, 2'b00}; // MATMUL (rows=3)
    instr_memory[22] = {6'h03, 8'h00, 8'h00, 8'h00, 2'b00}; // RELU

    // Fill rest with NOPs
    for (int i = 13; i < 32; i++) begin
        instr_memory[i] = 32'h00000000;
    end
end

// Instruction memory read
assign instr_data_in = instr_memory[instr_addr_out];

// =============================================================================
// DUT INSTANTIATION
// =============================================================================

tpu_top dut (
    .clk             (clk),
    .rst_n           (rst_n),
    .uart_rx         (1'b0),  // Tie UART RX low (no UART input in this test)
    .uart_tx         (),      // Leave UART TX unconnected
    .dma_start_in    (dma_start_in),
    .dma_dir_in      (dma_dir_in),
    .dma_ub_addr_in  (dma_ub_addr_in),
    .dma_length_in   (dma_length_in),
    .dma_elem_sz_in  (dma_elem_sz_in),
    .dma_data_in     (dma_data_in),
    .dma_busy_out    (dma_busy_out),
    .dma_done_out    (dma_done_out),
    .dma_data_out    (dma_data_out),
    .tpu_busy        (tpu_busy),
    .tpu_done        (tpu_done),
    .pipeline_stage  (pipeline_stage),
    .hazard_detected (hazard_detected),
    .uart_debug_state (),
    .uart_debug_cmd (),
    .uart_debug_byte_count ()
);

// =============================================================================
// TEST SEQUENCES
// =============================================================================

// Weight data for first layer (2x2 matrix)
logic [15:0] weights_layer1 [0:3] = {
    16'h0100,  // W[0][0] = 1
    16'h0200,  // W[0][1] = 2
    16'h0300,  // W[1][0] = 3
    16'h0400   // W[1][1] = 4
};

// Weight data for second layer (2x2 matrix)
logic [15:0] weights_layer2 [0:3] = {
    16'h0100,  // W[0][0] = 1
    16'h0100,  // W[0][1] = 1
    16'h0100,  // W[1][0] = 1
    16'h0100   // W[1][1] = 1
};

// Activation data (2x2 matrix)
logic [15:0] activations [0:3] = {
    16'h0500,  // A[0][0] = 5
    16'h0600,  // A[0][1] = 6
    16'h0700,  // A[1][0] = 7
    16'h0800   // A[1][1] = 8
};

// =============================================================================
// TEST STIMULUS
// =============================================================================

initial begin
    // Initialize
    rst_n = 0;
    dma_start_in = 0;
    dma_dir_in = 0;
    dma_ub_addr_in = 0;
    dma_length_in = 0;
    dma_elem_sz_in = 0;
    dma_data_in = 0;

    // Reset
    #20 rst_n = 1;

    $display("Starting TPU testbench...");
    $display("Time: %0t - Reset deasserted", $time);

    // Wait for pipeline to be ready
    #100;

    // Load weights for first layer via DMA
    $display("Time: %0t - Loading weights for layer 1", $time);
    for (int i = 0; i < 4; i++) begin
        dma_start_in = 1;
        dma_data_in = {240'b0, weights_layer1[i]};
        #10 dma_start_in = 0;
        #10;  // Wait for DMA
    end

    // Load activations via DMA
    $display("Time: %0t - Loading activations", $time);
    for (int i = 0; i < 4; i++) begin
        dma_start_in = 1;
        dma_data_in = {240'b0, activations[i]};
        #10 dma_start_in = 0;
        #10;
    end

    // Let the TPU run through the program
    $display("Time: %0t - Starting TPU program execution", $time);

    // Simple monitoring
    begin
        reg [9:0] cycle_count = 0;
        while (cycle_count < 1000) begin
            @(posedge clk);
            cycle_count = cycle_count + 1;

            if (pipeline_stage == 2'b01) begin
                $display("Time: %0t - Stage 1: Fetch/Decode", $time);
            end else if (pipeline_stage == 2'b10) begin
                $display("Time: %0t - Stage 2: Execute", $time);
            end

            if (hazard_detected) begin
                $display("Time: %0t - HAZARD DETECTED: Pipeline stalled", $time);
            end

            if (tpu_done && !tpu_busy) begin
                $display("Time: %0t - TPU computation completed", $time);
                cycle_count = 1000;  // Exit loop
            end
        end
    end

    // Run for sufficient time to complete the program
    #2000;

    // Check results
    $display("Time: %0t - Checking results...", $time);

    // Expected result after layer 1 + ReLU: [[23, 34], [31, 46]]
    // After layer 2 + ReLU: [[57, 57], [77, 77]]

    // Read results from DMA
    dma_dir_in = 1;  // TPU to host
    #10;
    $display("Time: %0t - Final result: %h", $time, dma_data_out);

    $display("Testbench completed");
    $finish;
end

// =============================================================================
// MONITORING AND DEBUGGING
// =============================================================================

// Monitor key signals
initial begin
    $monitor("Time: %0t | PC: %h | Stage: %b | Hazard: %b | Busy: %b | Done: %b",
             $time, instr_addr_out, pipeline_stage, hazard_detected, tpu_busy, tpu_done);
end

// Note: Verilator doesn't support simulation time delays
// The test will run until completion or manual termination

endmodule
