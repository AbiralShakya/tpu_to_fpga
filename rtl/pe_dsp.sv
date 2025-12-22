`timescale 1ns / 1ps

module pe_dsp #(
    parameter DATA_WIDTH = 8,
    parameter ACC_WIDTH = 32
)(
    input  logic                    clk,
    input  logic                    rst_n,

    // Weight loading interface
    input  logic                    en_weight_pass,
    input  logic                    en_weight_capture,
    input  logic [DATA_WIDTH-1:0]   weight_in,

    // Activation input (flows right through systolic array)
    input  logic [DATA_WIDTH-1:0]   act_in,

    // Partial sum input (flows down through systolic array)
    input  logic [ACC_WIDTH-1:0]    psum_in,

    // Outputs
    output logic [DATA_WIDTH-1:0]   act_out,     // Activation flows right
    output logic [ACC_WIDTH-1:0]    psum_out     // Partial sum flows down
);

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

// Weight register
logic [DATA_WIDTH-1:0] weight_reg;

// DSP48E1 interface signals
logic signed [29:0] dsp_a;        // 30-bit A input
logic signed [17:0] dsp_b;        // 18-bit B input
logic signed [47:0] dsp_c;        // 48-bit C input
logic signed [47:0] dsp_p;        // 48-bit P output
logic [3:0]         dsp_alumode;  // ALU mode
logic [6:0]         dsp_opmode;   // Operation mode

// MAC result
logic signed [ACC_WIDTH-1:0] mac_result;

// =============================================================================
// WEIGHT REGISTER
// =============================================================================

// Weight capture logic (systolic weight loading)
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        weight_reg <= {DATA_WIDTH{1'b0}};
    end else if (en_weight_capture) begin
        weight_reg <= weight_in;
    end
end

// =============================================================================
// DSP48E1 CONFIGURATION FOR MAC OPERATION
// =============================================================================

// DSP48E1 is configured for: P = C + (A * B)
// Where: A = activation (8-bit, sign-extended to 30-bit)
//        B = weight (8-bit, sign-extended to 18-bit)
//        C = psum_in (32-bit, sign-extended to 48-bit)

// Input assignments
assign dsp_a = {{22{act_in[DATA_WIDTH-1]}}, act_in};     // Sign-extend activation to 30 bits
assign dsp_b = {{10{weight_reg[DATA_WIDTH-1]}}, weight_reg}; // Sign-extend weight to 18 bits
assign dsp_c = {{16{psum_in[ACC_WIDTH-1]}}, psum_in};   // Sign-extend psum to 48 bits

// DSP control (configured for C + A*B)
assign dsp_alumode = 4'b0000;  // Z + W + X + Y + CIN (but CIN=0)
assign dsp_opmode  = 7'b0110101; // X=MULT, Y=MULT, Z=C, W=0

// =============================================================================
// DSP48E1 PRIMITIVE INSTANTIATION
// =============================================================================

// Note: In a real implementation, you would instantiate the actual DSP48E1 primitive
// For simulation purposes, we'll use behavioral modeling

DSP48E1 #(
    .ACASCREG(1),           // Number of pipeline stages on A input
    .ADREG(1),              // Number of AD registers
    .ALUMODEREG(1),         // Number of ALUMODE registers
    .AREG(1),               // Number of A registers
    .AUTORESET_PATDET("NO_RESET"), // Auto-reset pattern detect
    .A_INPUT("DIRECT"),     // A input source
    .BCASCREG(1),           // Number of pipeline stages on B input
    .BREG(1),               // Number of B registers
    .B_INPUT("DIRECT"),     // B input source
    .CARRYINREG(1),         // Number of CARRYIN registers
    .CARRYINSELREG(1),      // Number of CARRYINSEL registers
    .CREG(1),               // Number of C registers
    .DREG(1),               // Number of D registers
    .INMODEREG(1),          // Number of INMODE registers
    .MREG(1),               // Number of multiplier registers
    .OPMODEREG(1),          // Number of OPMODE registers
    .PREG(1),               // Number of P registers
    .USE_DPORT("FALSE"),    // Use D port
    .USE_MULT("MULTIPLY"),  // Use multiplier
    .USE_PATTERN_DETECT("NO_PATDET"), // Pattern detect
    .USE_SIMD("ONE48")      // SIMD mode
) dsp48e1_inst (
    .ACOUT(),               // 30-bit A port cascade output
    .BCOUT(),               // 18-bit B port cascade output
    .CARRYCASCOUT(),        // Cascade carry output
    .MULTSIGNOUT(),         // Multiplier sign cascade output
    .PCOUT(),               // 48-bit cascade output
    .OVERFLOW(),            // Overflow output
    .PATTERNBDETECT(),      // Pattern bar detect output
    .PATTERNDETECT(),       // Pattern detect output
    .UNDERFLOW(),           // Underflow output
    .CARRYOUT(),            // 4-bit carry output
    .P(dsp_p),              // 48-bit output

    .ACIN(30'b0),           // 30-bit A port cascade input
    .BCIN(18'b0),           // 18-bit B port cascade input
    .CARRYCASCIN(1'b0),     // Cascade carry input
    .MULTSIGNIN(1'b0),      // Multiplier sign cascade input
    .PCIN(48'b0),           // 48-bit cascade input
    .ALUMODE(dsp_alumode),  // 4-bit ALU control
    .CARRYINSEL(3'b000),    // 3-bit carry control
    .CLK(clk),              // Clock
    .INMODE(5'b00000),      // 5-bit input mode control
    .OPMODE(dsp_opmode),    // 7-bit operation mode
    .A(dsp_a),              // 30-bit A data input
    .B(dsp_b),              // 18-bit B data input
    .C(dsp_c),              // 48-bit C data input
    .CARRYIN(1'b0),         // 1-bit carry input
    .D(25'b0),              // 25-bit D data input

    .CEA1(1'b1),            // Clock enable for 1st stage A registers
    .CEA2(1'b1),            // Clock enable for 2nd stage A registers
    .CEAD(1'b1),            // Clock enable for AD registers
    .CEALUMODE(1'b1),       // Clock enable for ALUMODE registers
    .CEB1(1'b1),            // Clock enable for 1st stage B registers
    .CEB2(1'b1),            // Clock enable for 2nd stage B registers
    .CEC(1'b1),             // Clock enable for C registers
    .CECARRYIN(1'b1),       // Clock enable for CARRYIN registers
    .CECTRL(1'b1),          // Clock enable for OPMODE and CARRYINSEL
    .CED(1'b1),             // Clock enable for D registers
    .CEINMODE(1'b1),        // Clock enable for INMODE registers
    .CEM(1'b1),             // Clock enable for multiplier registers
    .CEP(1'b1),             // Clock enable for P registers

    .RSTA(!rst_n),          // Reset for A pipeline registers
    .RSTALLCARRYIN(!rst_n), // Reset for carry pipeline registers
    .RSTALUMODE(!rst_n),    // Reset for ALUMODE pipeline registers
    .RSTB(!rst_n),          // Reset for B pipeline registers
    .RSTC(!rst_n),          // Reset for C pipeline registers
    .RSTCTRL(!rst_n),       // Reset for OPMODE and CARRYINSEL
    .RSTD(!rst_n),          // Reset for D pipeline registers
    .RSTINMODE(!rst_n),     // Reset for INMODE pipeline registers
    .RSTM(!rst_n),          // Reset for multiplier registers
    .RSTP(!rst_n)           // Reset for P pipeline registers
);

// =============================================================================
// OUTPUT PROCESSING
// =============================================================================

// Extract the lower ACC_WIDTH bits from DSP result
assign mac_result = dsp_p[ACC_WIDTH-1:0];

// =============================================================================
// OUTPUT ASSIGNMENTS
// =============================================================================

// Activation flows right (registered for systolic timing)
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        act_out <= {DATA_WIDTH{1'b0}};
    end else begin
        act_out <= act_in;
    end
end

// Partial sum output (MAC result flows down)
assign psum_out = mac_result;

endmodule
