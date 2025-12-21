`timescale 1ns / 1ps

module unified_buffer #(
    parameter DATA_WIDTH = 256,  // 256-bit wide for burst transfers
    parameter DEPTH = 256,       // 256 entries = 64 KiB total
    parameter ADDR_WIDTH = 8     // log2(DEPTH)
)(
    input  logic                    clk,
    input  logic                    rst_n,

    // Port A - Read interface (to systolic array)
    input  logic                    port_a_rd_en,
    input  logic [ADDR_WIDTH-1:0]   port_a_addr,
    input  logic [ADDR_WIDTH-1:0]   port_a_count,   // Burst count
    output logic [DATA_WIDTH-1:0]   port_a_data,
    output logic                    port_a_valid,

    // Port B - Write interface (from DMA/VPU)
    input  logic                    port_b_wr_en,
    input  logic [ADDR_WIDTH-1:0]   port_b_addr,
    input  logic [ADDR_WIDTH-1:0]   port_b_count,   // Burst count
    input  logic [DATA_WIDTH-1:0]   port_b_data,
    output logic                    port_b_ready,

    // Status signals
    output logic                    ub_busy,
    output logic                    ub_done
);

// =============================================================================
// DUAL-PORT MEMORY
// =============================================================================

// 64 KiB unified buffer (256 entries × 256 bits)
(* ram_style = "block" *) logic [DATA_WIDTH-1:0] memory [0:DEPTH-1];

// =============================================================================
// PORT A (READ) STATE MACHINE
// =============================================================================

typedef enum logic [1:0] {
    PORT_A_IDLE   = 2'b00,
    PORT_A_READ   = 2'b01,
    PORT_A_BURST  = 2'b10
} port_a_state_t;

port_a_state_t port_a_state;
logic [ADDR_WIDTH-1:0] port_a_burst_count;
logic [ADDR_WIDTH-1:0] port_a_current_addr;

// =============================================================================
// PORT B (WRITE) STATE MACHINE
// =============================================================================

typedef enum logic [1:0] {
    PORT_B_IDLE   = 2'b00,
    PORT_B_WRITE  = 2'b01,
    PORT_B_BURST  = 2'b10
} port_b_state_t;

port_b_state_t port_b_state;
logic [ADDR_WIDTH-1:0] port_b_burst_count;
logic [ADDR_WIDTH-1:0] port_b_current_addr;

// =============================================================================
// PORT A READ LOGIC
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        port_a_state <= PORT_A_IDLE;
        port_a_burst_count <= {ADDR_WIDTH{1'b0}};
        port_a_current_addr <= {ADDR_WIDTH{1'b0}};
        port_a_data <= {DATA_WIDTH{1'b0}};
        port_a_valid <= 1'b0;
    end else begin
        case (port_a_state)
            PORT_A_IDLE: begin
                port_a_valid <= 1'b0;
                if (port_a_rd_en) begin
                    port_a_state <= PORT_A_READ;
                    port_a_current_addr <= port_a_addr;
                    port_a_burst_count <= port_a_count;
                end
            end

            PORT_A_READ: begin
                port_a_data <= memory[port_a_current_addr];
                port_a_valid <= 1'b1;
                port_a_current_addr <= port_a_current_addr + 1'b1;
                port_a_burst_count <= port_a_burst_count - 1'b1;

                if (port_a_burst_count == 1) begin
                    port_a_state <= PORT_A_IDLE;
                end else begin
                    port_a_state <= PORT_A_BURST;
                end
            end

            PORT_A_BURST: begin
                port_a_data <= memory[port_a_current_addr];
                port_a_valid <= 1'b1;
                port_a_current_addr <= port_a_current_addr + 1'b1;
                port_a_burst_count <= port_a_burst_count - 1'b1;

                if (port_a_burst_count == 0) begin
                    port_a_state <= PORT_A_IDLE;
                    port_a_valid <= 1'b0;
                end
            end

            default: begin
                port_a_state <= PORT_A_IDLE;
            end
        endcase
    end
end

// =============================================================================
// PORT B WRITE LOGIC
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        port_b_state <= PORT_B_IDLE;
        port_b_burst_count <= {ADDR_WIDTH{1'b0}};
        port_b_current_addr <= {ADDR_WIDTH{1'b0}};
        port_b_ready <= 1'b1;
    end else begin
        case (port_b_state)
            PORT_B_IDLE: begin
                port_b_ready <= 1'b1;
                if (port_b_wr_en) begin
                    port_b_state <= PORT_B_WRITE;
                    port_b_current_addr <= port_b_addr;
                    port_b_burst_count <= port_b_count;
                end
            end

            PORT_B_WRITE: begin
                memory[port_b_current_addr] <= port_b_data;
                port_b_current_addr <= port_b_current_addr + 1'b1;
                port_b_burst_count <= port_b_burst_count - 1'b1;
                port_b_ready <= 1'b1;

                if (port_b_burst_count == 1) begin
                    port_b_state <= PORT_B_IDLE;
                end else begin
                    port_b_state <= PORT_B_BURST;
                end
            end

            PORT_B_BURST: begin
                memory[port_b_current_addr] <= port_b_data;
                port_b_current_addr <= port_b_current_addr + 1'b1;
                port_b_burst_count <= port_b_burst_count - 1'b1;
                port_b_ready <= 1'b1;

                if (port_b_burst_count == 0) begin
                    port_b_state <= PORT_B_IDLE;
                end
            end

            default: begin
                port_b_state <= PORT_B_IDLE;
            end
        endcase
    end
end

// =============================================================================
// STATUS SIGNALS
// =============================================================================

assign ub_busy = (port_a_state != PORT_A_IDLE) || (port_b_state != PORT_B_IDLE);
assign ub_done = (port_a_state == PORT_A_IDLE) && (port_b_state == PORT_B_IDLE) &&
                 (port_a_burst_count == 0) && (port_b_burst_count == 0);

endmodule
