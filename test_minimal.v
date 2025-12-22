module test_minimal (
    input wire clk,
    input wire rst_n,
    output reg [7:0] out1,
    output reg out2
);

always @* begin
    out1 = 8'h00;
    out2 = 1'b0;
    
    if (!rst_n) begin
        out1 = 8'hFF;
        out2 = 1'b1;
    end
end

endmodule
