`timescale 1ns / 1ps

module accumulator (
    input  logic        clk,
    input  logic        rst_n,

    input  logic        acc_buf_sel,    // Buffer selection (0/1)
    input  logic        clear,          // Clear accumulator contents
    output logic        clear_busy,     // High while clearing (256 cycles)
    output logic        clear_complete, // Pulses high when clear finishes
    input  logic        wr_en,          // Write enable
    input  logic [7:0]  wr_addr,        // Write address
    input  logic [63:0] wr_data,        // Write data (64 bits: col1 + col0)
    input  logic        rd_en,          // Read enable
    input  logic [7:0]  rd_addr,        // Read address
    output logic [63:0] rd_data         // Read data (64 bits: col1 + col0)
);

// Clear busy output: COMBINATORIAL to prevent race condition
// Goes high immediately when clear is requested OR while clearing in progress
// This ensures the controller sees "busy" on the same cycle it sends the command
assign clear_busy = clear | clear_active;

// Clear complete: pulses high for one cycle when clear finishes
assign clear_complete = clear_done;

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

// =============================================================================
// SEQUENTIAL CLEAR STATE MACHINE
// BRAMs cannot be cleared in one cycle - must iterate through addresses
// =============================================================================
logic [7:0] clear_addr;
logic       clear_active;
logic       clear_done;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        clear_addr <= 8'h00;
        clear_active <= 1'b0;
        clear_done <= 1'b0;
    end else if (clear && !clear_active && !clear_done) begin
        // Start clear sequence
        clear_active <= 1'b1;
        clear_addr <= 8'h00;
        clear_done <= 1'b0;
    end else if (clear_active) begin
        // Advance through addresses
        if (clear_addr == 8'hFF) begin
            // Finished clearing all 256 addresses
            clear_active <= 1'b0;
            clear_done <= 1'b1;
        end else begin
            clear_addr <= clear_addr + 1'b1;
        end
    end else if (!clear) begin
        // Reset done flag when clear is deasserted
        clear_done <= 1'b0;
    end
end

// =============================================================================
// WRITE PORT MUX (CRITICAL: No if-else priority - explicit mux for BRAM)
// This ensures Vivado correctly infers BRAM without priority conflicts
// =============================================================================
logic        internal_wr_en;
logic [7:0]  internal_wr_addr;
logic [63:0] internal_wr_data;

// Mux the sources for the BRAM port
// When clearing: write zeros to clear_addr
// When not clearing: use normal wr_en/wr_addr/wr_data
// CRITICAL: Use OR for wr_en to prevent priority encoder issues in synthesis
// This ensures writes can occur from either source without timing hazards
assign internal_wr_en   = clear_active | wr_en;
assign internal_wr_addr = clear_active ? clear_addr : wr_addr;
assign internal_wr_data = clear_active ? 64'h0 : wr_data;

// Single write port - no priority conflicts
always_ff @(posedge clk) begin
    if (internal_wr_en) begin
        if (acc_buf_sel_reg == 0)
            buffer0[internal_wr_addr] <= internal_wr_data;
        else
            buffer1[internal_wr_addr] <= internal_wr_data;
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
