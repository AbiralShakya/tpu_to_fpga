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
    .CLKIN1_PERIOD(10.0),                 // Input clock period (100MHz = 10ns)
    .CLKFBOUT_MULT_F(CLKFBOUT_MULT),       // VCO multiplier
    .CLKOUT0_DIVIDE_F(CLKOUT0_DIVIDE)     // Output divider
) mmcm_inst (
    // Clock outputs (only ports that exist in primitive)
    .CLKOUT0(clk_out_unbuf),              // 100MHz output
    .CLKFBOUT(clkfb),                     // Feedback clock output
    .LOCKED(locked_int),                  // Lock indicator

    // Clock inputs
    .CLKIN1(clk_in),                      // Primary clock input

    // Control
    .PWRDWN(1'b0),                        // Power down (active high)
    .RST(!rst_n),                         // Reset (active high)

    // Feedback input
    .CLKFBIN(clkfb)                       // Feedback clock input
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
