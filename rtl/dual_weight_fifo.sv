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

    // Column 1: Raw value (pre-skew) and skewed output
    assign col1_raw = queue1[rd_ptr1];

    // Column 2: Raw value (pre-skew)
    assign col2_raw = queue2[rd_ptr2];

    // CRITICAL FIX: Column 1 and 2 need their own delay pipeline, NOT using FIFO read pointer
    // The issue: rd_ptr advances every cycle, but we need values to flow through delay stages
    logic [7:0] col1_delay_stage;  // 1-cycle delay for column 1
    logic [7:0] col2_delay_stage1; // First delay stage for column 2
    logic [7:0] col2_delay_stage2; // Second delay stage for column 2
    
    // Track pop count to handle initial delay correctly
    logic col1_pop_done;  // True after first pop for column 1
    logic [1:0] col2_pop_count;  // Count pops for column 2 (0, 1, 2+)

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
            col1_pop_done <= 1'b0;
            col2_pop_count <= 2'd0;
            // Initialize queues
            for (int i = 0; i < 4; i++) begin
                queue0[i] <= 8'd0;
                queue1[i] <= 8'd0;
                queue2[i] <= 8'd0;
            end
        end else begin
            // Reset pop counters when new weight loading sequence starts
            if (weight_load_start) begin
                col1_pop_done <= 1'b0;
                col2_pop_count <= 2'd0;
            end
            
            // Column 0: Push and Pop (no delay)
            if (push_col0) begin
                queue0[wr_ptr0] <= data_in_col0;  // Use column 0 data
                wr_ptr0 <= wr_ptr0 + 1'd1;
            end
            if (pop) begin
                rd_ptr0 <= rd_ptr0 + 1'd1;
            end

            // Column 1: Push and Pop with 1-cycle skew
            if (push_col1) begin
                queue1[wr_ptr1] <= data_in_col1;  // Use column 1 data
                wr_ptr1 <= wr_ptr1 + 1'd1;
            end
            if (pop) begin
                // CRITICAL FIX: First pop outputs immediately, subsequent pops use delay
                if (!col1_pop_done) begin
                    // First pop: output immediately, initialize delay stage
                    col1_out <= queue1[rd_ptr1];
                    col1_delay_stage <= queue1[rd_ptr1];
                    col1_pop_done <= 1'b1;
                end else begin
                    // Subsequent pops: use 1-cycle delay pipeline
                    col1_out <= col1_delay_stage;          // Output previous value
                    col1_delay_stage <= queue1[rd_ptr1];  // Load new value
                end
                rd_ptr1 <= rd_ptr1 + 1'd1;
            end

            // Column 2: Push and Pop with 2-cycle skew (our extension)
            if (push_col2) begin
                queue2[wr_ptr2] <= data_in_col2;  // Use column 2 data
                wr_ptr2 <= wr_ptr2 + 1'd1;
            end
            if (pop) begin
                // CRITICAL FIX: Handle 2-cycle delay with proper initialization
                if (col2_pop_count == 2'd0) begin
                    // First pop: output immediately, initialize first delay stage
                    col2_out <= queue2[rd_ptr2];
                    col2_delay_stage1 <= queue2[rd_ptr2];
                    col2_pop_count <= 2'd1;
                end else if (col2_pop_count == 2'd1) begin
                    // Second pop: output from first delay stage, initialize second
                    col2_out <= col2_delay_stage1;
                    col2_delay_stage2 <= col2_delay_stage1;
                    col2_delay_stage1 <= queue2[rd_ptr2];
                    col2_pop_count <= 2'd2;
                end else begin
                    // Subsequent pops: use full 2-cycle delay pipeline
                    col2_out <= col2_delay_stage2;          // Output from 2 cycles ago
                    col2_delay_stage2 <= col2_delay_stage1; // Shift stage2
                    col2_delay_stage1 <= queue2[rd_ptr2];   // Shift stage1
                end
                rd_ptr2 <= rd_ptr2 + 1'd1;
            end
        end
    end

endmodule
