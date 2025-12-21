`timescale 1ns / 1ps

module systolic_controller (
    input  logic        clk,
    input  logic        rst_n,

    // Control inputs from main controller
    input  logic        sys_start,
    input  logic [7:0]  sys_rows,

    // Status outputs
    output logic        sys_busy,
    output logic        sys_done,

    // Control outputs to systolic array
    output logic        en_weight_pass,
    output logic        en_capture_col0,
    output logic        en_capture_col1,
    output logic        en_capture_col2,

    // Data flow control
    output logic        systolic_active
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
            // 3-cycle diagonal wavefront weight loading
            if (weight_load_counter >= 3) begin
                next_state = SYS_COMPUTE;
            end
        end

        SYS_COMPUTE: begin
            // Compute for total_rows + 1 cycles (skew compensation)
            if (compute_counter >= total_rows + 1) begin
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

// Staggered column capture for diagonal wavefront (3x3 array)
always_comb begin
    case (weight_load_counter)
        8'h01: begin  // Cycle 1: Capture column 0
            en_capture_col0 = 1'b1;
            en_capture_col1 = 1'b0;
            en_capture_col2 = 1'b0;
        end
        8'h02: begin  // Cycle 2: Hold column 0, capture column 1
            en_capture_col0 = 1'b0;
            en_capture_col1 = 1'b1;
            en_capture_col2 = 1'b0;
        end
        8'h03: begin  // Cycle 3: Hold columns 0-1, capture column 2
            en_capture_col0 = 1'b0;
            en_capture_col1 = 1'b0;
            en_capture_col2 = 1'b1;
        end
        default: begin  // Other cycles: no capture
            en_capture_col0 = 1'b0;
            en_capture_col1 = 1'b0;
            en_capture_col2 = 1'b0;
        end
    endcase
end

// =============================================================================
// STATUS OUTPUTS
// =============================================================================

assign sys_busy = (current_state != SYS_IDLE);
assign sys_done = (current_state == SYS_DONE);
assign systolic_active = (current_state == SYS_COMPUTE);

endmodule
