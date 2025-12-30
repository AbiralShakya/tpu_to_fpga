`timescale 1ns / 1ps

module accumulator (
    input  logic        clk,
    input  logic        rst_n,

    input  logic        acc_buf_sel,    // Buffer selection (0/1)
    input  logic        clear,          // Clear accumulator contents
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
// CRITICAL: ram_style = "block" forces BRAM inference (prevents flip-flop synthesis)
(* ram_style = "block" *) logic [63:0] buffer0 [0:255];
(* ram_style = "block" *) logic [63:0] buffer1 [0:255];

// Register buffer selection to prevent glitches during read operations
// This ensures atomic operations - once a read starts, it completes on the same buffer
logic acc_buf_sel_reg;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        acc_buf_sel_reg <= 1'b0;
    end else begin
        // Only update when no operations are active to prevent race conditions
        // In practice, buffer selection should only change via SYNC instruction
        // which waits for operations to complete
        acc_buf_sel_reg <= acc_buf_sel;
    end
end

// Write operation (uses registered buffer selection for consistency)
// Also handle clear operation
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        // Reset clears all memory (synthesis should infer this)
        for (int i = 0; i < 256; i++) begin
            buffer0[i] <= 64'h0;
            buffer1[i] <= 64'h0;
        end
    end else begin
        // Clear entire buffer if requested
        if (clear) begin
            if (acc_buf_sel_reg == 0) begin
                for (int i = 0; i < 256; i++) begin
                    buffer0[i] <= 64'h0;
                end
            end else begin
                for (int i = 0; i < 256; i++) begin
                    buffer1[i] <= 64'h0;
                end
            end
        end else if (wr_en) begin
            // Normal write operation
            if (acc_buf_sel_reg == 0)
                buffer0[wr_addr] <= wr_data;
            else
                buffer1[wr_addr] <= wr_data;
        end
    end
end

// Read operation (synchronous with registered buffer selection)
// BRAMs in FPGAs are typically registered, so we use synchronous read
// VPU valid signal is delayed by 1 cycle to match this registered output
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        rd_data <= 64'h0;
    end else if (rd_en) begin
        if (acc_buf_sel_reg == 0)
            rd_data <= buffer0[rd_addr];
        else
            rd_data <= buffer1[rd_addr];
    end
end

endmodule
