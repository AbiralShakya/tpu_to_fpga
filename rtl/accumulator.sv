`timescale 1ns / 1ps

module accumulator (
    input  logic        clk,
    input  logic        rst_n,

    input  logic        acc_buf_sel,    // Buffer selection (0/1)
    input  logic        wr_en,          // Write enable
    input  logic [7:0]  wr_addr,        // Write address
    input  logic [63:0] wr_data,        // Write data (64 bits: col1 + col0)
    input  logic        rd_en,          // Read enable
    input  logic [7:0]  rd_addr,        // Read address
    output logic [63:0] rd_data         // Read data (64 bits: col1 + col0)
);

// =============================================================================
// SIMPLE ACCUMULATOR MEMORY (Dual-port, double-buffered)
// =============================================================================

// Two buffers of 256 entries each (8-bit address), 64-bit data each
logic [63:0] buffer0 [0:255];
logic [63:0] buffer1 [0:255];

// Write operation
always_ff @(posedge clk) begin
    if (wr_en) begin
        if (acc_buf_sel == 0)
            buffer0[wr_addr] <= wr_data;
        else
            buffer1[wr_addr] <= wr_data;
    end
end

// Read operation (combinational)
always_comb begin
    if (acc_buf_sel == 0)
        rd_data = buffer0[rd_addr];
    else
        rd_data = buffer1[rd_addr];
end

endmodule
