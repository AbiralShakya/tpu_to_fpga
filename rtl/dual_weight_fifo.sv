// =============================================================================
// Dual Weight FIFO (tinytinyTPU compatible, extended for 3 columns)
// Supports diagonal wavefront weight loading for 3x3 systolic array
// =============================================================================

`timescale 1ns / 1ps

module dual_weight_fifo (
    input  logic       clk,
    input  logic       rst_n,
    // Push interface (separate data for each column)
    input  logic       push_col0,
    input  logic       push_col1,
    input  logic       push_col2,
    input  logic [7:0] data_in_col0,   // Weight for column 0 PEs
    input  logic [7:0] data_in_col1,   // Weight for column 1 PEs
    input  logic [7:0] data_in_col2,   // Weight for column 2 PEs
    // Pop interface (MMU loads weights with column staggering)
    input  logic       pop,
    input  logic       weight_load_start,  // Asserted when new weight loading sequence begins
    output logic [7:0] col0_out,       // Direct output (combinational, no skew)
    output logic [7:0] col1_out,       // Skewed output (1-cycle delay for wavefront)
    output logic [7:0] col2_out,       // Double-skewed output (2-cycle delay)
    // Raw outputs for debugging
    output logic [7:0] col1_raw,
    output logic [7:0] col2_raw
);

    // Three 4-deep queues for 3x3 array
    logic [7:0] queue0 [4];
    logic [7:0] queue1 [4];
    logic [7:0] queue2 [4];

    // Pointers for read/write
    logic [1:0] wr_ptr0, rd_ptr0;
    logic [1:0] wr_ptr1, rd_ptr1;
    logic [1:0] wr_ptr2, rd_ptr2;

    // Column 0: Direct output (no delay - first to arrive)
    assign col0_out = queue0[rd_ptr0];

    // Column 1: Raw value (pre-skew) for debugging
    assign col1_raw = queue1[rd_ptr1];

    // Column 2: Raw value (pre-skew) for debugging
    assign col2_raw = queue2[rd_ptr2];

    // PURE SHIFT REGISTER delay pipelines
    // Column 1: 1-cycle delay (single stage)
    // Column 2: 2-cycle delay (two stages)
    logic [7:0] col1_delay_stage;  // 1-cycle delay for column 1
    logic [7:0] col2_delay_stage1; // First delay stage for column 2
    logic [7:0] col2_delay_stage2; // Second delay stage for column 2

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wr_ptr0 <= 2'd0;
            rd_ptr0 <= 2'd0;
            wr_ptr1 <= 2'd0;
            rd_ptr1 <= 2'd0;
            wr_ptr2 <= 2'd0;
            rd_ptr2 <= 2'd0;
            col1_delay_stage <= 8'd0;
            col1_out <= 8'd0;
            col2_delay_stage1 <= 8'd0;
            col2_delay_stage2 <= 8'd0;
            col2_out <= 8'd0;
            // Initialize queues
            for (int i = 0; i < 4; i++) begin
                queue0[i] <= 8'd0;
                queue1[i] <= 8'd0;
                queue2[i] <= 8'd0;
            end
        end else begin
            // CRITICAL: Reset ALL state when new weight loading sequence starts
            // This ensures clean state between successive MATMUL operations
            //
            // BUG FIX: The FIFO is only 4 entries deep, but we pop 7 times during
            // weight loading to flush the skew pipeline. This causes rd_ptr to wrap
            // and desync from wr_ptr. Without resetting pointers, subsequent tests
            // read garbage from wrapped/uninitialized queue positions.
            if (weight_load_start) begin
                // Reset ALL read/write pointers to resync the FIFO
                wr_ptr0 <= 2'd0; rd_ptr0 <= 2'd0;
                wr_ptr1 <= 2'd0; rd_ptr1 <= 2'd0;
                wr_ptr2 <= 2'd0; rd_ptr2 <= 2'd0;
                // Clear delay stages so first pops output 0 (correct timing)
                col1_delay_stage <= 8'd0;
                col1_out <= 8'd0;
                col2_delay_stage1 <= 8'd0;
                col2_delay_stage2 <= 8'd0;
                col2_out <= 8'd0;
            end

            // Column 0: Push and Pop (no delay - direct from queue)
            if (push_col0) begin
                queue0[wr_ptr0] <= data_in_col0;
                wr_ptr0 <= wr_ptr0 + 1'd1;
            end
            if (pop) begin
                rd_ptr0 <= rd_ptr0 + 1'd1;
            end

            // Column 1: Push and Pop with PURE 1-cycle delay (shift register)
            // Output ALWAYS comes from delay stage, never directly from queue
            // This ensures proper diagonal wavefront timing
            if (push_col1) begin
                queue1[wr_ptr1] <= data_in_col1;
                wr_ptr1 <= wr_ptr1 + 1'd1;
            end
            if (pop) begin
                // Pure shift register: output from delay stage, load new value
                col1_out <= col1_delay_stage;          // Output value from 1 cycle ago
                col1_delay_stage <= queue1[rd_ptr1];   // Load current queue value into delay
                rd_ptr1 <= rd_ptr1 + 1'd1;
            end

            // Column 2: Push and Pop with PURE 2-cycle delay (shift register)
            // Output ALWAYS comes from stage2, never directly from queue
            // This ensures proper diagonal wavefront timing
            if (push_col2) begin
                queue2[wr_ptr2] <= data_in_col2;
                wr_ptr2 <= wr_ptr2 + 1'd1;
            end
            if (pop) begin
                // Pure shift register: shift through both stages
                col2_out <= col2_delay_stage2;          // Output value from 2 cycles ago
                col2_delay_stage2 <= col2_delay_stage1; // Shift stage 1 -> stage 2
                col2_delay_stage1 <= queue2[rd_ptr2];   // Load current queue value into stage 1
                rd_ptr2 <= rd_ptr2 + 1'd1;
            end
        end
    end

endmodule
