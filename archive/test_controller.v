module test_controller (
    input wire clk,
    input wire rst_n,
    output reg sys_start,
    output reg [7:0] sys_rows,
    output reg [7:0] ub_rd_addr,
    output reg wt_fifo_wr
);

// Local parameters
localparam OPCODE_WIDTH = 6;
localparam [OPCODE_WIDTH-1:0] MATMUL_OP = 6'h01;

// Simple logic
reg exec_valid;
reg [OPCODE_WIDTH-1:0] exec_opcode;
reg [7:0] exec_arg1, exec_arg3;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        exec_valid <= 1'b0;
        exec_opcode <= 6'h00;
        exec_arg1 <= 8'h00;
        exec_arg3 <= 8'h00;
    end else begin
        exec_valid <= 1'b1;
        exec_opcode <= MATMUL_OP;
        exec_arg1 <= 8'h10;
        exec_arg3 <= 8'h09;
    end
end

always @* begin
    // Default assignments
    sys_start = 1'b0;
    sys_rows = 8'h00;
    ub_rd_addr = 8'h00;
    wt_fifo_wr = 1'b0;

    if (exec_valid) begin
        case (exec_opcode)
            MATMUL_OP: begin
                sys_start = 1'b1;
                sys_rows = exec_arg3;
                ub_rd_addr = exec_arg1;
            end
            default: begin
                // NOP
            end
        endcase
    end
end

endmodule
