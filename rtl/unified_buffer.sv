`timescale 1ns / 1ps

module unified_buffer #(
    parameter DATA_WIDTH = 256,  // 256-bit wide for burst transfers
    parameter DEPTH = 128,       // 128 entries per bank = 32 KiB per bank (total 64 KiB)
    parameter ADDR_WIDTH = 7     // log2(DEPTH) per bank
)(
    input  wire                     clk,
    input  wire                     rst_n,
    input  wire                     ub_buf_sel,     // Double-buffering control

    // Unified Read/Write Interface (alternates per clock cycle)
    input  wire                     ub_rd_en,
    input  wire [ADDR_WIDTH:0]      ub_rd_addr,     // +1 bit for bank selection
    input  wire [ADDR_WIDTH:0]      ub_rd_count,    // Burst count
    output reg [DATA_WIDTH-1:0]     ub_rd_data,
    output reg                      ub_rd_valid,

    input  wire                     ub_wr_en,
    input  wire [ADDR_WIDTH:0]      ub_wr_addr,     // +1 bit for bank selection
    input  wire [ADDR_WIDTH:0]      ub_wr_count,    // Burst count
    input  wire [DATA_WIDTH-1:0]    ub_wr_data,
    output reg                      ub_wr_ready,

    // Status signals
    output wire                     ub_busy,
    output wire                     ub_done
);

// =============================================================================
// DOUBLE-BUFFERED MEMORY
// =============================================================================

// Two memory banks (32 KiB each = 64 KiB total)
(* ram_style = "block" *) reg [DATA_WIDTH-1:0] memory_bank0 [0:DEPTH-1];
(* ram_style = "block" *) reg [DATA_WIDTH-1:0] memory_bank1 [0:DEPTH-1];

// =============================================================================
// DOUBLE-BUFFERED READ/WRITE STATE MACHINES
// =============================================================================

// Read state machine (one bank active for reading)
localparam RD_IDLE   = 2'b00;
localparam RD_READ   = 2'b01;
localparam RD_BURST  = 2'b10;

reg [1:0] rd_state;
reg [ADDR_WIDTH:0] rd_burst_count;
reg [ADDR_WIDTH:0] rd_current_addr;
reg rd_bank_sel;  // Which bank is currently being read

// Write state machine (other bank active for writing)
localparam WR_IDLE   = 2'b00;
localparam WR_WRITE  = 2'b01;
localparam WR_BURST  = 2'b10;

reg [1:0] wr_state;
reg [ADDR_WIDTH:0] wr_burst_count;
reg [ADDR_WIDTH:0] wr_current_addr;
reg wr_bank_sel;  // Which bank is currently being written

// =============================================================================
// DOUBLE-BUFFERED READ LOGIC
// =============================================================================

// Bank selection based on ub_buf_sel (alternates every clock cycle)
always @* begin
    rd_bank_sel = ub_buf_sel;  // Current read bank
    wr_bank_sel = ~ub_buf_sel; // Current write bank (opposite)
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        rd_state <= RD_IDLE;
        rd_burst_count <= {(ADDR_WIDTH+1){1'b0}};
        rd_current_addr <= {(ADDR_WIDTH+1){1'b0}};
        ub_rd_data <= {DATA_WIDTH{1'b0}};
        ub_rd_valid <= 1'b0;
    end else begin
        case (rd_state)
            RD_IDLE: begin
                ub_rd_valid <= 1'b0;
                if (ub_rd_en) begin
                    rd_state <= RD_READ;
                    rd_current_addr <= ub_rd_addr;
                    rd_burst_count <= ub_rd_count;
                end
            end

            RD_READ: begin
                // Read from current bank
                if (rd_bank_sel) begin
                    ub_rd_data <= memory_bank1[rd_current_addr[ADDR_WIDTH-1:0]];
                end else begin
                    ub_rd_data <= memory_bank0[rd_current_addr[ADDR_WIDTH-1:0]];
                end
                ub_rd_valid <= 1'b1;
                rd_current_addr <= rd_current_addr + 1'b1;
                rd_burst_count <= rd_burst_count - 1'b1;

                if (rd_burst_count == 1) begin
                    rd_state <= RD_IDLE;
                end else begin
                    rd_state <= RD_BURST;
                end
            end

            RD_BURST: begin
                // Read from current bank
                if (rd_bank_sel) begin
                    ub_rd_data <= memory_bank1[rd_current_addr[ADDR_WIDTH-1:0]];
                end else begin
                    ub_rd_data <= memory_bank0[rd_current_addr[ADDR_WIDTH-1:0]];
                end
                ub_rd_valid <= 1'b1;
                rd_current_addr <= rd_current_addr + 1'b1;
                rd_burst_count <= rd_burst_count - 1'b1;

                if (rd_burst_count == 0) begin
                    rd_state <= RD_IDLE;
                    ub_rd_valid <= 1'b0;
                end
            end

            default: begin
                rd_state <= RD_IDLE;
            end
        endcase
    end
end

// =============================================================================
// DOUBLE-BUFFERED WRITE LOGIC
// =============================================================================

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        wr_state <= WR_IDLE;
        wr_burst_count <= {(ADDR_WIDTH+1){1'b0}};
        wr_current_addr <= {(ADDR_WIDTH+1){1'b0}};
        ub_wr_ready <= 1'b1;
    end else begin
        case (wr_state)
            WR_IDLE: begin
                ub_wr_ready <= 1'b1;
                if (ub_wr_en) begin
                    wr_state <= WR_WRITE;
                    wr_current_addr <= ub_wr_addr;
                    wr_burst_count <= ub_wr_count;
                end
            end

            WR_WRITE: begin
                // Write to current bank
                if (wr_bank_sel) begin
                    memory_bank1[wr_current_addr[ADDR_WIDTH-1:0]] <= ub_wr_data;
                end else begin
                    memory_bank0[wr_current_addr[ADDR_WIDTH-1:0]] <= ub_wr_data;
                end
                wr_current_addr <= wr_current_addr + 1'b1;
                wr_burst_count <= wr_burst_count - 1'b1;
                ub_wr_ready <= 1'b1;

                if (wr_burst_count == 1) begin
                    wr_state <= WR_IDLE;
                end else begin
                    wr_state <= WR_BURST;
                end
            end

            WR_BURST: begin
                // Write to current bank
                if (wr_bank_sel) begin
                    memory_bank1[wr_current_addr[ADDR_WIDTH-1:0]] <= ub_wr_data;
                end else begin
                    memory_bank0[wr_current_addr[ADDR_WIDTH-1:0]] <= ub_wr_data;
                end
                wr_current_addr <= wr_current_addr + 1'b1;
                wr_burst_count <= wr_burst_count - 1'b1;
                ub_wr_ready <= 1'b1;

                if (wr_burst_count == 0) begin
                    wr_state <= WR_IDLE;
                end
            end

            default: begin
                wr_state <= WR_IDLE;
            end
        endcase
    end
end

// =============================================================================
// STATUS SIGNALS
// =============================================================================

// Unified buffer is busy when either read or write operation is active
assign ub_busy = (rd_state != RD_IDLE) || (wr_state != WR_IDLE);

// Done when both read and write operations are complete
assign ub_done = (rd_state == RD_IDLE) && (wr_state == WR_IDLE) &&
                 (rd_burst_count == 0) && (wr_burst_count == 0);

endmodule
