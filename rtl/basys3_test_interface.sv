`timescale 1ns / 1ps

module basys3_test_interface (
    input  logic        clk,           // System clock (100MHz)
    input  logic        rst_n,         // Active-low reset

    // Physical Interface - Basys 3
    input  logic [15:0] sw,            // 16 slide switches
    input  logic [4:0]  btn,           // 5 push buttons [BTNU, BTND, BTNL, BTNR, BTNC]

    // 7-Segment Display Interface
    output logic [6:0]  seg,           // 7-segment segments (A-G)
    output logic [3:0]  an,            // 7-segment anodes
    output logic        dp,            // Decimal point

    // Status LEDs
    output logic [15:0] led,           // 16 green LEDs

    // UART Interface (passthrough to existing UART DMA)
    input  logic        uart_rx,
    output logic        uart_tx,

    // To TPU Core (replaces direct UART DMA connection)
    output logic        ub_wr_en,
    output logic [7:0]  ub_wr_addr,
    output logic [255:0] ub_wr_data,
    output logic        ub_rd_en,
    output logic [7:0]  ub_rd_addr,
    input  logic [255:0] ub_rd_data,

    output logic        wt_wr_en,
    output logic [9:0]  wt_wr_addr,
    output logic [63:0] wt_wr_data,

    output logic        instr_wr_en,
    output logic [4:0]  instr_wr_addr,
    output logic [31:0] instr_wr_data,

    output logic        start_execution,

    // From TPU Core (status)
    input  logic        sys_busy,
    input  logic        sys_done,
    input  logic        vpu_busy,
    input  logic        vpu_done,
    input  logic        ub_busy,
    input  logic        ub_done
);

// ============================================================================
// INTERFACE MODES
// ============================================================================

typedef enum logic [2:0] {
    MODE_IDLE      = 3'b000,  // Idle - show status
    MODE_INSTR     = 3'b001,  // Instruction programming mode
    MODE_DATA      = 3'b010,  // Data programming mode
    MODE_WEIGHT    = 3'b011,  // Weight programming mode
    MODE_EXECUTE   = 3'b100,  // Execution mode
    MODE_RESULTS   = 3'b101   // Results viewing mode
} mode_t;

mode_t current_mode;

// ============================================================================
// BUTTON DEBOUNCING
// ============================================================================

logic [4:0] btn_debounced;
logic [4:0] btn_prev;
logic [19:0] debounce_counter;  // ~20ms at 100MHz

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        btn_debounced <= 5'b0;
        btn_prev <= 5'b0;
        debounce_counter <= 20'b0;
    end else begin
        btn_prev <= btn;

        if (debounce_counter == 20'hFFFFF) begin  // ~20ms debounce
            btn_debounced <= btn;
            debounce_counter <= 20'b0;
        end else begin
            debounce_counter <= debounce_counter + 1;
        end
    end
end

// Button press detection (single pulse)
logic [4:0] btn_press;
assign btn_press = ~btn_prev & btn_debounced;

// Button assignments:
// btn[4] = BTNU (Up)    - Mode up
// btn[3] = BTND (Down)  - Mode down
// btn[2] = BTNL (Left)  - Previous item
// btn[1] = BTNR (Right) - Next item
// btn[0] = BTNC (Center)- Execute/Select

// ============================================================================
// MODE CONTROL
// ============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        current_mode <= MODE_IDLE;
    end else begin
        if (btn_press[4]) begin  // BTNU - next mode
            case (current_mode)
                MODE_IDLE:    current_mode <= MODE_INSTR;
                MODE_INSTR:   current_mode <= MODE_DATA;
                MODE_DATA:    current_mode <= MODE_WEIGHT;
                MODE_WEIGHT:  current_mode <= MODE_EXECUTE;
                MODE_EXECUTE: current_mode <= MODE_RESULTS;
                MODE_RESULTS: current_mode <= MODE_IDLE;
                default:      current_mode <= MODE_IDLE;
            endcase
        end else if (btn_press[3]) begin  // BTND - previous mode
            case (current_mode)
                MODE_IDLE:    current_mode <= MODE_RESULTS;
                MODE_INSTR:   current_mode <= MODE_IDLE;
                MODE_DATA:    current_mode <= MODE_INSTR;
                MODE_WEIGHT:  current_mode <= MODE_DATA;
                MODE_EXECUTE: current_mode <= MODE_WEIGHT;
                MODE_RESULTS: current_mode <= MODE_EXECUTE;
                default:      current_mode <= MODE_IDLE;
            endcase
        end
    end
end

// ============================================================================
// 7-SEGMENT DISPLAY CONTROLLER
// ============================================================================

logic [15:0] display_value;
logic [1:0]  display_mode;  // 0=hex, 1=decimal, 2=status, 3=debug

// 7-segment encoding (active low: 0=on, 1=off)
// Segments: ABCDEFG (A=top, B=top-right, C=bottom-right, D=bottom,
//                    E=bottom-left, F=top-left, G=middle)
function [6:0] hex_to_7seg(input [3:0] hex);
    case (hex)
        4'h0: hex_to_7seg = 7'b1000000;  // 0: ABCDEF
        4'h1: hex_to_7seg = 7'b1111001;  // 1: BC
        4'h2: hex_to_7seg = 7'b0100100;  // 2: ABDEG
        4'h3: hex_to_7seg = 7'b0110000;  // 3: ABCDG
        4'h4: hex_to_7seg = 7'b0011001;  // 4: BCFG
        4'h5: hex_to_7seg = 7'b0010010;  // 5: ACDFG
        4'h6: hex_to_7seg = 7'b0000010;  // 6: ACDEFG
        4'h7: hex_to_7seg = 7'b1111000;  // 7: ABC
        4'h8: hex_to_7seg = 7'b0000000;  // 8: ABCDEFG
        4'h9: hex_to_7seg = 7'b0010000;  // 9: ABCDFG
        4'hA: hex_to_7seg = 7'b0001000;  // A: ABCEFG
        4'hB: hex_to_7seg = 7'b0000011;  // B: CDEFG
        4'hC: hex_to_7seg = 7'b1000110;  // C: ADEF
        4'hD: hex_to_7seg = 7'b0100001;  // D: BCDEG
        4'hE: hex_to_7seg = 7'b0000110;  // E: ADEFG
        4'hF: hex_to_7seg = 7'b0001110;  // F: AEFG
        default: hex_to_7seg = 7'b1111111; // All off
    endcase
endfunction

// Display multiplexing (refresh ~1kHz)
logic [1:0] digit_select;
logic [16:0] refresh_counter;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        digit_select <= 2'b00;
        refresh_counter <= 17'b0;
    end else begin
        refresh_counter <= refresh_counter + 1;
        if (refresh_counter == 17'd100000) begin  // ~1ms at 100MHz
            refresh_counter <= 17'b0;
            digit_select <= digit_select + 1;
        end
    end
end

// Display control
always_comb begin
    an = 4'b1111;  // All off by default
    an[digit_select] = 1'b0;  // Enable current digit

    case (current_mode)
        MODE_IDLE: begin
            // Show last received byte on 7-seg display for debugging
            display_value = {8'h00, uart_debug_last_rx_byte};
            display_mode = 2'b00;  // Hex
        end
        MODE_INSTR: begin
            display_value = {4'h0, 4'h0, 4'h0, sw[15:12]};
            display_mode = 2'b00;  // Hex
        end
        MODE_DATA: begin
            display_value = {sw[15:12], sw[11:8], sw[7:4], sw[3:0]};
            display_mode = 2'b00;  // Hex
        end
        MODE_WEIGHT: begin
            display_value = {4'h0, 4'h0, sw[7:4], sw[3:0]};
            display_mode = 2'b00;  // Hex
        end
        MODE_EXECUTE: begin
            display_value = {4'h0, 4'h0, 4'h0, sys_busy, vpu_busy, 2'b00};
            display_mode = 2'b00;  // Hex
        end
        MODE_RESULTS: begin
            display_value = {4'h0, 4'h0, 4'h0, ub_rd_data[3:0]};
            display_mode = 2'b00;  // Hex
        end
        default: begin
            display_value = 16'hDEAD;
            display_mode = 2'b00;
        end
    endcase

    // Select digit value and convert to 7-segment
    case (digit_select)
        2'b00: seg = hex_to_7seg(display_value[3:0]);
        2'b01: seg = hex_to_7seg(display_value[7:4]);
        2'b10: seg = hex_to_7seg(display_value[11:8]);
        2'b11: seg = hex_to_7seg(display_value[15:12]);
    endcase

    dp = 1'b1;  // Decimal point off
end

// ============================================================================
// LED STATUS DISPLAY (Multiplexed: UART Debug vs Physical Mode)
// ============================================================================

// #region agent log - Monitor UART RX pin directly (HYPOTHESIS A)
// Register UART RX pin to observe its state
reg uart_rx_sync;
always_ff @(posedge clk) begin
    uart_rx_sync <= uart_rx;
end
// #endregion

always_comb begin
    // Always show UART debug info on LEDs for debugging (even when UART inactive)
    // LED[15:12] = Framing error count[3:0] (to see if bytes have errors)
    // LED[11:8] = RX count[3:0] (valid bytes received)
    // LED[7:4] = State[3:0] (UART state machine state)
    // LED[3:0] = Last RX byte[3:0] (last valid byte received, low nibble)
    // This allows us to observe FPGA state without needing UART to work
    // Show command register and full last byte to diagnose bit corruption
    // LED[15:12] = Framing errors
    // LED[11:8] = RX count
    // LED[7:4] = Command register low nibble (what command was captured)
    // LED[3:0] = Last RX byte low nibble (what was actually received)
    // Show state and TX count to diagnose why no response is sent
    // LED[15:12] = Framing errors
    // LED[11:8] = RX count
    // LED[7:4] = UART state machine state (10 = SEND_STATUS = 0xA)
    // LED[3:0] = TX count low nibble (should increment when sending)
    led = {
        uart_debug_framing_error_count[3:0],  // LED[15:12] - Framing errors
        uart_debug_rx_count[3:0],             // LED[11:8] - Valid RX count
        uart_debug_state[3:0],                // LED[7:4] - UART state (0xA = SEND_STATUS)
        uart_debug_tx_count[3:0]             // LED[3:0] - TX count (should increment)
    };
    
    // #region agent log - Additional diagnostic: show full last byte on 7-seg display
    // We can use the 7-segment display to show the full byte value
    // #endregion
    
    // #region agent log - Log LED values for debugging (HYPOTHESIS A, B)
    // The LED values are visible to user, so we can infer FPGA state
    // #endregion
    
    // Original logic (commented out - can restore if needed):
    // if (uart_active) begin
    //     // UART Debug Mode: Show UART state and byte count
    //     led = {uart_debug_byte_count[7:0], uart_debug_state[7:0]};
    // end else begin
    //     // Physical Mode: Show mode-specific status
    //     case (current_mode)
    //         MODE_IDLE:    led = {12'b000000000000, sys_busy, vpu_busy, ub_busy, 1'b0};
    //         MODE_INSTR:   led = {8'b00001111, sw[7:0]};
    //         MODE_DATA:    led = sw;
    //         MODE_WEIGHT:  led = {8'b11110000, sw[7:0]};
    //         MODE_EXECUTE: led = {12'b101010101010, sys_busy, vpu_busy, ub_busy, 1'b0};
    //         MODE_RESULTS: led = ub_rd_data[15:0];
    //         default:      led = 16'b1010101010101010;
    //     endcase
    // end
end

// ============================================================================
// INSTRUCTION/DATA PROGRAMMING
// ============================================================================

logic [31:0] current_instruction;
logic [7:0]  program_counter;
logic [255:0] current_data;
logic [63:0]  current_weight;

// Instruction programming (internal state only - outputs driven by assign statements)
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        current_instruction <= 32'b0;
        program_counter <= 8'b0;
    end else begin
        if (current_mode == MODE_INSTR && btn_press[0]) begin  // BTNC - program instruction
            // Build instruction from switches
            // sw[15:12] = opcode, sw[11:8] = ARG1, sw[7:4] = ARG2, sw[3:0] = ARG3
            current_instruction <= {6'b000000, sw[15:12], sw[11:8], sw[7:4], sw[3:0], 2'b00};
            program_counter <= program_counter + 1;
        end
    end
end

// Data programming (UB) - internal state only - outputs driven by assign statements
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        current_data <= 256'b0;
    end else begin
        if (current_mode == MODE_DATA && btn_press[0]) begin  // BTNC - program data
            current_data <= {240'b0, sw};  // Use switches as 16-bit data
        end
    end
end

// Weight programming - internal state only - outputs driven by assign statements
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        current_weight <= 64'b0;
    end else begin
        if (current_mode == MODE_WEIGHT && btn_press[0]) begin  // BTNC - program weight
            current_weight <= {48'b0, sw};  // Use switches as 16-bit weight
        end
    end
end

// ============================================================================
// EXECUTION CONTROL
// ============================================================================

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        start_execution <= 1'b0;
    end else begin
        start_execution <= 1'b0;

        if (current_mode == MODE_EXECUTE && btn_press[0]) begin  // BTNC - start execution
            start_execution <= 1'b1;
        end
    end
end

// ============================================================================
// RESULTS VIEWING
// ============================================================================

logic [7:0] phys_ub_rd_addr_reg;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        phys_ub_rd_addr_reg <= 8'b0;
    end else begin
        if (current_mode == MODE_RESULTS && !uart_active) begin
            if (btn_press[2]) begin  // BTNL - previous address
                phys_ub_rd_addr_reg <= phys_ub_rd_addr_reg - 1;
            end else if (btn_press[1]) begin  // BTNR - next address
                phys_ub_rd_addr_reg <= phys_ub_rd_addr_reg + 1;
            end
        end
    end
end

// ub_rd_addr multiplexing
assign ub_rd_addr = uart_active ? uart_ub_rd_addr : phys_ub_rd_addr_reg;

// ============================================================================
// UART DMA INTERFACE (Improved with Streaming Mode)
// ============================================================================

// UART DMA signals
logic uart_ub_wr_en;
logic [7:0] uart_ub_wr_addr;
logic [255:0] uart_ub_wr_data;
logic uart_ub_rd_en;
logic [7:0] uart_ub_rd_addr;
logic [255:0] uart_ub_rd_data;
logic uart_wt_wr_en;
logic [9:0] uart_wt_wr_addr;
logic [63:0] uart_wt_wr_data;
logic uart_instr_wr_en;
logic [4:0] uart_instr_wr_addr;
logic [31:0] uart_instr_wr_data;
logic uart_start_execution;

// UART Debug signals
logic [7:0] uart_debug_state;
logic [7:0] uart_debug_cmd;
logic [15:0] uart_debug_byte_count;
logic [31:0] uart_debug_rx_count;
logic [31:0] uart_debug_tx_count;
logic [7:0] uart_debug_last_rx_byte;
logic [31:0] uart_debug_framing_error_count;
logic [7:0] uart_debug_cmd;

// Instantiate standard UART DMA module (complete implementation)
uart_dma_basys3 uart_dma (
    .clk(clk),
    .rst_n(rst_n),
    .uart_rx(uart_rx),
    .uart_tx(uart_tx),
    .ub_wr_en(uart_ub_wr_en),
    .ub_wr_addr(uart_ub_wr_addr),
    .ub_wr_data(uart_ub_wr_data),
    .ub_rd_en(uart_ub_rd_en),
    .ub_rd_addr(uart_ub_rd_addr),
    .ub_rd_data(ub_rd_data),  // Read from UB
    .wt_wr_en(uart_wt_wr_en),
    .wt_wr_addr(uart_wt_wr_addr),
    .wt_wr_data(uart_wt_wr_data),
    .instr_wr_en(uart_instr_wr_en),
    .instr_wr_addr(uart_instr_wr_addr),
    .instr_wr_data(uart_instr_wr_data),
    .start_execution(uart_start_execution),
    .sys_busy(sys_busy),
    .sys_done(sys_done),
    .vpu_busy(vpu_busy),
    .vpu_done(vpu_done),
    .ub_busy(ub_busy),
    .ub_done(ub_done),
    .debug_state(uart_debug_state),
    .debug_cmd(uart_debug_cmd),
    .debug_byte_count(uart_debug_byte_count),
    .debug_rx_count(uart_debug_rx_count),
    .debug_tx_count(uart_debug_tx_count),
    .debug_last_rx_byte(uart_debug_last_rx_byte),
    .debug_framing_error_count(uart_debug_framing_error_count)
);

// Multiplex between UART DMA and physical interface
// UART DMA takes priority when active (when UART is being used)
logic uart_active;
assign uart_active = uart_ub_wr_en | uart_ub_rd_en | uart_wt_wr_en | uart_instr_wr_en | uart_start_execution;

// Physical interface signals (from existing logic)
logic phys_ub_wr_en;
logic [7:0] phys_ub_wr_addr;
logic [255:0] phys_ub_wr_data;
logic phys_ub_rd_en;
logic [7:0] phys_ub_rd_addr;
logic phys_wt_wr_en;
logic [9:0] phys_wt_wr_addr;
logic [63:0] phys_wt_wr_data;
logic phys_instr_wr_en;
logic [4:0] phys_instr_wr_addr;
logic [31:0] phys_instr_wr_data;
logic phys_start_execution;

// Physical interface assignments (from existing always blocks)
assign phys_ub_wr_en = (current_mode == MODE_DATA && btn_press[0]);
assign phys_ub_wr_addr = sw[15:8];
assign phys_ub_wr_data = {240'b0, sw};

assign phys_ub_rd_en = (current_mode == MODE_RESULTS && (btn_press[2] | btn_press[1] | btn_press[0]));
// phys_ub_rd_addr handled in always_ff block

assign phys_wt_wr_en = (current_mode == MODE_WEIGHT && btn_press[0]);
assign phys_wt_wr_addr = sw[15:6];
assign phys_wt_wr_data = {48'b0, sw};

assign phys_instr_wr_en = (current_mode == MODE_INSTR && btn_press[0]);
assign phys_instr_wr_addr = program_counter[4:0];
assign phys_instr_wr_data = {6'b000000, sw[15:12], sw[11:8], sw[7:4], sw[3:0], 2'b00};

assign phys_start_execution = (current_mode == MODE_EXECUTE && btn_press[0]);

// Output multiplexing - UART takes priority
assign ub_wr_en = uart_active ? uart_ub_wr_en : phys_ub_wr_en;
assign ub_wr_addr = uart_active ? uart_ub_wr_addr : phys_ub_wr_addr;
assign ub_wr_data = uart_active ? uart_ub_wr_data : phys_ub_wr_data;

assign ub_rd_en = uart_active ? uart_ub_rd_en : phys_ub_rd_en;
assign ub_rd_addr = uart_active ? uart_ub_rd_addr : phys_ub_rd_addr_reg;

assign wt_wr_en = uart_active ? uart_wt_wr_en : phys_wt_wr_en;
assign wt_wr_addr = uart_active ? uart_wt_wr_addr : phys_wt_wr_addr;
assign wt_wr_data = uart_active ? uart_wt_wr_data : phys_wt_wr_data;

assign instr_wr_en = uart_active ? uart_instr_wr_en : phys_instr_wr_en;
assign instr_wr_addr = uart_active ? uart_instr_wr_addr : phys_instr_wr_addr;
assign instr_wr_data = uart_active ? uart_instr_wr_data : phys_instr_wr_data;

assign start_execution = uart_active ? uart_start_execution : phys_start_execution;

endmodule
