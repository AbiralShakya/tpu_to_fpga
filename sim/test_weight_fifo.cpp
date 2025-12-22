/**
 * Simple Verilator test harness for weight_fifo module
 */
#include "Vweight_fifo.h"
#include <verilated.h>
#include <iostream>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // Create instance
    Vweight_fifo* dut = new Vweight_fifo();

    // Initialize signals
    dut->rst_n = 0;
    dut->clk = 0;
    dut->wt_wr_en = 0;
    dut->wt_num_tiles = 4;  // Test loading 4 tiles
    dut->wt_data = 0;

    std::cout << "Starting weight_fifo test..." << std::endl;
    std::cout << "Cycle | rst_n | wt_wr_en | wt_data | empty | full | wt_load_done" << std::endl;

    // Run for several clock cycles
    for (int cycle = 0; cycle < 20; cycle++) {
        // Toggle clock
        dut->clk = !dut->clk;

        // Set inputs based on cycle
        if (cycle == 2) {
            dut->rst_n = 1;  // Deassert reset on cycle 2
        }

        if (cycle >= 4 && cycle < 8) {
            dut->wt_wr_en = 1;
            dut->wt_data = (cycle - 4) + 1;  // Data: 1, 2, 3, 4
        } else {
            dut->wt_wr_en = 0;
            dut->wt_data = 0;
        }

        // Evaluate the DUT
        dut->eval();

        // Print status on each cycle
        std::cout << "  " << cycle << "   |   " << (int)dut->rst_n
                  << "   |    " << (int)dut->wt_wr_en
                  << "     |   " << (int)dut->wt_data
                  << "    |   " << (int)dut->empty
                  << "   |  " << (int)dut->full
                  << "  |      " << (int)dut->wt_load_done << std::endl;
    }

    delete dut;

    std::cout << "Weight FIFO test completed successfully!" << std::endl;
    return 0;
}
