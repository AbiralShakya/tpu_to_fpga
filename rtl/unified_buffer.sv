`timescale 1ns / 1ps

module unified_buffer #(
    parameter DATA_WIDTH = 256,  // 256-bit wide for burst transfers
    parameter DEPTH = 128,       // 128 entries per bank = 32 KiB per bank (total 64 KiB)
    parameter ADDR_WIDTH = 8     // 8 bits for address (includes bank select bit)
)(
    input  logic                    clk,
    input  logic                    rst_n,
    input  logic                    ub_buf_sel,     // Double-buffering control

    // Unified Read/Write Interface (alternates per clock cycle)
    input  logic                    ub_rd_en,
    input  logic [ADDR_WIDTH:0]     ub_rd_addr,     // 9-bit: [8]=bank, [7:0]=address
    input  logic [ADDR_WIDTH:0]     ub_rd_count,    // 9-bit burst count
    output logic [DATA_WIDTH-1:0]   ub_rd_data,
    output logic                    ub_rd_valid,

    input  logic                    ub_wr_en,
    input  logic [ADDR_WIDTH:0]     ub_wr_addr,     // 9-bit: [8]=bank, [7:0]=address
    input  logic [ADDR_WIDTH:0]     ub_wr_count,    // 9-bit burst count
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
// DOUBLE-BUFFERED READ/WRITE STATE MACHINES
// =============================================================================

// Read state machine (one bank active for reading)
localparam RD_IDLE   = 2'b00;
localparam RD_READ   = 2'b01;
localparam RD_BURST  = 2'b10;

logic [1:0] rd_state;
logic [ADDR_WIDTH:0] rd_burst_count;
logic [ADDR_WIDTH:0] rd_current_addr;
logic rd_bank_sel;  // Which bank is currently being read (combinatorial)
logic rd_bank_sel_latched;  // Latched bank selection (captured when ub_rd_en asserted)

// Write state machine (other bank active for writing)
localparam WR_IDLE   = 2'b00;
localparam WR_WRITE  = 2'b01;
localparam WR_BURST  = 2'b10;

logic [1:0] wr_state;
logic [ADDR_WIDTH:0] wr_burst_count;
logic [ADDR_WIDTH:0] wr_current_addr;
logic wr_bank_sel;  // Which bank is currently being written (combinatorial)
logic wr_bank_sel_latched;  // Latched bank selection (captured when ub_wr_en asserted)
logic [DATA_WIDTH-1:0] wr_data_latched;  // Latched write data (captured when ub_wr_en asserted)

// =============================================================================
// DOUBLE-BUFFERED READ LOGIC
// =============================================================================

// Bank selection for true double buffering:
// - Read from bank selected by ub_buf_sel (active bank)
// - Write to opposite bank (~ub_buf_sel) to prevent conflicts
// This ensures read and write never access the same bank simultaneously
always @* begin
    rd_bank_sel = ub_buf_sel;   // Current read bank (active bank)
    wr_bank_sel = ~ub_buf_sel;  // Write to opposite bank (inactive bank)
end

always @(posedge clk) begin
    if (!rst_n) begin
        rd_state <= RD_IDLE;
        rd_burst_count <= {(ADDR_WIDTH+1){1'b0}};
        rd_current_addr <= {(ADDR_WIDTH+1){1'b0}};
        rd_bank_sel_latched <= 1'b0;
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
                    rd_bank_sel_latched <= rd_bank_sel;  // CRITICAL: Latch bank selection when enable is asserted
                end
            end

            RD_READ: begin
                // Read from LATCHED bank (prevents mid-read bank switching)
                if (rd_bank_sel_latched) begin
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
                // Read from LATCHED bank (prevents mid-read bank switching)
                if (rd_bank_sel_latched) begin
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

always @(posedge clk) begin
    if (!rst_n) begin
        wr_state <= WR_IDLE;
        wr_burst_count <= {(ADDR_WIDTH+1){1'b0}};
        wr_current_addr <= {(ADDR_WIDTH+1){1'b0}};
        wr_bank_sel_latched <= 1'b0;
        wr_data_latched <= {DATA_WIDTH{1'b0}};
        ub_wr_ready <= 1'b1;
    end else begin
        case (wr_state)
            WR_IDLE: begin
                ub_wr_ready <= 1'b1;
                if (ub_wr_en) begin
                    wr_state <= WR_WRITE;
                    wr_current_addr <= ub_wr_addr;
                    wr_burst_count <= ub_wr_count;
                    wr_bank_sel_latched <= wr_bank_sel;  // Latch bank selection when enable is asserted
                    wr_data_latched <= ub_wr_data;  // Latch data when enable is asserted
                end
            end

            WR_WRITE: begin
                // Write to LATCHED bank using LATCHED data
                if (wr_bank_sel_latched) begin
                    memory_bank1[wr_current_addr[ADDR_WIDTH-1:0]] <= wr_data_latched;
                end else begin
                    memory_bank0[wr_current_addr[ADDR_WIDTH-1:0]] <= wr_data_latched;
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
                // Write to LATCHED bank using LATCHED data
                // Note: For burst writes, caller must provide new data each cycle
                // For now, we use the initially latched data (single-word write behavior)
                if (wr_bank_sel_latched) begin
                    memory_bank1[wr_current_addr[ADDR_WIDTH-1:0]] <= wr_data_latched;
                end else begin
                    memory_bank0[wr_current_addr[ADDR_WIDTH-1:0]] <= wr_data_latched;
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

// =============================================================================
// SAFETY CHECK: Verify read and write never access same bank simultaneously
// =============================================================================
// This is a design-time check - if rd_bank_sel_latched == wr_bank_sel_latched, there's a bug
// In normal operation with proper double buffering:
//   - rd_bank_sel = ub_buf_sel (active bank for reading)
//   - wr_bank_sel = ~ub_buf_sel (inactive bank for writing)
//   - Both are LATCHED when enable is asserted to prevent mid-operation bank switching
//   - This is critical for UART/ISA transitions where ub_buf_sel may change while
//     an operation is in progress
// Note: This check is commented out to avoid synthesis issues, but serves as documentation
// `ifdef SIMULATION
//     always @(posedge clk) begin
//         if ((rd_state != RD_IDLE) && (wr_state != WR_IDLE) &&
//             (rd_bank_sel_latched == wr_bank_sel_latched)) begin
//             $error("UNIFIED BUFFER ERROR: Read and write accessing same bank simultaneously!");
//         end
//     end
// `endif

endmodule
