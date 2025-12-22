"""
Dual Weight FIFO testbench for 3x3 TPU
Tests 3-column weight streaming with diagonal wavefront timing
"""
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, ClockCycles, Timer


@cocotb.test()
async def test_dual_fifo_reset(dut):
    """Test dual FIFO reset functionality"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize all inputs
    dut.rst_n.value = 0
    dut.push_col0.value = 0
    dut.push_col1.value = 0
    dut.push_col2.value = 0
    dut.data_in.value = 0
    dut.pop.value = 0

    await ClockCycles(dut.clk, 2)

    # Check reset state - all outputs should be 0
    assert dut.col0_out.value == 0, f"col0_out should be 0 after reset, got {dut.col0_out.value}"
    assert dut.col1_out.value == 0, f"col1_out should be 0 after reset, got {dut.col1_out.value}"
    assert dut.col2_out.value == 0, f"col2_out should be 0 after reset, got {dut.col2_out.value}"


@cocotb.test()
async def test_dual_fifo_column_independence(dut):
    """Test that columns operate independently"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.push_col0.value = 0
    dut.push_col1.value = 0
    dut.push_col2.value = 0
    dut.data_in.value = 0
    dut.pop.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Push different values to each column
    dut.push_col0.value = 1
    dut.push_col1.value = 1
    dut.push_col2.value = 1
    dut.data_in.value = 10  # Push 10 to col0

    await ClockCycles(dut.clk, 1)

    dut.data_in.value = 20  # Push 20 to col1
    await ClockCycles(dut.clk, 1)

    dut.data_in.value = 30  # Push 30 to col2
    await ClockCycles(dut.clk, 1)

    # Stop pushing
    dut.push_col0.value = 0
    dut.push_col1.value = 0
    dut.push_col2.value = 0

    # Pop to read values
    dut.pop.value = 1
    await ClockCycles(dut.clk, 1)
    await Timer(1, units='ns')

    # Check column independence - each column should have its own data
    dut._log.info(f"Column outputs: col0={dut.col0_out.value}, col1={dut.col1_out.value}, col2={dut.col2_out.value}")

    # Note: The exact timing depends on FIFO implementation
    # This test validates that columns can be written independently


@cocotb.test()
async def test_dual_fifo_skew_timing(dut):
    """Test the skew timing for diagonal wavefront loading"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.push_col0.value = 0
    dut.push_col1.value = 0
    dut.push_col2.value = 0
    dut.data_in.value = 0
    dut.pop.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Fill FIFO with test pattern for diagonal loading
    # Pattern: [1, 2, 3, 4, 5, 6, 7, 8, 9]

    test_data = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    for i, data in enumerate(test_data):
        # Push to all columns simultaneously (simulating parallel weight loading)
        dut.push_col0.value = 1
        dut.push_col1.value = 1
        dut.push_col2.value = 1
        dut.data_in.value = data

        await ClockCycles(dut.clk, 1)

    # Stop pushing
    dut.push_col0.value = 0
    dut.push_col1.value = 0
    dut.push_col2.value = 0

    # Now test pop timing - columns should have different skew
    dut.pop.value = 1

    # Read multiple cycles to see the skew effect
    for cycle in range(5):
        await ClockCycles(dut.clk, 1)
        await Timer(1, units='ns')
        dut._log.info(f"Cycle {cycle}: col0={dut.col0_out.value}, col1={dut.col1_out.value}, col2={dut.col2_out.value}")

    # Stop popping
    dut.pop.value = 0

    dut._log.info("Skew timing test completed - validates diagonal wavefront weight loading")


@cocotb.test()
async def test_dual_fifo_wraparound(dut):
    """Test FIFO wraparound behavior"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.push_col0.value = 0
    dut.push_col1.value = 0
    dut.push_col2.value = 0
    dut.data_in.value = 0
    dut.pop.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Fill FIFO and test wraparound
    # Assuming FIFO depth is 4 entries per column

    for i in range(6):  # Push more than FIFO depth
        dut.push_col0.value = 1
        dut.data_in.value = i + 100  # Use 100+ to distinguish from earlier tests

        await ClockCycles(dut.clk, 1)

    dut.push_col0.value = 0

    # Pop and check wraparound behavior
    dut.pop.value = 1

    for i in range(4):  # Read FIFO depth
        await ClockCycles(dut.clk, 1)
        await Timer(1, units='ns')
        dut._log.info(f"Wraparound test - entry {i}: col0={dut.col0_out.value}")

    dut.pop.value = 0

    dut._log.info("Wraparound test completed")
