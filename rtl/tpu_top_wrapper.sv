`timescale 1ns / 1ps

// Wrapper module for tpu_top that only exposes essential Basys3 ports
// This reduces the number of IOs from 579 to ~50
// Uses separate ports instead of arrays to avoid nextpnr-xilinx double-bracket bug
module tpu_top_wrapper (
    // Essential Basys3 interface
    input  logic        clk,
    input  logic        rst_n,
    
    // Switches (15 ports - SW15 is reset)
    input  logic        sw0, sw1, sw2, sw3, sw4, sw5, sw6, sw7,
    input  logic        sw8, sw9, sw10, sw11, sw12, sw13, sw14,
    
    // Push Buttons (5 ports)
    input  logic        btn0, btn1, btn2, btn3, btn4,
    
    // 7-Segment Display (7 segment ports)
    output logic        seg0, seg1, seg2, seg3, seg4, seg5, seg6,
    
    // 7-Segment Anodes (4 ports)
    output logic        an0, an1, an2, an3,
    
    // Decimal Point
    output logic        dp,
    
    // LEDs (16 ports)
    output logic        led0, led1, led2, led3, led4, led5, led6, led7,
    output logic        led8, led9, led10, led11, led12, led13, led14, led15,

    // UART
    input  logic        uart_rx,
    output logic        uart_tx
);

    //==========================================================================
    // TPU KEEP SIGNALS - Force TPU to be synthesized by connecting to LEDs
    //==========================================================================
    // These signals will carry TPU internal status and MUST reach external LEDs
    // to prevent Vivado from optimizing away the entire TPU core
    //==========================================================================
    logic [15:0] tpu_led_internal;
    logic [15:0] test_interface_led;

    // Local GND signals to prevent global constant routing issues
    // Note: vcc removed - not used anywhere, and causes VCC_WIRE routing errors
    (* keep *) logic gnd = 1'b0;
    (* keep *) logic [255:0] gnd_256 = 256'h0;
    (* keep *) logic [15:0] gnd_16 = 16'h0;
    (* keep *) logic [7:0] gnd_8 = 8'h0;
    (* keep *) logic [1:0] gnd_2 = 2'h0;

    // Internal signals to prevent optimization
    (* keep = "true" *) logic        tpu_busy_internal;
    (* keep = "true" *) logic        tpu_done_internal;
    (* keep = "true" *) logic [1:0]  pipeline_stage_internal;
    (* keep = "true" *) logic        hazard_detected_internal;
    (* keep = "true" *) logic [7:0]  uart_debug_state_internal;
    (* keep = "true" *) logic [7:0]  uart_debug_cmd_internal;
    (* keep = "true" *) logic [15:0] uart_debug_byte_count_internal;

    // Instantiate tpu_top with outputs connected to prevent optimization
    tpu_top tpu_inst (
        .clk(clk),
        .rst_n(rst_n),
        .sw({gnd, sw14, sw13, sw12, sw11, sw10, sw9, sw8, sw7, sw6, sw5, sw4, sw3, sw2, sw1, sw0}),
        .btn({btn4, btn3, btn2, btn1, btn0}),
        .seg({seg6, seg5, seg4, seg3, seg2, seg1, seg0}),
        .an({an3, an2, an1, an0}),
        .dp(dp),
        .led(tpu_led_internal),  // Connect to internal wire, NOT directly to output
        .uart_rx(uart_rx),
        .uart_tx(uart_tx),

        // DMA interface - tie inputs to local GND, outputs connected to prevent optimization
        .dma_start_in(gnd),
        .dma_dir_in(gnd),
        .dma_ub_addr_in(gnd_8),
        .dma_length_in(gnd_16),
        .dma_elem_sz_in(gnd_2),
        .dma_data_in(gnd_256),
        .dma_busy_out(),        // Unused but TPU still runs
        .dma_done_out(),        // Unused but TPU still runs
        .dma_data_out(),        // Unused but TPU still runs

        // Status outputs - CONNECT to internal signals with KEEP attribute
        .tpu_busy(tpu_busy_internal),
        .tpu_done(tpu_done_internal),

        // Debug outputs - CONNECT to internal signals with KEEP attribute
        .pipeline_stage(pipeline_stage_internal),
        .hazard_detected(hazard_detected_internal),
        .uart_debug_state(uart_debug_state_internal),
        .uart_debug_cmd(uart_debug_cmd_internal),
        .uart_debug_byte_count(uart_debug_byte_count_internal)
    );

    //==========================================================================
    // FORCE TPU STATUS TO EXTERNAL OUTPUTS
    //==========================================================================
    // Mix TPU internal signals with LED outputs to prevent optimization
    // This ensures the entire TPU datapath must be synthesized
    //==========================================================================
    assign {led15, led14, led13, led12, led11, led10, led9, led8, led7, led6, led5, led4, led3, led2, led1, led0} =
           tpu_led_internal | {
               8'b0,  // Upper 8 LEDs from test interface
               uart_debug_byte_count_internal[7:6],  // UART byte count
               uart_debug_cmd_internal[3:2],          // UART command
               uart_debug_state_internal[1:0],        // UART state
               tpu_busy_internal,                     // TPU busy (CRITICAL!)
               tpu_done_internal                      // TPU done (CRITICAL!)
           };

endmodule
