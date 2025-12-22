// =============================================================================
// Accumulator Memory Module (tinytinyTPU compatible, extended for 3x3)
// Double-buffered accumulator storage with accumulate/overwrite modes
// =============================================================================

`timescale 1ns / 1ps

module accumulator_mem #(
    parameter bit BYPASS_READ_NEW = 1'b1  // Output newly written value same cycle
) (
    input  logic        clk,
    input  logic        rst_n,

    input  logic        enable,           // Valid data input
    input  logic        accumulator_mode, // 0: overwrite, 1: accumulate
    input  logic        buffer_select,    // Double buffer selection (0/1)

    input  logic [31:0] in_col0,          // Column 0 input
    input  logic [31:0] in_col1,          // Column 1 input
    input  logic [31:0] in_col2,          // Column 2 input (our extension)

    output logic        valid_out,
    output logic signed [31:0] out_col0,
    output logic signed [31:0] out_col1,
    output logic signed [31:0] out_col2
);

    // Double buffer memory: 2 buffers × 3 columns × 32 bits
    logic signed [31:0] mem_buff0_col0, mem_buff0_col1, mem_buff0_col2;
    logic signed [31:0] mem_buff1_col0, mem_buff1_col1, mem_buff1_col2;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            valid_out <= 1'b0;
            out_col0 <= 32'sd0;
            out_col1 <= 32'sd0;
            out_col2 <= 32'sd0;

            // Initialize memory
            mem_buff0_col0 <= 32'sd0;
            mem_buff0_col1 <= 32'sd0;
            mem_buff0_col2 <= 32'sd0;
            mem_buff1_col0 <= 32'sd0;
            mem_buff1_col1 <= 32'sd0;
            mem_buff1_col2 <= 32'sd0;
        end
        else begin
            if (enable) begin
                unique case (buffer_select)
                    1'b0: begin // --- BUFFER 0 ---
                        if (accumulator_mode) begin
                            // Accumulate mode: add to existing values
                            mem_buff0_col0 <= mem_buff0_col0 + $signed(in_col0);
                            mem_buff0_col1 <= mem_buff0_col1 + $signed(in_col1);
                            mem_buff0_col2 <= mem_buff0_col2 + $signed(in_col2);
                        end else begin
                            // Overwrite mode: replace values
                            mem_buff0_col0 <= $signed(in_col0);
                            mem_buff0_col1 <= $signed(in_col1);
                            mem_buff0_col2 <= $signed(in_col2);
                        end

                        // Output logic
                        if (BYPASS_READ_NEW) begin
                            // Output newly computed values
                            out_col0 <= accumulator_mode ?
                                (mem_buff0_col0 + $signed(in_col0)) : $signed(in_col0);
                            out_col1 <= accumulator_mode ?
                                (mem_buff0_col1 + $signed(in_col1)) : $signed(in_col1);
                            out_col2 <= accumulator_mode ?
                                (mem_buff0_col2 + $signed(in_col2)) : $signed(in_col2);
                        end else begin
                            // Output stored values
                            out_col0 <= mem_buff0_col0;
                            out_col1 <= mem_buff0_col1;
                            out_col2 <= mem_buff0_col2;
                        end
                    end

                    1'b1: begin // --- BUFFER 1 ---
                        if (accumulator_mode) begin
                            // Accumulate mode: add to existing values
                            mem_buff1_col0 <= mem_buff1_col0 + $signed(in_col0);
                            mem_buff1_col1 <= mem_buff1_col1 + $signed(in_col1);
                            mem_buff1_col2 <= mem_buff1_col2 + $signed(in_col2);
                        end else begin
                            // Overwrite mode: replace values
                            mem_buff1_col0 <= $signed(in_col0);
                            mem_buff1_col1 <= $signed(in_col1);
                            mem_buff1_col2 <= $signed(in_col2);
                        end

                        // Output logic
                        if (BYPASS_READ_NEW) begin
                            // Output newly computed values
                            out_col0 <= accumulator_mode ?
                                (mem_buff1_col0 + $signed(in_col0)) : $signed(in_col0);
                            out_col1 <= accumulator_mode ?
                                (mem_buff1_col1 + $signed(in_col1)) : $signed(in_col1);
                            out_col2 <= accumulator_mode ?
                                (mem_buff1_col2 + $signed(in_col2)) : $signed(in_col2);
                        end else begin
                            // Output stored values
                            out_col0 <= mem_buff1_col0;
                            out_col1 <= mem_buff1_col1;
                            out_col2 <= mem_buff1_col2;
                        end
                    end
                endcase

                valid_out <= 1'b1;
            end
            else begin
                valid_out <= 1'b0;
            end
        end
    end

endmodule
