`timescale 1ns / 1ps

// =============================================================================
// TPU_TOP SIGNAL PATH TESTBENCH
// =============================================================================
// Verifies that all critical signals are properly connected in tpu_top
// =============================================================================

module tb_tpu_top_signals;

// =============================================================================
// CLOCK AND RESET
// =============================================================================
logic clk = 0;
logic rst_n;

always #5 clk = ~clk;

// =============================================================================
// DUT SIGNALS
// =============================================================================
logic [15:0] sw = 16'h0;
logic [4:0]  btn = 5'h0;
logic [6:0]  seg;
logic [3:0]  an;
logic        dp;
logic [15:0] led;
logic        uart_rx = 1'b1;
logic        uart_tx;

logic        dma_start_in = 0;
logic        dma_dir_in = 0;
logic [7:0]  dma_ub_addr_in = 0;
logic [15:0] dma_length_in = 0;
logic [1:0]  dma_elem_sz_in = 0;
logic [255:0] dma_data_in = 0;
logic        dma_busy_out;
logic        dma_done_out;
logic [255:0] dma_data_out;
logic        tpu_busy;
logic        tpu_done;
logic [1:0]  pipeline_stage;
logic        hazard_detected;
logic [7:0]  uart_debug_state;
logic [7:0]  uart_debug_cmd;
logic [15:0] uart_debug_byte_count;
logic [7:0]  debug_bank_state;

// Test tracking
integer test_pass = 0;
integer test_fail = 0;

// =============================================================================
// DUT
// =============================================================================
tpu_top dut (
    .clk                (clk),
    .rst_n              (rst_n),
    .sw                 (sw),
    .btn                (btn),
    .seg                (seg),
    .an                 (an),
    .dp                 (dp),
    .led                (led),
    .uart_rx            (uart_rx),
    .uart_tx            (uart_tx),
    .dma_start_in       (dma_start_in),
    .dma_dir_in         (dma_dir_in),
    .dma_ub_addr_in     (dma_ub_addr_in),
    .dma_length_in      (dma_length_in),
    .dma_elem_sz_in     (dma_elem_sz_in),
    .dma_data_in        (dma_data_in),
    .dma_busy_out       (dma_busy_out),
    .dma_done_out       (dma_done_out),
    .dma_data_out       (dma_data_out),
    .tpu_busy           (tpu_busy),
    .tpu_done           (tpu_done),
    .pipeline_stage     (pipeline_stage),
    .hazard_detected    (hazard_detected),
    .uart_debug_state   (uart_debug_state),
    .uart_debug_cmd     (uart_debug_cmd),
    .uart_debug_byte_count (uart_debug_byte_count),
    .debug_bank_state   (debug_bank_state)
);

// =============================================================================
// TESTS
// =============================================================================
initial begin
    $display("\n================================================================");
    $display("TPU_TOP Signal Verification Testbench");
    $display("================================================================\n");

    rst_n = 0;
    repeat(10) @(posedge clk);
    rst_n = 1;
    repeat(10) @(posedge clk);

    $display("Reset complete.\n");

    // TEST 1: ub_rd_valid signal exists and is not X
    $display("=== TEST 1: ub_rd_valid in tpu_top ===");
    if (dut.ub_rd_valid === 1'bx) begin
        $display("  FAIL: ub_rd_valid is X");
        test_fail = test_fail + 1;
    end else begin
        $display("  PASS: ub_rd_valid is defined: %b", dut.ub_rd_valid);
        test_pass = test_pass + 1;
    end

    // TEST 2: ub_rd_valid connects from datapath to test_interface
    $display("\n=== TEST 2: ub_rd_valid connection chain ===");
    // Check internal datapath signal
    if (dut.datapath.ub_rd_valid === 1'bx) begin
        $display("  FAIL: datapath.ub_rd_valid is X");
        test_fail = test_fail + 1;
    end else begin
        $display("  PASS: datapath.ub_rd_valid is defined: %b", dut.datapath.ub_rd_valid);
        test_pass = test_pass + 1;
    end
    // Check it matches tpu_top signal
    if (dut.ub_rd_valid == dut.datapath.ub_rd_valid) begin
        $display("  PASS: tpu_top.ub_rd_valid == datapath.ub_rd_valid");
        test_pass = test_pass + 1;
    end else begin
        $display("  FAIL: Signal mismatch!");
        test_fail = test_fail + 1;
    end

    // TEST 3: dma_start is not X (multi-driver fix)
    $display("\n=== TEST 3: dma_start no multi-driver ===");
    if (dut.dma_start === 1'bx) begin
        $display("  FAIL: dma_start is X (multi-driver conflict!)");
        test_fail = test_fail + 1;
    end else begin
        $display("  PASS: dma_start is defined: %b", dut.dma_start);
        test_pass = test_pass + 1;
    end

    // TEST 4: test_start_execution is not X
    $display("\n=== TEST 4: test_start_execution ===");
    if (dut.test_start_execution === 1'bx) begin
        $display("  FAIL: test_start_execution is X");
        test_fail = test_fail + 1;
    end else begin
        $display("  PASS: test_start_execution is defined: %b", dut.test_start_execution);
        test_pass = test_pass + 1;
    end

    // TEST 5: use_test_interface signal
    $display("\n=== TEST 5: use_test_interface signal ===");
    if (dut.use_test_interface === 1'bx) begin
        $display("  FAIL: use_test_interface is X");
        test_fail = test_fail + 1;
    end else begin
        $display("  PASS: use_test_interface is defined: %b", dut.use_test_interface);
        test_pass = test_pass + 1;
    end

    // TEST 6: UB signals multiplexing
    $display("\n=== TEST 6: UB control signal muxing ===");
    if (dut.ub_rd_en === 1'bx) begin
        $display("  FAIL: ub_rd_en is X");
        test_fail = test_fail + 1;
    end else begin
        $display("  PASS: ub_rd_en is defined: %b", dut.ub_rd_en);
        test_pass = test_pass + 1;
    end

    // TEST 7: Datapath internal signals (from BUG #6 fix)
    $display("\n=== TEST 7: Datapath acc_wr_en signals ===");
    if (dut.datapath.sc_acc_wr_en === 1'bx) begin
        $display("  FAIL: sc_acc_wr_en is X");
        test_fail = test_fail + 1;
    end else begin
        $display("  PASS: sc_acc_wr_en is defined: %b", dut.datapath.sc_acc_wr_en);
        test_pass = test_pass + 1;
    end
    if (dut.datapath.acc_wr_en_combined === 1'bx) begin
        $display("  FAIL: acc_wr_en_combined is X");
        test_fail = test_fail + 1;
    end else begin
        $display("  PASS: acc_wr_en_combined is defined: %b", dut.datapath.acc_wr_en_combined);
        test_pass = test_pass + 1;
    end

    // TEST 8: Controller ub_buf_sel output
    $display("\n=== TEST 8: Controller ub_buf_sel ===");
    if (dut.ctrl_ub_buf_sel === 1'bx) begin
        $display("  FAIL: ctrl_ub_buf_sel is X");
        test_fail = test_fail + 1;
    end else begin
        $display("  PASS: ctrl_ub_buf_sel is defined: %b", dut.ctrl_ub_buf_sel);
        test_pass = test_pass + 1;
    end

    // Summary
    $display("\n================================================================");
    $display("TEST SUMMARY");
    $display("================================================================");
    $display("  PASSED: %0d", test_pass);
    $display("  FAILED: %0d", test_fail);
    $display("================================================================\n");

    if (test_fail == 0) begin
        $display("SUCCESS: All tpu_top signal tests passed!");
    end else begin
        $display("FAILURE: Some tests failed");
    end

    $finish;
end

// Waveform dump
initial begin
    $dumpfile("tb_tpu_top_signals.vcd");
    $dumpvars(0, tb_tpu_top_signals);
end

endmodule
