`timescale 1ns / 1ps

module simple_test_top (
    input  logic        clk,           // System clock (100MHz)
    input  logic        rst_n,         // Active-low reset

    // Basys 3 Physical Interface
    input  logic [15:0] sw,            // 16 slide switches
    input  logic [4:0]  btn,           // 5 push buttons

    // 7-Segment Display Interface
    output logic [6:0]  seg,           // 7-segment segments (A-G)
    output logic [3:0]  an,            // 7-segment anodes
    output logic        dp,            // Decimal point

    // Status LEDs
    output logic [15:0] led,           // 16 green LEDs

    // UART Interface (for debugging)
    input  logic        uart_rx,
    output logic        uart_tx
);

// ============================================================================
// SIMPLE TEST DESIGN - Just for testing physical interface
// ============================================================================

// Simple registers to store data
logic [31:0] instruction_reg;
logic [255:0] data_reg;
logic [63:0] weight_reg;
logic [7:0] addr_reg;
logic [255:0] result_reg;

// Button debouncing
logic [4:0] btn_prev;
logic [19:0] debounce_counter;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        btn_prev <= 5'b0;
        debounce_counter <= 20'b0;
    end else begin
        btn_prev <= btn;
        debounce_counter <= debounce_counter + 1;
    end
end

logic [4:0] btn_pressed;
assign btn_pressed = ~btn_prev & btn & (debounce_counter > 20'hFFFF);

// Simple state machine
typedef enum logic [2:0] {
    IDLE    = 3'b000,
    INSTR   = 3'b001,
    DATA    = 3'b010,
    WEIGHT  = 3'b011,
    COMPUTE = 3'b100,
    RESULT  = 3'b101
} state_t;

state_t current_state;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        current_state <= IDLE;
        instruction_reg <= 32'b0;
        data_reg <= 256'b0;
        weight_reg <= 64'b0;
        addr_reg <= 8'b0;
        result_reg <= 256'b0;
    end else begin
        // State transitions with buttons
        case (current_state)
            IDLE: begin
                if (btn_pressed[4]) current_state <= INSTR;    // BTNU
                if (btn_pressed[0]) result_reg <= {240'b0, sw}; // BTNC - set result
            end
            INSTR: begin
                if (btn_pressed[4]) current_state <= DATA;     // BTNU
                if (btn_pressed[3]) current_state <= IDLE;     // BTND
                if (btn_pressed[0]) begin                      // BTNC - program instruction
                    instruction_reg <= {8'b0, sw[15:8], sw[7:0], 8'b0};
                end
            end
            DATA: begin
                if (btn_pressed[4]) current_state <= WEIGHT;   // BTNU
                if (btn_pressed[3]) current_state <= INSTR;    // BTND
                if (btn_pressed[0]) begin                      // BTNC - program data
                    data_reg <= {240'b0, sw};
                    addr_reg <= sw[15:8];
                end
            end
            WEIGHT: begin
                if (btn_pressed[4]) current_state <= COMPUTE;  // BTNU
                if (btn_pressed[3]) current_state <= DATA;     // BTND
                if (btn_pressed[0]) begin                      // BTNC - program weight
                    weight_reg <= {48'b0, sw};
                end
            end
            COMPUTE: begin
                if (btn_pressed[4]) current_state <= RESULT;   // BTNU
                if (btn_pressed[3]) current_state <= WEIGHT;   // BTND
                if (btn_pressed[0]) begin                      // BTNC - perform "compute"
                    result_reg <= data_reg + {192'b0, weight_reg[15:0], 48'b0};
                end
            end
            RESULT: begin
                if (btn_pressed[3]) current_state <= COMPUTE;  // BTND
                if (btn_pressed[0]) current_state <= IDLE;     // BTNC - back to idle
            end
        endcase
    end
end

// ============================================================================
// 7-SEGMENT DISPLAY - Show current state and values
// ============================================================================

logic [15:0] display_value;
logic [1:0] digit_select;
logic [16:0] refresh_counter;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        digit_select <= 2'b00;
        refresh_counter <= 17'b0;
    end else begin
        refresh_counter <= refresh_counter + 1;
        if (refresh_counter == 17'd100000) begin
            refresh_counter <= 17'b0;
            digit_select <= digit_select + 1;
        end
    end
end

always_comb begin
    an = 4'b1111;
    an[digit_select] = 1'b0;

    dp = 1'b1;

    case (current_state)
        IDLE:    display_value = {4'b0, 4'b0, 4'b0, 4'h0};
        INSTR:   display_value = instruction_reg[15:0];
        DATA:    display_value = data_reg[15:0];
        WEIGHT:  display_value = weight_reg[15:0];
        COMPUTE: display_value = {4'b0, 4'b0, 4'b0, current_state};
        RESULT:  display_value = result_reg[15:0];
        default: display_value = 16'hDEAD;
    endcase

    // 7-segment encoding (active low)
    case (digit_select)
        2'b00: case (display_value[3:0])
            4'h0: seg = 7'b1000000;
            4'h1: seg = 7'b1111001;
            4'h2: seg = 7'b0100100;
            4'h3: seg = 7'b0110000;
            4'h4: seg = 7'b0011001;
            4'h5: seg = 7'b0010010;
            4'h6: seg = 7'b0000010;
            4'h7: seg = 7'b1111000;
            4'h8: seg = 7'b0000000;
            4'h9: seg = 7'b0010000;
            4'hA: seg = 7'b0001000;
            4'hB: seg = 7'b0000011;
            4'hC: seg = 7'b1000110;
            4'hD: seg = 7'b0100001;
            4'hE: seg = 7'b0000110;
            4'hF: seg = 7'b0001110;
            default: seg = 7'b1111111;
        endcase
        2'b01: case (display_value[7:4])
            4'h0: seg = 7'b1000000;
            4'h1: seg = 7'b1111001;
            4'h2: seg = 7'b0100100;
            4'h3: seg = 7'b0110000;
            4'h4: seg = 7'b0011001;
            4'h5: seg = 7'b0010010;
            4'h6: seg = 7'b0000010;
            4'h7: seg = 7'b1111000;
            4'h8: seg = 7'b0000000;
            4'h9: seg = 7'b0010000;
            4'hA: seg = 7'b0001000;
            4'hB: seg = 7'b0000011;
            4'hC: seg = 7'b1000110;
            4'hD: seg = 7'b0100001;
            4'hE: seg = 7'b0000110;
            4'hF: seg = 7'b0001110;
            default: seg = 7'b1111111;
        endcase
        2'b10: case (display_value[11:8])
            4'h0: seg = 7'b1000000;
            4'h1: seg = 7'b1111001;
            4'h2: seg = 7'b0100100;
            4'h3: seg = 7'b0110000;
            4'h4: seg = 7'b0011001;
            4'h5: seg = 7'b0010010;
            4'h6: seg = 7'b0000010;
            4'h7: seg = 7'b1111000;
            4'h8: seg = 7'b0000000;
            4'h9: seg = 7'b0010000;
            4'hA: seg = 7'b0001000;
            4'hB: seg = 7'b0000011;
            4'hC: seg = 7'b1000110;
            4'hD: seg = 7'b0100001;
            4'hE: seg = 7'b0000110;
            4'hF: seg = 7'b0001110;
            default: seg = 7'b1111111;
        endcase
        2'b11: case (display_value[15:12])
            4'h0: seg = 7'b1000000;
            4'h1: seg = 7'b1111001;
            4'h2: seg = 7'b0100100;
            4'h3: seg = 7'b0110000;
            4'h4: seg = 7'b0011001;
            4'h5: seg = 7'b0010010;
            4'h6: seg = 7'b0000010;
            4'h7: seg = 7'b1111000;
            4'h8: seg = 7'b0000000;
            4'h9: seg = 7'b0010000;
            4'hA: seg = 7'b0001000;
            4'hB: seg = 7'b0000011;
            4'hC: seg = 7'b1000110;
            4'hD: seg = 7'b0100001;
            4'hE: seg = 7'b0000110;
            4'hF: seg = 7'b0001110;
            default: seg = 7'b1111111;
        endcase
    endcase
end

// ============================================================================
// LED STATUS DISPLAY
// ============================================================================

always_comb begin
    case (current_state)
        IDLE:    led = {8'b00000000, 3'b000, current_state};
        INSTR:   led = {8'b00001111, instruction_reg[7:0]};
        DATA:    led = data_reg[15:0];
        WEIGHT:  led = weight_reg[15:0];
        COMPUTE: led = {8'b10101010, 3'b000, current_state};
        RESULT:  led = result_reg[15:0];
        default: led = 16'b1010101010101010;
    endcase
end

// ============================================================================
// UART - Simple echo for debugging
// ============================================================================

assign uart_tx = 1'b1;  // Idle high

endmodule
