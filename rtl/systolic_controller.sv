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
    output logic        acc_wr_en,        // Accumulator write enable
    output logic [7:0]  acc_wr_addr,      // Accumulator write address
    output logic        acc_wr_col01,     // Write acc0+acc1 (even addresses)
    output logic        acc_wr_col2,      // Write acc2 (odd addresses)
    output logic        acc_clear         // Accumulator clear signal
);

// =============================================================================
// STATE MACHINE DEFINITION
// =============================================================================

typedef enum logic [2:0] {
    SYS_IDLE      = 3'b000,
    SYS_LOAD_WEIGHTS = 3'b001,
    SYS_COMPUTE   = 3'b010,
    SYS_WAIT      = 3'b011,
    SYS_DONE      = 3'b100
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
                next_state = SYS_LOAD_WEIGHTS;
            end
        end

        SYS_LOAD_WEIGHTS: begin
            // 5-cycle diagonal wavefront weight loading (3x3 array with staggered timing)
            // CRITICAL: Counter starts at 1 on first cycle (set in always_ff), so check for >= 5
            if (weight_load_counter >= 5) begin
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

// Staggered row+column capture for diagonal wavefront (3x3 array)
// Timing accounts for registered pass-through delay: 1 cycle per row
// Cycle 1: Row 0, Column 0 (pe00) - sees col0_in directly
// Cycle 2: Row 0, Column 1 (pe01) AND Row 1, Column 0 (pe10) - pe10 sees pe00's output (1 cycle delay)
// Cycle 3: Row 0, Column 2 (pe02) AND Row 1, Column 1 (pe11) AND Row 2, Column 0 (pe20) - pe20 sees pe10's output (2 cycle delay)
// Cycle 4: Row 1, Column 2 (pe12) AND Row 2, Column 1 (pe21)
// Cycle 5: Row 2, Column 2 (pe22)
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
        8'h01: begin  // Cycle 1: Row 0, Column 0
            en_capture_row0_col0 = 1'b1;
        end
        8'h02: begin  // Cycle 2: Row 0, Column 1 AND Row 1, Column 0
            en_capture_row0_col1 = 1'b1;
            en_capture_row1_col0 = 1'b1;
        end
        8'h03: begin  // Cycle 3: Row 0, Column 2 AND Row 1, Column 1 AND Row 2, Column 0
            en_capture_row0_col2 = 1'b1;
            en_capture_row1_col1 = 1'b1;
            en_capture_row2_col0 = 1'b1;
        end
        8'h04: begin  // Cycle 4: Row 1, Column 2 AND Row 2, Column 1
            en_capture_row1_col2 = 1'b1;
            en_capture_row2_col1 = 1'b1;
        end
        8'h05: begin  // Cycle 5: Row 2, Column 2
            en_capture_row2_col2 = 1'b1;
        end
        default: ; // All already 0
    endcase
end

// =============================================================================
// ACCUMULATOR CONTROL OUTPUTS
// =============================================================================

// PIPELINE_LATENCY declared earlier
logic acc_wr_en_valid;
logic [7:0] acc_wr_count;  // Counter to track number of writes (max 3 for 3x3)

// CRITICAL FIX: Only enable writes for exactly 3 cycles (3 columns)
// Without this, acc_wr_en_valid stays high for multiple cycles and overwrites data
assign acc_wr_en_valid = (current_state == SYS_COMPUTE) &&
                         (compute_counter >= PIPELINE_LATENCY) &&
                         (acc_wr_count < 3);

// Write counter: Track how many times we've written
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        acc_wr_count <= 8'h00;
    end else if (current_state == SYS_IDLE) begin
        acc_wr_count <= 8'h00;  // Reset when idle
    end else if (acc_wr_en_valid) begin
        acc_wr_count <= acc_wr_count + 1'b1;  // Increment on each write
    end
end

// Write address: start at sys_acc_addr, increment for each column write
// For 3x3 MATMUL: Write 3 results (acc0, acc1, acc2) at consecutive addresses
logic [7:0] acc_wr_addr_reg;
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        acc_wr_addr_reg <= 8'h00;
    end else if (current_state == SYS_IDLE && sys_start) begin
        acc_wr_addr_reg <= sys_acc_addr;  // Initialize to base address
    end else if (acc_wr_en_valid) begin
        // Increment address after writing each column (3 writes total for 3x3)
        acc_wr_addr_reg <= acc_wr_addr_reg + 1'b1;
    end
end

// Write enable: active when valid data is available AND write count < 3
assign acc_wr_en = acc_wr_en_valid;

// Write address: use incremented address
assign acc_wr_addr = acc_wr_addr_reg;

// Column selection: Write each column result in sequence
// For 3x3 MATMUL: Write acc0 (addr), acc1 (addr+1), acc2 (addr+2)
// Use address offset to determine which column
logic [7:0] acc_wr_addr_offset;
assign acc_wr_addr_offset = acc_wr_addr_reg - sys_acc_addr;

// Column selection signals based on address offset
// Offset 0: acc0, Offset 1: acc1, Offset 2: acc2
assign acc_wr_col01 = acc_wr_en_valid && (acc_wr_addr_offset[1:0] != 2'd2);
assign acc_wr_col2 = acc_wr_en_valid && (acc_wr_addr_offset[1:0] == 2'd2);

// Clear accumulator during weight loading phase (before computation starts)
assign acc_clear = acc_clear_request && (current_state == SYS_LOAD_WEIGHTS);

// =============================================================================
// STATUS OUTPUTS
// =============================================================================

assign sys_busy = (current_state != SYS_IDLE);
assign sys_done = (current_state == SYS_DONE);
assign systolic_active = (current_state == SYS_COMPUTE);

endmodule
