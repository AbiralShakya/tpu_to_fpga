`timescale 1ns / 1ps

module vpu (
    input  logic        clk,
    input  logic        rst_n,

    // Control inputs
    input  logic        vpu_start,
    input  logic [3:0]  vpu_mode,      // 0001=ReLU, 0010=Sigmoid, etc.
    input  logic [7:0]  vpu_in_addr,
    input  logic [7:0]  vpu_out_addr,
    input  logic [7:0]  vpu_length,

    // Data input (from accumulators)
    input  logic [63:0] vpu_in_data,   // 64-bit (2x32-bit values)

    // Data output (to unified buffer)
    output logic [255:0] vpu_out_data,  // 256-bit output
    output logic        vpu_out_valid,

    // Status
    output logic        vpu_busy,
    output logic        vpu_done
);

// =============================================================================
// PARAMETERS
// =============================================================================

localparam MODE_RELU     = 4'h1;
localparam MODE_SIGMOID  = 4'h2;
localparam MODE_TANH     = 4'h3;
localparam MODE_POOL     = 4'h4;
localparam MODE_BATCHNORM = 4'h5;

// =============================================================================
// STATE MACHINE
// =============================================================================

typedef enum logic [1:0] {
    VPU_IDLE   = 2'b00,
    VPU_PROCESS = 2'b01,
    VPU_OUTPUT = 2'b10,
    VPU_DONE   = 2'b11
} vpu_state_t;

vpu_state_t current_state;

// =============================================================================
// INTERNAL REGISTERS
// =============================================================================

logic [7:0] process_counter;
logic [31:0] data_buffer [0:7];  // Buffer for processing

// ReLU outputs
logic [31:0] relu_result0, relu_result1;

// =============================================================================
// STATE MACHINE LOGIC
// =============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        current_state <= VPU_IDLE;
        process_counter <= 8'h00;
        vpu_out_valid <= 1'b0;
        vpu_out_data <= 256'h0;
    end else begin
        case (current_state)
            VPU_IDLE: begin
                vpu_out_valid <= 1'b0;
                process_counter <= 8'h00;
                if (vpu_start) begin
                    current_state <= VPU_PROCESS;
                end
            end

            VPU_PROCESS: begin
                // Process input data based on mode
                case (vpu_mode)
                    MODE_RELU: begin
                        // ReLU: max(0, x)
                        relu_result0 = ($signed(vpu_in_data[31:0]) > 0) ?
                                      vpu_in_data[31:0] : 32'h00000000;
                        relu_result1 = ($signed(vpu_in_data[63:32]) > 0) ?
                                      vpu_in_data[63:32] : 32'h00000000;

                        // Pack into output buffer
                        data_buffer[process_counter[2:0]] <= relu_result0;
                        data_buffer[process_counter[2:0] + 1] <= relu_result1;
                    end

                    default: begin
                        // Default: pass through
                        data_buffer[process_counter[2:0]] <= vpu_in_data[31:0];
                        data_buffer[process_counter[2:0] + 1] <= vpu_in_data[63:32];
                    end
                endcase

                process_counter <= process_counter + 2;  // Process 2 values per cycle

                if (process_counter >= vpu_length - 2) begin
                    current_state <= VPU_OUTPUT;
                    process_counter <= 8'h00;
                end
            end

            VPU_OUTPUT: begin
                // Output processed data (8 values = 256 bits)
                vpu_out_data <= {
                    data_buffer[7], data_buffer[6], data_buffer[5], data_buffer[4],
                    data_buffer[3], data_buffer[2], data_buffer[1], data_buffer[0]
                };
                vpu_out_valid <= 1'b1;
                current_state <= VPU_DONE;
            end

            VPU_DONE: begin
                vpu_out_valid <= 1'b0;
                current_state <= VPU_IDLE;
            end

            default: begin
                current_state <= VPU_IDLE;
            end
        endcase
    end
end

// =============================================================================
// STATUS OUTPUTS
// =============================================================================

assign vpu_busy = (current_state != VPU_IDLE);
assign vpu_done = (current_state == VPU_DONE);

endmodule
