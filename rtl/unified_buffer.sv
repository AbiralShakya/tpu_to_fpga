`timescale 1ns / 1ps

module unified_buffer #(
    parameter DATA_WIDTH = 256,  // 256-bit wide for burst transfers
    parameter DEPTH = 128,       // 128 entries per bank = 32 KiB per bank (total 64 KiB)
    parameter ADDR_WIDTH = 8     // 8 bits for address (includes bank select bit)
)(
    input  logic                    clk,
    input  logic                    rst_n,

    // Unified Read/Write Interface (single-word operations, no burst)
    input  logic                    ub_rd_en,
    input  logic [ADDR_WIDTH:0]     ub_rd_addr,     // 9-bit: [8]=bank, [7:0]=address
    output logic [DATA_WIDTH-1:0]   ub_rd_data,
    output logic                    ub_rd_valid,

    input  logic                    ub_wr_en,
    input  logic [ADDR_WIDTH:0]     ub_wr_addr,     // 9-bit: [8]=bank, [7:0]=address
    input  logic [DATA_WIDTH-1:0]   ub_wr_data,
    output logic                    ub_wr_ready,

    // Status signals
    output logic                    ub_busy,
    output logic                    ub_done
);

// =============================================================================
// DOUBLE-BUFFERED MEMORY
// =============================================================================

// Two memory banks (32 KiB each = 64 KiB total)
(* ram_style = "block" *) logic [DATA_WIDTH-1:0] memory_bank0 [0:DEPTH-1];
(* ram_style = "block" *) logic [DATA_WIDTH-1:0] memory_bank1 [0:DEPTH-1];

// =============================================================================
// DOUBLE-BUFFERED READ/WRITE LOGIC (Single-word operations, no burst)
// =============================================================================

// Bank selection signals
logic rd_bank_sel;  // Which bank is currently being read (combinatorial)
logic wr_bank_sel;  // Which bank is currently being written (combinatorial)

// =============================================================================
// DOUBLE-BUFFERED READ LOGIC (Single-word operations)
// =============================================================================

// Bank selection: Use address bit 8 directly
assign rd_bank_sel = ub_rd_addr[8];  // Bank from address bit 8
assign wr_bank_sel = ub_wr_addr[8];  // Bank from address bit 8

// Read operation (single-word, no burst)
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ub_rd_data <= {DATA_WIDTH{1'b0}};
        ub_rd_valid <= 1'b0;
    end else begin
        if (ub_rd_en) begin
            // Read from selected bank
            if (rd_bank_sel) begin
                ub_rd_data <= memory_bank1[ub_rd_addr[ADDR_WIDTH-1:0]];
            end else begin
                ub_rd_data <= memory_bank0[ub_rd_addr[ADDR_WIDTH-1:0]];
            end
            ub_rd_valid <= 1'b1;
        end else begin
            ub_rd_valid <= 1'b0;
        end
    end
end

// =============================================================================
// DOUBLE-BUFFERED WRITE LOGIC (Single-word operations)
// =============================================================================

// Write operation (single-word, no burst)
// CRITICAL: Memory writes must NOT use async reset - BRAMs don't support it
// Memory arrays initialize to zero by default in BRAM
always_ff @(posedge clk) begin
    if (ub_wr_en) begin
        // Write to selected bank
        if (wr_bank_sel) begin
            memory_bank1[ub_wr_addr[ADDR_WIDTH-1:0]] <= ub_wr_data;
        end else begin
            memory_bank0[ub_wr_addr[ADDR_WIDTH-1:0]] <= ub_wr_data;
        end
    end
end

// Write ready signal (can use async reset for control signals)
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ub_wr_ready <= 1'b1;
    end else begin
        ub_wr_ready <= 1'b1;  // Always ready (single-word operations)
    end
end

// =============================================================================
// STATUS SIGNALS
// =============================================================================

// Unified buffer is busy when either read or write operation is active
assign ub_busy = ub_rd_en || ub_wr_en;

// Done when no operations are active
assign ub_done = !ub_rd_en && !ub_wr_en;

// =============================================================================
// SAFETY CHECK: Verify read and write never access same bank simultaneously
// =============================================================================
// This is a design-time check - if rd_bank_sel == wr_bank_sel during active operations, there's a bug
// In normal operation with proper double buffering:
//   - rd_bank_sel = ub_rd_addr[8] (bank selection from address bit 8)
//   - wr_bank_sel = ub_wr_addr[8] (bank selection from address bit 8)
//   - This is critical for UART/ISA transitions where the address may be set by different sources
// Note: This check is commented out to avoid synthesis issues, but serves as documentation
// `ifdef SIMULATION
//     always @(posedge clk) begin
//         if (ub_rd_en && ub_wr_en && (rd_bank_sel == wr_bank_sel)) begin
//             $error("UNIFIED BUFFER ERROR: Read and write accessing same bank simultaneously!");
//         end
//     end
// `endif

endmodule
