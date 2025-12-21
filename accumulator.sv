`timescale 1ns / 1ps

module accumulator #(
    parameter DATA_WIDTH = 32,
    parameter ARRAY_SIZE = 3,      // Updated for 3x3 array
    parameter BUFFER_DEPTH = 256  // 256 entries per buffer
)(
    input  logic                        clk,
    input  logic                        rst_n,

    // Control signals
    input  logic                        acc_buf_sel,    // Which buffer to use

    // Write interface (from systolic array)
    input  logic                        wr_en,
    input  logic [7:0]                  wr_addr,
    input  logic [DATA_WIDTH*ARRAY_SIZE-1:0] wr_data,  // 96-bit for 3x32-bit values

    // Read interface (to VPU)
    input  logic                        rd_en,
    input  logic [7:0]                  rd_addr,
    output logic [DATA_WIDTH*ARRAY_SIZE-1:0] rd_data,  // 96-bit for 3x32-bit values

    // Status
    output logic                        acc_busy
);

// =============================================================================
// DOUBLE-BUFFER MEMORY
// =============================================================================

// Two buffers: each holds 256 x 64-bit entries (32 KiB per buffer)
(* ram_style = "block" *) logic [DATA_WIDTH*ARRAY_SIZE-1:0] buffer0 [0:BUFFER_DEPTH-1];
(* ram_style = "block" *) logic [DATA_WIDTH*ARRAY_SIZE-1:0] buffer1 [0:BUFFER_DEPTH-1];

// =============================================================================
// WRITE LOGIC
// =============================================================================

always_ff @(posedge clk) begin
    if (wr_en) begin
        if (!acc_buf_sel) begin
            buffer0[wr_addr] <= wr_data;
        end else begin
            buffer1[wr_addr] <= wr_data;
        end
    end
end

// =============================================================================
// READ LOGIC
// =============================================================================

logic [DATA_WIDTH*ARRAY_SIZE-1:0] selected_rd_data;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        selected_rd_data <= {(DATA_WIDTH*ARRAY_SIZE){1'b0}};
    end else if (rd_en) begin
        if (!acc_buf_sel) begin
            selected_rd_data <= buffer0[rd_addr];
        end else begin
            selected_rd_data <= buffer1[rd_addr];
        end
    end
end

// =============================================================================
// OUTPUT ASSIGNMENTS
// =============================================================================

assign rd_data = selected_rd_data;
assign acc_busy = 1'b0;  // Simple implementation - always ready

endmodule
