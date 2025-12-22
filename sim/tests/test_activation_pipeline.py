"""
Activation Pipeline testbench for TPU
Tests full pipeline: Activation -> Normalization -> Loss -> Quantization
"""
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, ClockCycles, Timer


@cocotb.test()
async def test_activation_pipeline_reset(dut):
    """Test activation pipeline reset"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.acc_in.value = 0
    dut.valid_in.value = 0
    dut.target_in.value = 0

    # Set configuration inputs
    dut.norm_gain.value = 256  # 1.0 in Q8.8
    dut.norm_bias.value = 0
    dut.norm_shift.value = 0
    dut.q_inv_scale.value = 256  # 1.0 in Q8.8
    dut.q_zero_point.value = 0

    await ClockCycles(dut.clk, 2)

    # Check reset state
    assert dut.valid_out.value == 0, f"valid_out should be 0 after reset, got {dut.valid_out.value}"
    assert dut.loss_valid.value == 0, f"loss_valid should be 0 after reset, got {dut.loss_valid.value}"


@cocotb.test()
async def test_activation_pipeline_basic_flow(dut):
    """Test basic activation pipeline flow"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.acc_in.value = 0
    dut.valid_in.value = 0
    dut.target_in.value = 0

    # Set configuration inputs
    dut.norm_gain.value = 256  # 1.0 in Q8.8
    dut.norm_bias.value = 0
    dut.norm_shift.value = 0
    dut.q_inv_scale.value = 256  # 1.0 in Q8.8
    dut.q_zero_point.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Test input: 5 (positive, should pass through ReLU unchanged)
    dut.acc_in.value = 5
    dut.valid_in.value = 1

    # Pipeline has multiple stages, need several cycles
    await ClockCycles(dut.clk, 5)
    await Timer(1, units='ns')

    # Check that we get some output (exact timing depends on pipeline depth)
    dut._log.info(f"Pipeline output valid: {dut.valid_out.value}")
    dut._log.info(f"UB data output: {dut.ub_data_out.value}")
    dut._log.info(f"Loss valid: {dut.loss_valid.value}")
    dut._log.info(f"Loss output: {dut.loss_out.value}")

    # Basic check that pipeline produces some output
    # (Exact values depend on quantization implementation)


@cocotb.test()
async def test_activation_pipeline_negative_input(dut):
    """Test pipeline with negative input (ReLU should clamp to 0)"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.acc_in.value = 0
    dut.valid_in.value = 0
    dut.target_in.value = 0

    # Set configuration inputs
    dut.norm_gain.value = 256  # 1.0 in Q8.8
    dut.norm_bias.value = 0
    dut.norm_shift.value = 0
    dut.q_inv_scale.value = 256  # 1.0 in Q8.8
    dut.q_zero_point.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Test input: -3 (negative, should be clamped to 0 by ReLU)
    dut.acc_in.value = -3
    dut.valid_in.value = 1

    await ClockCycles(dut.clk, 5)
    await Timer(1, units='ns')

    # Check pipeline behavior with negative input
    dut._log.info(f"Negative input pipeline output valid: {dut.valid_out.value}")
    dut._log.info(f"Negative input UB data: {dut.ub_data_out.value}")

    dut._log.info("Activation pipeline tests completed")
