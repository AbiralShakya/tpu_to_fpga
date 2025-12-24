module minimal_controller2 (
    input wire clk,
    input wire rst_n,
    output reg sys_start,
    output reg [7:0] sys_rows,
    output reg [7:0] ub_rd_addr,
    output reg wt_fifo_wr,
    output reg vpu_start,
    output reg [3:0] vpu_mode,
    output reg wt_buf_sel,
    output reg acc_buf_sel,
    output reg dma_start,
    output reg dma_dir,
    output reg [7:0] dma_ub_addr,
    output reg [15:0] dma_length,
    output reg [1:0] dma_elem_sz,
    output reg pipeline_stall,
    output reg [1:0] current_stage
);

// Local parameters
localparam OPCODE_WIDTH = 6;
localparam [OPCODE_WIDTH-1:0] MATMUL_OP = 6'h01;
localparam [OPCODE_WIDTH-1:0] RD_WEIGHT_OP = 6'h02;
localparam [OPCODE_WIDTH-1:0] RELU_OP = 6'h03;
localparam [OPCODE_WIDTH-1:0] SYNC_OP = 6'h04;

always @* begin
    sys_start = 1'b0;
    sys_rows = 8'h00;
    ub_rd_addr = 8'h00;
    wt_fifo_wr = 1'b0;
    vpu_start = 1'b0;
    vpu_mode = 4'h0;
    wt_buf_sel = 1'b0;
    acc_buf_sel = 1'b0;
    dma_start = 1'b0;
    dma_dir = 1'b0;
    dma_ub_addr = 8'h00;
    dma_length = 16'h0000;
    dma_elem_sz = 2'b00;
    pipeline_stall = 1'b0;
    current_stage = 2'b00;
end

endmodule
