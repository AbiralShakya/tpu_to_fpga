// =============================================================================
// Accumulator Alignment Module (tinytinyTPU compatible)
// Aligns staggered column outputs from systolic array
// =============================================================================

`timescale 1ns / 1ps

module accumulator_align (
    input  logic        clk,
    input  logic        rst_n,

    input  logic        valid_in,
    input  logic [31:0] raw_col0,    // Raw column 0 data (32-bit for our 3x3 array)
    input  logic [31:0] raw_col1,    // Raw column 1 data
    input  logic [31:0] raw_col2,    // Raw column 2 data (our extension)

    output logic        aligned_valid,
    output logic [31:0] align_col0,
    output logic [31:0] align_col1,
    output logic [31:0] align_col2
);

    // For our 3x3 systolic array, we need to align the columns
    // Column 0 arrives first, then column 1 (1 cycle delay), then column 2 (2 cycle delay)
    logic [31:0] col0_delay_reg, col1_delay_reg;
    logic [1:0]  alignment_state;  // Track alignment progress

    localparam [1:0] WAIT_COL0 = 2'b00;
    localparam [1:0] WAIT_COL1 = 2'b01;
    localparam [1:0] WAIT_COL2 = 2'b10;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            col0_delay_reg <= 32'd0;
            col1_delay_reg <= 32'd0;
            alignment_state <= WAIT_COL0;
            aligned_valid <= 1'b0;
            align_col0 <= 32'd0;
            align_col1 <= 32'd0;
            align_col2 <= 32'd0;
        end
        else begin
            aligned_valid <= 1'b0;

            if (valid_in) begin
                case (alignment_state)
                    WAIT_COL0: begin
                        col0_delay_reg <= raw_col0;
                        alignment_state <= WAIT_COL1;
                    end

                    WAIT_COL1: begin
                        col1_delay_reg <= raw_col1;
                        alignment_state <= WAIT_COL2;
                    end

                    WAIT_COL2: begin
                        // All columns aligned - output and reset
                        aligned_valid <= 1'b1;
                        align_col0 <= col0_delay_reg;
                        align_col1 <= col1_delay_reg;
                        align_col2 <= raw_col2;
                        alignment_state <= WAIT_COL0;
                    end
                endcase
            end
            else begin
                // No valid input - reset alignment state
                alignment_state <= WAIT_COL0;
            end
        end
    end

endmodule
