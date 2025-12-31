`timescale 1ns / 1ps

module systolic_controller (
    input  logic        clk,
    input  logic        rst_n,

    // Control inputs from main controller
    input  logic        sys_start,
    input  logic [1:0]  sys_mode,        // Operation mode (00=MatMul, 01=Conv2D, 10=Accumulate)
    input  logic [7:0]  sys_rows,         // Number of rows to process
    input  logic [7:0]  sys_acc_addr,    // Accumulator write address
    input  logic        sys_acc_clear,   // Clear accumulator before write
    input  logic        acc_clear_busy,  // Accumulator clear in progress (256 cycles)
    input  logic        acc_clear_complete, // Pulses high when clear finishes

    // Status outputs
    output logic        sys_busy,
    output logic        sys_done,

    // Control outputs to systolic array
    output logic        en_weight_pass,
    // Row+column specific capture signals for staggered timing (registered pass-through delay)
    output logic        en_capture_row0_col0,  // Row 0, Column 0 (pe00)
    output logic        en_capture_row0_col1,  // Row 0, Column 1 (pe01)
    output logic        en_capture_row0_col2,  // Row 0, Column 2 (pe02)
    output logic        en_capture_row1_col0,  // Row 1, Column 0 (pe10) - 1 cycle delay
    output logic        en_capture_row1_col1,  // Row 1, Column 1 (pe11) - 1 cycle delay
    output logic        en_capture_row1_col2,  // Row 1, Column 2 (pe12) - 1 cycle delay
    output logic        en_capture_row2_col0,  // Row 2, Column 0 (pe20) - 2 cycle delay
    output logic        en_capture_row2_col1,  // Row 2, Column 1 (pe21) - 2 cycle delay
    output logic        en_capture_row2_col2,  // Row 2, Column 2 (pe22) - 2 cycle delay

    // Data flow control
    output logic        systolic_active,

    // Additional control outputs
    output logic        acc_wr_en,        // Accumulator write enable (writes all 3 columns at once)
    output logic [7:0]  acc_wr_addr,      // Accumulator write address (single address for all 3 columns)
    output logic        acc_clear,        // Accumulator clear signal
    output logic [7:0]  weight_load_cnt   // Weight loading counter (for FIFO pop gating)
);

// =============================================================================
// STATE MACHINE DEFINITION
// =============================================================================

typedef enum logic [2:0] {
    SYS_IDLE      = 3'b000,
    SYS_CLEAR     = 3'b001,  // Wait for accumulator clear (256 cycles)
    SYS_LOAD_WEIGHTS = 3'b010,
    SYS_COMPUTE   = 3'b011,
    SYS_WAIT      = 3'b100,
    SYS_DONE      = 3'b101
} sys_state_t;

sys_state_t current_state, next_state;

// =============================================================================
// INTERNAL COUNTERS AND REGISTERS
// =============================================================================

logic [7:0] weight_load_counter;  // Counts weight loading cycles
logic [7:0] compute_counter;      // Counts compute cycles
logic [7:0] total_rows;           // Total rows to process

// Latch accumulator clear request (stays high during weight loading)
logic acc_clear_request;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        acc_clear_request <= 1'b0;
    end else if (sys_acc_clear && (current_state == SYS_IDLE)) begin
        // Latch clear request when sys_acc_clear is asserted in IDLE
        acc_clear_request <= 1'b1;
    end else if (current_state == SYS_COMPUTE) begin
        // Clear the request once computation starts
        acc_clear_request <= 1'b0;
    end
end

// CRITICAL FIX: Track if clear has actually started (busy went high)
// This prevents the race condition where we exit SYS_CLEAR before clear begins
logic clear_started;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        clear_started <= 1'b0;
    end else if (current_state == SYS_CLEAR) begin
        // Latch when acc_clear_busy goes high (clear actually started)
        if (acc_clear_busy) begin
            clear_started <= 1'b1;
        end
    end else begin
        // Reset when not in SYS_CLEAR
        clear_started <= 1'b0;
    end
end

// Pipeline latency constant (must be declared before use)
localparam PIPELINE_LATENCY = 4;

// =============================================================================
// STATE MACHINE LOGIC
// =============================================================================

// State register
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        current_state <= SYS_IDLE;
    end else begin
        current_state <= next_state;
    end
end

// Next state logic
always_comb begin
    next_state = current_state;
    case (current_state)
        SYS_IDLE: begin
            if (sys_start) begin
                // If clear requested, go to SYS_CLEAR first, else go to LOAD_WEIGHTS
                if (acc_clear_request) begin
                    next_state = SYS_CLEAR;
                end else begin
                    next_state = SYS_LOAD_WEIGHTS;
                end
            end
        end

        SYS_CLEAR: begin
            // Wait for sequential accumulator clear (256 cycles)
            // Use acc_clear_complete pulse which fires when clear finishes
            // This avoids race conditions with the registered busy signal
            if (acc_clear_complete) begin
                next_state = SYS_LOAD_WEIGHTS;
            end
        end

        SYS_LOAD_WEIGHTS: begin
            // 7-cycle weight loading (3x3 array with psum register delay compensation)
            // Each row hop through psum adds 1 cycle delay, so:
            // - Row 0 PEs capture with column FIFO delays only (0, 1, 2 cycles)
            // - Row 1 PEs need +2 cycles (1 for psum hop + 1 for FIFO to advance)
            // - Row 2 PEs need +2 more cycles
            // Total: 7 cycles to capture all 9 PE weights correctly
            if (weight_load_counter >= 7) begin
                next_state = SYS_COMPUTE;
            end
        end

        SYS_COMPUTE: begin
            // Compute for total_rows + 1 cycles (skew compensation)
            // CRITICAL: Need enough cycles to write all accumulator results
            // For 3x3: Need at least PIPELINE_LATENCY (4) + 3 writes = 7 cycles minimum
            // Use max(total_rows + 1, PIPELINE_LATENCY + total_rows) to ensure all writes complete
            if (compute_counter >= (total_rows + 1 + PIPELINE_LATENCY)) begin
                next_state = SYS_WAIT;
            end
        end

        SYS_WAIT: begin
            // Wait for accumulators to finish
            next_state = SYS_DONE;
        end

        SYS_DONE: begin
            next_state = SYS_IDLE;
        end

        default: begin
            next_state = SYS_IDLE;
        end
    endcase
end

// =============================================================================
// WEIGHT LOAD COUNTER
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        weight_load_counter <= 8'h00;
    end else if (next_state == SYS_LOAD_WEIGHTS && current_state != SYS_LOAD_WEIGHTS) begin
        weight_load_counter <= 8'h01;
    end else if (current_state == SYS_LOAD_WEIGHTS) begin
        weight_load_counter <= weight_load_counter + 1'b1;
    end else begin
        weight_load_counter <= 8'h00;
    end
end

// =============================================================================
// COMPUTE COUNTER
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        compute_counter <= 8'h00;
        total_rows <= 8'h00;
    end else if (current_state == SYS_IDLE && sys_start) begin
        total_rows <= sys_rows;
    end else if (current_state == SYS_COMPUTE) begin
        compute_counter <= compute_counter + 1'b1;
    end else begin
        compute_counter <= 8'h00;
    end
end

// =============================================================================
// CONTROL SIGNAL GENERATION
// =============================================================================

// Weight loading timing (3-cycle diagonal wavefront)
assign en_weight_pass = (current_state == SYS_LOAD_WEIGHTS);

// CORRECTED weight capture timing for 3x3 systolic array
// 
// Key insight: Each row's psum_out is REGISTERED (1-cycle delay), and the weight FIFO
// outputs W[0,j], W[1,j], W[2,j] on cycles 1, 2, 3 respectively for each column.
// Additionally, column FIFOs have staggered delays: col0=0, col1=1, col2=2 cycles.
//
// For a PE to capture the correct weight via psum, we must account for:
// 1. Column FIFO delay (0, 1, or 2 cycles)
// 2. Psum register delay (1 cycle per row hop from top)
//
// Corrected timing:
// - Cycle 1: pe00 (row0,col0) - direct from col0, no delay
// - Cycle 2: pe01 (row0,col1) - col1 has 1-cycle FIFO delay, arrives cycle 2
// - Cycle 3: pe10 (row1,col0), pe02 (row0,col2) - pe10 needs psum from cycle 2; pe02 needs col2 with 2-cycle delay
// - Cycle 4: pe11 (row1,col1) - needs pe01.psum from cycle 3
// - Cycle 5: pe20 (row2,col0), pe12 (row1,col2) - pe20 needs 2 psum hops; pe12 needs pe02.psum
// - Cycle 6: pe21 (row2,col1) - needs 2 psum hops from col1
// - Cycle 7: pe22 (row2,col2) - needs 2 psum hops from col2
//
always_comb begin
    // Default all to 0
    en_capture_row0_col0 = 1'b0;
    en_capture_row0_col1 = 1'b0;
    en_capture_row0_col2 = 1'b0;
    en_capture_row1_col0 = 1'b0;
    en_capture_row1_col1 = 1'b0;
    en_capture_row1_col2 = 1'b0;
    en_capture_row2_col0 = 1'b0;
    en_capture_row2_col1 = 1'b0;
    en_capture_row2_col2 = 1'b0;
    
    case (weight_load_counter)
        8'h01: begin  // Cycle 1: pe00 only
            en_capture_row0_col0 = 1'b1;
        end
        8'h02: begin  // Cycle 2: pe01
            en_capture_row0_col1 = 1'b1;
        end
        8'h03: begin  // Cycle 3: pe10, pe02
            en_capture_row1_col0 = 1'b1;
            en_capture_row0_col2 = 1'b1;
        end
        8'h04: begin  // Cycle 4: pe11
            en_capture_row1_col1 = 1'b1;
        end
        8'h05: begin  // Cycle 5: pe20, pe12
            en_capture_row2_col0 = 1'b1;
            en_capture_row1_col2 = 1'b1;
        end
        8'h06: begin  // Cycle 6: pe21
            en_capture_row2_col1 = 1'b1;
        end
        8'h07: begin  // Cycle 7: pe22
            en_capture_row2_col2 = 1'b1;
        end
        default: ; // All already 0
    endcase
end

// =============================================================================
// ACCUMULATOR CONTROL OUTPUTS
// =============================================================================

// Accumulator write: Write all 3 columns at once to a single address
// With 96-bit wide word architecture, all 3 columns are packed and written in one cycle
logic acc_wr_en_valid;

// Write once when pipeline latency is met (all 3 columns are ready)
assign acc_wr_en_valid = (current_state == SYS_COMPUTE) &&
                         (compute_counter >= (PIPELINE_LATENCY + 2));  // Wait for all 3 columns

// Write enable and address
assign acc_wr_en = acc_wr_en_valid;
assign acc_wr_addr = sys_acc_addr;  // Single address for all 3 columns

// Clear accumulator during SYS_CLEAR state (sequential clear takes 256 cycles)
// Stays high until acc_clear_busy goes low
assign acc_clear = acc_clear_request && (current_state == SYS_CLEAR);

// =============================================================================
// STATUS OUTPUTS
// =============================================================================

assign sys_busy = (current_state != SYS_IDLE);
assign sys_done = (current_state == SYS_DONE);
assign systolic_active = (current_state == SYS_COMPUTE);

// Export weight load counter for FIFO pop gating in datapath
assign weight_load_cnt = weight_load_counter;

endmodule
