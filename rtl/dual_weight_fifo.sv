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

    // Column 0: Combinational read (no skew, no latency)
    assign col0_out = queue0[rd_ptr0];

    // Column 1: Raw value (pre-skew) and skewed output
    assign col1_raw = queue1[rd_ptr1];

    // Column 2: Raw value (pre-skew)
    assign col2_raw = queue2[rd_ptr2];

    // Double-skew register for column 2 (2-cycle delay)
    logic [7:0] col2_skew_reg;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wr_ptr0 <= 2'd0;
            rd_ptr0 <= 2'd0;
            wr_ptr1 <= 2'd0;
            rd_ptr1 <= 2'd0;
            wr_ptr2 <= 2'd0;
            rd_ptr2 <= 2'd0;
            col1_out <= 8'd0;
            col2_skew_reg <= 8'd0;
            col2_out <= 8'd0;
            // Initialize queues
            for (int i = 0; i < 4; i++) begin
                queue0[i] <= 8'd0;
                queue1[i] <= 8'd0;
                queue2[i] <= 8'd0;
            end
        end else begin
            // Column 0: Push and Pop (no skew)
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
                // Output current queue value (1-cycle delayed)
                col1_out <= queue1[rd_ptr1];
                rd_ptr1 <= rd_ptr1 + 1'd1;
            end

            // Column 2: Push and Pop with 2-cycle skew (our extension)
            if (push_col2) begin
                queue2[wr_ptr2] <= data_in_col2;  // Use column 2 data
                wr_ptr2 <= wr_ptr2 + 1'd1;
            end
            if (pop) begin
                // First cycle: load raw value into skew register
                col2_skew_reg <= col2_raw;
                // Second cycle: output from skew register (2-cycle total delay)
                col2_out <= col2_skew_reg;
                rd_ptr2 <= rd_ptr2 + 1'd1;
            end
        end
    end

endmodule
