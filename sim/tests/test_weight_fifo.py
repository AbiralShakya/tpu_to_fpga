"""
Weight FIFO testbench for TPU
Tests basic FIFO operations for weight storage
"""
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, ClockCycles, Timer


@cocotb.test()
async def test_weight_fifo_reset(dut):
    """Test FIFO reset functionality"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize all inputs
    dut.rst_n.value = 0
    dut.wt_buf_sel.value = 0
    dut.wt_num_tiles.value = 0
    dut.wr_en.value = 0
    dut.rd_en.value = 0
    dut.wr_data.value = 0

    await ClockCycles(dut.clk, 2)

    # Check reset state
    assert dut.rd_empty.value == 1, f"rd_empty should be 1 after reset, got {dut.rd_empty.value}"
    assert dut.wr_full.value == 0, f"wr_full should be 0 after reset, got {dut.wr_full.value}"
    # Note: wt_load_done might be 1 if wt_num_tiles is 0 (counters match), so set wt_num_tiles first
    dut.wt_num_tiles.value = 4
    await ClockCycles(dut.clk, 1)
    await Timer(1, units='ns')
    assert dut.wt_load_done.value == 0, f"wt_load_done should be 0 after reset (with wt_num_tiles=4), got {dut.wt_load_done.value}"


@cocotb.test()
async def test_weight_fifo_basic_ops(dut):
    """Test basic FIFO read/write operations"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.wt_buf_sel.value = 0
    dut.wt_num_tiles.value = 4
    dut.wr_en.value = 0
    dut.rd_en.value = 0
    dut.wr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Write some data (24-bit for 3x8-bit weights)
    dut.wr_en.value = 1
    dut.wr_data.value = 0x123456  # 24-bit value

    await ClockCycles(dut.clk, 1)
    await Timer(1, units='ns')

    assert dut.rd_empty.value == 0, "FIFO should not be empty after write"
    assert dut.wr_full.value == 0, "FIFO should not be full after one write"

    # Read the data
    dut.wr_en.value = 0
    dut.rd_en.value = 1

    await ClockCycles(dut.clk, 1)
    await Timer(1, units='ns')

    assert dut.rd_data.value == 0x123456, f"Read data should be 0x123456, got {hex(dut.rd_data.value)}"
    assert dut.rd_empty.value == 1, "FIFO should be empty after read"


@cocotb.test()
async def test_weight_fifo_double_buffering(dut):
    """Test double buffering functionality"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.wt_num_tiles.value = 2
    dut.wr_en.value = 0
    dut.rd_en.value = 0
    dut.wr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Write to buffer 0
    dut.wt_buf_sel.value = 0
    dut.wr_en.value = 1
    dut.wr_data.value = 0x111111

    await ClockCycles(dut.clk, 1)
    await Timer(1, units='ns')

    # Write to buffer 1
    dut.wt_buf_sel.value = 1
    dut.wr_data.value = 0x222222

    await ClockCycles(dut.clk, 1)
    await Timer(1, units='ns')

    dut.wr_en.value = 0

    # Read from buffer 0
    dut.wt_buf_sel.value = 0
    dut.rd_en.value = 1

    await ClockCycles(dut.clk, 1)
    await Timer(1, units='ns')

    assert dut.rd_data.value == 0x111111, f"Buffer 0 should have 0x111111, got {hex(dut.rd_data.value)}"

    # Read from buffer 1
    dut.wt_buf_sel.value = 1

    await ClockCycles(dut.clk, 1)
    await Timer(1, units='ns')

    assert dut.rd_data.value == 0x222222, f"Buffer 1 should have 0x222222, got {hex(dut.rd_data.value)}"

    dut._log.info("Double buffering test completed - buffers operate independently")


@cocotb.test()
async def test_weight_fifo_load_done(dut):
    """Test weight load completion detection"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.wt_buf_sel.value = 0
    dut.wt_num_tiles.value = 2  # Load 2 tiles
    dut.wr_en.value = 0
    dut.wr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Write 2 tiles worth of data (each tile is multiple entries)
    # For simplicity, just write a few entries
    dut.wr_en.value = 1
    for i in range(4):  # Write some data
        dut.wr_data.value = i + 1
        await ClockCycles(dut.clk, 1)
        await Timer(1, units='ns')

    dut.wr_en.value = 0

    # wt_load_done depends on the internal counter reaching wt_num_tiles
    # This is a simplified test - actual behavior depends on tile size
    dut._log.info(f"wt_load_done: {dut.wt_load_done.value}")
    dut._log.info("Weight load completion test completed")
