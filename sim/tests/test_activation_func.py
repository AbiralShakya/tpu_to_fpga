"""
Activation Function testbench for TPU
Tests ReLU, ReLU6 activation functions
"""
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, ClockCycles, Timer


@cocotb.test()
async def test_activation_func_reset(dut):
    """Test activation function reset"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.data_in.value = 0
    dut.valid_in.value = 0

    await ClockCycles(dut.clk, 2)

    # Check reset state
    assert dut.valid_out.value == 0, f"valid_out should be 0 after reset, got {dut.valid_out.value}"
    assert dut.data_out.value == 0, f"data_out should be 0 after reset, got {dut.data_out.value}"


@cocotb.test()
async def test_activation_func_relu(dut):
    """Test ReLU activation function (this is what the module actually implements)"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.data_in.value = 0
    dut.valid_in.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Test cases for ReLU: max(0, input)
    test_cases = [
        (10, 10),   # Positive -> positive
        (-5, 0),    # Negative -> 0
        (0, 0),     # Zero -> zero
        (100, 100), # Large positive
        (-100, 0),  # Large negative -> 0
    ]

    for input_val, expected in test_cases:
        dut.data_in.value = input_val
        dut.valid_in.value = 1

        await ClockCycles(dut.clk, 1)
        await Timer(1, units='ns')

        assert dut.data_out.value == expected, f"ReLU({input_val}) should be {expected}, got {dut.data_out.value}"
        assert dut.valid_out.value == 1, f"valid_out should be 1 for input {input_val}"

        dut.valid_in.value = 0
        await ClockCycles(dut.clk, 1)


@cocotb.test()
async def test_activation_func_pipeline(dut):
    """Test activation function pipeline timing"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.data_in.value = 0
    dut.valid_in.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Test that valid_in produces valid_out one cycle later
    dut.data_in.value = 42
    dut.valid_in.value = 1

    await ClockCycles(dut.clk, 1)
    await Timer(1, units='ns')

    assert dut.valid_out.value == 1, "valid_out should be 1 one cycle after valid_in"
    assert dut.data_out.value == 42, "data_out should be 42 (ReLU of 42)"

    # Test valid_in going low
    dut.valid_in.value = 0

    await ClockCycles(dut.clk, 1)
    await Timer(1, units='ns')

    assert dut.valid_out.value == 0, "valid_out should be 0 when valid_in is 0"

    dut._log.info("Activation function tests completed")
