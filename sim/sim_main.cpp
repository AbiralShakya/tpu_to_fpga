#include "Vtpu_top_tb.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // Initialize top level
    Vtpu_top_tb* top = new Vtpu_top_tb;

    // Enable waveform tracing
    VerilatedVcdC* tfp = nullptr;
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("tpu_sim.vcd");

    // Initialize simulation inputs
    top->rst_n = 0;
    top->dma_start_in = 0;

    // Run simulation for a few clock cycles
    vluint64_t sim_time = 0;
    const vluint64_t sim_max_time = 1000000;  // 1M time units

    while (sim_time < sim_max_time && !Verilated::gotFinish()) {
        // Toggle clock
        top->clk = !top->clk;

        // Release reset after a few cycles
        if (sim_time > 100) {
            top->rst_n = 1;
        }

        // Evaluate model
        top->eval();

        // Dump waveform
        if (tfp) tfp->dump(sim_time);

        sim_time++;
    }

    // Close waveform file
    if (tfp) {
        tfp->close();
        delete tfp;
    }

    // Clean up
    delete top;

    return 0;
}
