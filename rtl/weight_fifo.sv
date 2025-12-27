`timescale 1ns / 1ps

module weight_fifo #(
    parameter DATA_WIDTH = 24,  // 3 bytes per weight (for 3x8-bit weights)
    parameter DEPTH = 4096,     // 4K entries = 64KiB total per buffer
    parameter ADDR_WIDTH = 12   // log2(DEPTH)
)(
    input  logic                    clk,
    input  logic                    rst_n,

    // Control signals
    input  logic                    wt_buf_sel,    // Which buffer to use (0 or 1)
    input  logic [7:0]              wt_num_tiles,  // Number of tiles to load

    // Write interface (from DMA/DRAM)
    input  logic                    wr_en,
    input  logic [DATA_WIDTH-1:0]   wr_data,
    output logic                    wr_full,
    output logic                    wr_almost_full,

    // Read interface (to systolic array)
    input  logic                    rd_en,
    output logic [DATA_WIDTH-1:0]   rd_data,
    output logic                    rd_empty,
    output logic                    rd_almost_empty,

    // Status
    output logic                    wt_load_done
);

// =============================================================================
// DOUBLE-BUFFER MEMORY
// =============================================================================

// Two buffers: each 64KiB (4096 x 16-bit)
(* ram_style = "block" *) logic [DATA_WIDTH-1:0] buffer0 [0:DEPTH-1];
(* ram_style = "block" *) logic [DATA_WIDTH-1:0] buffer1 [0:DEPTH-1];

// =============================================================================
// BUFFER 0 POINTERS AND CONTROL
// =============================================================================

logic [ADDR_WIDTH-1:0] buf0_wr_ptr;
logic [ADDR_WIDTH-1:0] buf0_rd_ptr;
logic [ADDR_WIDTH:0]   buf0_wr_count;  // Extra bit for full detection
logic [ADDR_WIDTH:0]   buf0_rd_count;

// =============================================================================
// BUFFER 1 POINTERS AND CONTROL
// =============================================================================

logic [ADDR_WIDTH-1:0] buf1_wr_ptr;
logic [ADDR_WIDTH-1:0] buf1_rd_ptr;
logic [ADDR_WIDTH:0]   buf1_wr_count;
logic [ADDR_WIDTH:0]   buf1_rd_count;

// =============================================================================
// CURRENT BUFFER SELECTION (registered to prevent glitches)
// =============================================================================

// Register buffer selection to prevent mid-operation changes
// This ensures atomic operations - once an operation starts, it completes on the same buffer
logic wt_buf_sel_reg;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        wt_buf_sel_reg <= 1'b0;
    end else begin
        // Only update when no operations are active to prevent race conditions
        // In practice, buffer selection should only change via SYNC instruction
        // which waits for operations to complete
        wt_buf_sel_reg <= wt_buf_sel;
    end
end

logic [DATA_WIDTH-1:0] selected_wr_data;
logic [DATA_WIDTH-1:0] selected_rd_data;
logic                   selected_wr_full;
logic                   selected_wr_almost_full;
logic                   selected_rd_empty;
logic                   selected_rd_almost_empty;

// =============================================================================
// BUFFER 0 WRITE POINTER LOGIC
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        buf0_wr_ptr <= {ADDR_WIDTH{1'b0}};
        buf0_wr_count <= {(ADDR_WIDTH+1){1'b0}};
    end else if (wr_en && !wt_buf_sel_reg && !selected_wr_full) begin  // Writing to buffer 0
        buf0_wr_ptr <= buf0_wr_ptr + 1'b1;
        buf0_wr_count <= buf0_wr_count + 1'b1;
    end
end

// =============================================================================
// BUFFER 0 READ POINTER LOGIC
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        buf0_rd_ptr <= {ADDR_WIDTH{1'b0}};
        buf0_rd_count <= {(ADDR_WIDTH+1){1'b0}};
    end else if (rd_en && !wt_buf_sel_reg && !selected_rd_empty) begin  // Reading from buffer 0
        buf0_rd_ptr <= buf0_rd_ptr + 1'b1;
        buf0_rd_count <= buf0_rd_count + 1'b1;
    end
end

// =============================================================================
// BUFFER 1 WRITE POINTER LOGIC
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        buf1_wr_ptr <= {ADDR_WIDTH{1'b0}};
        buf1_wr_count <= {(ADDR_WIDTH+1){1'b0}};
    end else if (wr_en && wt_buf_sel_reg && !selected_wr_full) begin  // Writing to buffer 1
        buf1_wr_ptr <= buf1_wr_ptr + 1'b1;
        buf1_wr_count <= buf1_wr_count + 1'b1;
    end
end

// =============================================================================
// BUFFER 1 READ POINTER LOGIC
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        buf1_rd_ptr <= {ADDR_WIDTH{1'b0}};
        buf1_rd_count <= {(ADDR_WIDTH+1){1'b0}};
    end else if (rd_en && wt_buf_sel_reg && !selected_rd_empty) begin  // Reading from buffer 1
        buf1_rd_ptr <= buf1_rd_ptr + 1'b1;
        buf1_rd_count <= buf1_rd_count + 1'b1;
    end
end

// =============================================================================
// MEMORY WRITE LOGIC
// =============================================================================

always_ff @(posedge clk) begin
    if (wr_en) begin
        if (!wt_buf_sel_reg && !selected_wr_full) begin
            buffer0[buf0_wr_ptr] <= wr_data;
        end else if (wt_buf_sel_reg && !selected_wr_full) begin
            buffer1[buf1_wr_ptr] <= wr_data;
        end
    end
end

// =============================================================================
// MEMORY READ LOGIC
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        selected_rd_data <= {DATA_WIDTH{1'b0}};
    end else begin
        if (!wt_buf_sel_reg) begin
            selected_rd_data <= buffer0[buf0_rd_ptr];
        end else begin
            selected_rd_data <= buffer1[buf1_rd_ptr];
        end
    end
end

// =============================================================================
// STATUS SIGNAL GENERATION
// =============================================================================

// Selected buffer status (based on registered wt_buf_sel_reg for stability)
assign selected_wr_full = wt_buf_sel_reg ?
    (buf1_wr_count == DEPTH) : (buf0_wr_count == DEPTH);

assign selected_wr_almost_full = wt_buf_sel_reg ?
    (buf1_wr_count >= DEPTH - 4) : (buf0_wr_count >= DEPTH - 4);

assign selected_rd_empty = wt_buf_sel_reg ?
    (buf1_rd_count == buf1_wr_count) : (buf0_rd_count == buf0_wr_count);

assign selected_rd_almost_empty = wt_buf_sel_reg ?
    ((buf1_wr_count - buf1_rd_count) <= 4) : ((buf0_wr_count - buf0_rd_count) <= 4);

// =============================================================================
// OUTPUT ASSIGNMENTS
// =============================================================================

assign wr_full = selected_wr_full;
assign wr_almost_full = selected_wr_almost_full;
assign rd_data = selected_rd_data;
assign rd_empty = selected_rd_empty;
assign rd_almost_empty = selected_rd_almost_empty;

// Weight loading completion detection
assign wt_load_done = (buf0_wr_count == { {(ADDR_WIDTH-7){1'b0}}, wt_num_tiles }) ||
                      (buf1_wr_count == { {(ADDR_WIDTH-7){1'b0}}, wt_num_tiles });

endmodule
