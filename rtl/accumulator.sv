`timescale 1ns / 1ps

module accumulator (
    input  logic        clk,
    input  logic        rst_n,

    input  logic        acc_buf_sel,    // Buffer selection (0/1)
    input  logic        clear,          // Clear accumulator contents
    output logic        clear_busy,     // High while clearing (256 cycles)
    output logic        clear_complete, // Pulses high when clear finishes
    input  logic        wr_en,          // Write enable
    input  logic [7:0]  wr_addr,        // Write address (matrix row index)
    input  logic [95:0] wr_data,        // Write data: {Col2[31:0], Col1[31:0], Col0[31:0]}
    input  logic        rd_en,          // Read enable
    input  logic [7:0]  rd_addr,      // Read address (matrix row index)
    output logic [95:0] rd_data         // Read data: {Col2[31:0], Col1[31:0], Col0[31:0]}
);

// Status outputs
assign clear_busy = clear | clear_active;
assign clear_complete = clear_done;

// =============================================================================
// ACCUMULATOR MEMORY (96-bit wide word, double-buffered)
// =============================================================================
// Stores all 3 columns per address: {Col2[31:0], Col1[31:0], Col0[31:0]}
// 256 rows × 96 bits = 3 KiB per buffer, 6 KiB total
// Vivado will cascade ~3 BRAM tiles per buffer to achieve 96-bit width
(* ram_style = "block" *) logic [95:0] buffer0 [0:255];
(* ram_style = "block" *) logic [95:0] buffer1 [0:255];

// Buffer selection (no safety gate - controller must ensure idle state)
logic acc_buf_sel_reg;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        acc_buf_sel_reg <= 1'b0;
    end else begin
        acc_buf_sel_reg <= acc_buf_sel;
    end
end

// =============================================================================
// CLEAR STATE MACHINE
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
        clear_active <= 1'b1;
        clear_addr <= 8'h00;
        clear_done <= 1'b0;
    end else if (clear_active) begin
        if (clear_addr == 8'hFF) begin
            clear_active <= 1'b0;
            clear_done <= 1'b1;
        end else begin
            clear_addr <= clear_addr + 1'b1;
        end
    end else if (!clear) begin
        clear_done <= 1'b0;
    end
end

// =============================================================================
// WRITE PORT
// =============================================================================
logic        internal_wr_en;
logic [7:0]  internal_wr_addr;
logic [95:0] internal_wr_data;

assign internal_wr_en   = clear_active | wr_en;
assign internal_wr_addr = clear_active ? clear_addr : wr_addr;
assign internal_wr_data = clear_active ? 96'h0 : wr_data;

always_ff @(posedge clk) begin
    if (internal_wr_en) begin
        if (acc_buf_sel_reg == 0)
            buffer0[internal_wr_addr] <= internal_wr_data;
        else
            buffer1[internal_wr_addr] <= internal_wr_data;
    end
end

// =============================================================================
// READ PORT
// =============================================================================
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        rd_data <= 96'h0;
    end else if (rd_en) begin
        if (acc_buf_sel_reg == 0)
            rd_data <= buffer0[rd_addr];
        else
            rd_data <= buffer1[rd_addr];
    end
end

endmodule
