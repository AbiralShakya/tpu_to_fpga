"""
Normalizer testbench for TPU
Tests gain/bias/shift normalization
"""
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, ClockCycles, Timer


@cocotb.test()
async def test_normalizer_gain_bias(dut):
    """Test gain and bias normalization"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.data_in.value = 0
    dut.valid_in.value = 0
    dut.gain.value = 0
    dut.bias.value = 0
    dut.shift.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Set gain=2, bias=10
    dut.gain.value = 2
    dut.bias.value = 10

    # Test: output = (input * gain) + bias
    test_cases = [
        (5, (5 * 2) + 10),    # 5*2 + 10 = 20
        (0, (0 * 2) + 10),    # 0*2 + 10 = 10
        (-3, (-3 * 2) + 10),  # -3*2 + 10 = 4
    ]

    for input_val, expected in test_cases:
        dut.data_in.value = input_val
        dut.valid_in.value = 1

        await ClockCycles(dut.clk, 1)
        await Timer(1, units='ns')

        assert dut.data_out.value == expected, f"Normalizer({input_val}) with gain=2, bias=10 should be {expected}, got {dut.data_out.value}"
        assert dut.valid_out.value == 1, f"valid_out should be 1 for input {input_val}"

        dut.valid_in.value = 0
        await ClockCycles(dut.clk, 1)


@cocotb.test()
async def test_normalizer_shift(dut):
    """Test shift normalization (division by power of 2)"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.data_in.value = 0
    dut.valid_in.value = 0
    dut.gain.value = 0
    dut.bias.value = 0
    dut.shift.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Set gain=1, bias=0, shift=2 (divide by 4)
    dut.gain.value = 1
    dut.bias.value = 0
    dut.shift.value = 2

    # Test: output = ((input * gain) + bias) >> shift
    test_cases = [
        (16, (16 * 1) + 0 >> 2),   # 16 >> 2 = 4
        (8, (8 * 1) + 0 >> 2),     # 8 >> 2 = 2
        (32, (32 * 1) + 0 >> 2),   # 32 >> 2 = 8
    ]

    for input_val, expected in test_cases:
        dut.data_in.value = input_val
        dut.valid_in.value = 1

        await ClockCycles(dut.clk, 1)
        await Timer(1, units='ns')

        assert dut.data_out.value == expected, f"Normalizer({input_val}) with shift=2 should be {expected}, got {dut.data_out.value}"
        assert dut.valid_out.value == 1, f"valid_out should be 1 for input {input_val}"

        dut.valid_in.value = 0
        await ClockCycles(dut.clk, 1)


@cocotb.test()
async def test_normalizer_full_pipeline(dut):
    """Test full normalization pipeline: gain * input + bias >> shift"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.data_in.value = 0
    dut.valid_in.value = 0
    dut.gain.value = 0
    dut.bias.value = 0
    dut.shift.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Set gain=3, bias=5, shift=1
    # output = ((input * 3) + 5) >> 1
    dut.gain.value = 3
    dut.bias.value = 5
    dut.shift.value = 1

    test_cases = [
        (4, ((4 * 3) >> 1) + 5),   # (12 >> 1) + 5 = 6 + 5 = 11
        (2, ((2 * 3) >> 1) + 5),   # (6 >> 1) + 5 = 3 + 5 = 8
        (10, ((10 * 3) >> 1) + 5), # (30 >> 1) + 5 = 15 + 5 = 20
    ]

    for input_val, expected in test_cases:
        dut.data_in.value = input_val
        dut.valid_in.value = 1

        await ClockCycles(dut.clk, 1)
        await Timer(1, units='ns')

        assert dut.data_out.value == expected, f"Full normalization({input_val}) should be {expected}, got {dut.data_out.value}"
        assert dut.valid_out.value == 1, f"valid_out should be 1 for input {input_val}"

        dut.valid_in.value = 0
        await ClockCycles(dut.clk, 1)

    dut._log.info("Normalizer tests completed")
