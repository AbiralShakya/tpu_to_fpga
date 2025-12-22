`timescale 1ns / 1ps

module clock_manager (
    // Clock inputs
    input  logic clk_in,       // External crystal input (100MHz typical)

    // Clock outputs
    output logic clk_out,      // System clock (100MHz)
    output logic clk_locked,   // PLL locked indicator

    // Reset input
    input  logic rst_n
);

// =============================================================================
// MMCME2_BASE PARAMETERS
// =============================================================================

// For Artix-7, we'll configure for 100MHz input -> 100MHz output
// VCO frequency range: 600MHz - 1200MHz for Artix-7
// With input divider = 1, feedback divider = 10, output divider = 10:
// VCO = 100MHz * 10 = 1000MHz (within range)
// Output = 1000MHz / 10 = 100MHz

localparam CLKFBOUT_MULT = 10;     // VCO multiplier
localparam DIVCLK_DIVIDE = 1;      // Input divider
localparam CLKOUT0_DIVIDE = 10;    // Output divider for 100MHz

// =============================================================================
// INTERNAL SIGNALS
// =============================================================================

logic clkfb;           // Feedback clock
logic clk_out_unbuf;   // Unbuffered output clock
logic locked_int;      // Internal locked signal

// =============================================================================
// MMCME2_BASE PRIMITIVE INSTANTIATION
// =============================================================================

MMCME2_BASE #(
    .BANDWIDTH("OPTIMIZED"),              // Jitter programming
    .CLKFBOUT_MULT_F(CLKFBOUT_MULT),       // VCO multiplier
    .CLKFBOUT_PHASE(0.0),                 // Phase offset
    .CLKIN1_PERIOD(10.0),                 // Input clock period (100MHz = 10ns)

    // CLKOUT0 configuration (100MHz output)
    .CLKOUT0_DIVIDE_F(CLKOUT0_DIVIDE),    // Output divider
    .CLKOUT0_DUTY_CYCLE(0.5),             // 50% duty cycle
    .CLKOUT0_PHASE(0.0),                  // Phase offset

    // Other CLKOUT unused
    .CLKOUT1_DIVIDE(1),
    .CLKOUT1_DUTY_CYCLE(0.5),
    .CLKOUT1_PHASE(0.0),
    .CLKOUT2_DIVIDE(1),
    .CLKOUT2_DUTY_CYCLE(0.5),
    .CLKOUT2_PHASE(0.0),
    .CLKOUT3_DIVIDE(1),
    .CLKOUT3_DUTY_CYCLE(0.5),
    .CLKOUT3_PHASE(0.0),
    .CLKOUT4_DIVIDE(1),
    .CLKOUT4_DUTY_CYCLE(0.5),
    .CLKOUT4_PHASE(0.0),
    .CLKOUT5_DIVIDE(1),
    .CLKOUT5_DUTY_CYCLE(0.5),
    .CLKOUT5_PHASE(0.0),
    .CLKOUT6_DIVIDE(1),
    .CLKOUT6_DUTY_CYCLE(0.5),
    .CLKOUT6_PHASE(0.0),

    // Input clock selection
    .CLKIN1_PERIOD(10.0),                 // Input period for CLKIN1
    .REF_JITTER1(0.01),                  // Input jitter

    // Dividers
    .DIVCLK_DIVIDE(DIVCLK_DIVIDE),        // Master division

    // Startup and reset
    .STARTUP_WAIT("FALSE")                // No wait for PLL lock at startup
) mmcm_inst (
    // Clock outputs
    .CLKOUT0(clk_out_unbuf),              // 100MHz output
    .CLKOUT0B(),                          // Inverted output (unused)
    .CLKOUT1(),                           // Unused
    .CLKOUT1B(),                          // Unused
    .CLKOUT2(),                           // Unused
    .CLKOUT2B(),                          // Unused
    .CLKOUT3(),                           // Unused
    .CLKOUT3B(),                           // Unused
    .CLKOUT4(),                           // Unused
    .CLKOUT5(),                           // Unused
    .CLKOUT6(),                           // Unused

    // Feedback
    .CLKFBOUT(clkfb),                     // Feedback clock output
    .CLKFBOUTB(),                         // Inverted feedback (unused)

    // Status
    .LOCKED(locked_int),                  // Lock indicator

    // Clock inputs
    .CLKIN1(clk_in),                      // Primary clock input
    .CLKIN2(1'b0),                        // Secondary clock input (unused)

    // Control
    .PWRDWN(1'b0),                        // Power down (active high)
    .RST(!rst_n),                         // Reset (active high)

    // Feedback input
    .CLKFBIN(clkfb),                      // Feedback clock input

    // Dynamic reconfiguration (unused)
    .DADDR(7'b0),
    .DCLK(1'b0),
    .DEN(1'b0),
    .DI(16'b0),
    .DO(),
    .DRDY(),
    .DWE(1'b0),

    // Phase shift (unused)
    .PSCLK(1'b0),
    .PSEN(1'b0),
    .PSINCDEC(1'b0),
    .PSDONE()
);

// =============================================================================
// OUTPUT BUFFERING
// =============================================================================

// Global clock buffer for the output clock
BUFG clk_bufg (
    .I(clk_out_unbuf),
    .O(clk_out)
);

// =============================================================================
// LOCKED SIGNAL SYNCHRONIZATION
// =============================================================================

// Synchronize the locked signal to the output clock domain
logic locked_sync1, locked_sync2;

always_ff @(posedge clk_out or negedge rst_n) begin
    if (!rst_n) begin
        locked_sync1 <= 1'b0;
        locked_sync2 <= 1'b0;
        clk_locked <= 1'b0;
    end else begin
        locked_sync1 <= locked_int;
        locked_sync2 <= locked_sync1;
        clk_locked <= locked_sync2;
    end
end

endmodule
