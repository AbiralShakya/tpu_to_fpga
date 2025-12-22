"""
MLP Integration testbench for TPU
Tests mlp_top module with proper interface
"""
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, ClockCycles, Timer


@cocotb.test()
async def test_mlp_reset(dut):
    """Test MLP top reset functionality"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize all inputs
    dut.rst_n.value = 0
    dut.wf_push_col0.value = 0
    dut.wf_push_col1.value = 0
    dut.wf_push_col2.value = 0
    dut.wf_data_in.value = 0
    dut.wf_reset.value = 0
    dut.init_act_valid.value = 0
    dut.init_act_data.value = 0
    dut.start_mlp.value = 0
    dut.weights_ready.value = 0
    dut.norm_gain.value = 256  # 1.0 in Q8.8
    dut.norm_bias.value = 0
    dut.norm_shift.value = 0
    dut.q_inv_scale.value = 256  # 1.0 in Q8.8
    dut.q_zero_point.value = 0

    await ClockCycles(dut.clk, 2)

    # Check reset state
    assert dut.state.value == 0, f"state should be 0 after reset, got {dut.state.value}"
    assert dut.cycle_cnt.value == 0, f"cycle_cnt should be 0 after reset, got {dut.cycle_cnt.value}"
    assert dut.current_layer.value == 0, f"current_layer should be 0 after reset, got {dut.current_layer.value}"
    assert dut.layer_complete.value == 0, f"layer_complete should be 0 after reset, got {dut.layer_complete.value}"
    assert dut.acc_valid.value == 0, f"acc_valid should be 0 after reset, got {dut.acc_valid.value}"

    dut._log.info("MLP reset test passed")


@cocotb.test()
async def test_mlp_weight_loading(dut):
    """Test weight loading into MLP"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.wf_push_col0.value = 0
    dut.wf_push_col1.value = 0
    dut.wf_push_col2.value = 0
    dut.wf_data_in.value = 0
    dut.wf_reset.value = 0
    dut.init_act_valid.value = 0
    dut.init_act_data.value = 0
    dut.start_mlp.value = 0
    dut.weights_ready.value = 0
    dut.norm_gain.value = 256
    dut.norm_bias.value = 0
    dut.norm_shift.value = 0
    dut.q_inv_scale.value = 256
    dut.q_zero_point.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Load some test weights into the FIFO
    # Push 8-bit weight values to different columns
    test_weights = [10, 20, 30, 40, 50, 60]

    for i, weight in enumerate(test_weights):
        # Push to all columns simultaneously (shared data bus)
        dut.wf_push_col0.value = 1
        dut.wf_push_col1.value = 1
        dut.wf_push_col2.value = 1
        dut.wf_data_in.value = weight

        await ClockCycles(dut.clk, 1)
        await Timer(1, units='ns')

    # Stop pushing
    dut.wf_push_col0.value = 0
    dut.wf_push_col1.value = 0
    dut.wf_push_col2.value = 0

    # Signal that weights are ready
    dut.weights_ready.value = 1

    await ClockCycles(dut.clk, 2)

    dut._log.info(f"MLP weight loading completed")
    dut._log.info(f"State: {dut.state.value}, Current layer: {dut.current_layer.value}")


@cocotb.test()
async def test_mlp_activation_loading(dut):
    """Test activation data loading into MLP"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.wf_push_col0.value = 0
    dut.wf_push_col1.value = 0
    dut.wf_push_col2.value = 0
    dut.wf_data_in.value = 0
    dut.wf_reset.value = 0
    dut.init_act_valid.value = 0
    dut.init_act_data.value = 0
    dut.start_mlp.value = 0
    dut.weights_ready.value = 1  # Weights ready
    dut.norm_gain.value = 256
    dut.norm_bias.value = 0
    dut.norm_shift.value = 0
    dut.q_inv_scale.value = 256
    dut.q_zero_point.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Load activation data (24-bit for 3x8-bit values)
    # Example: [5, 6, 7] as 24-bit value
    activation_data = (5 << 16) | (6 << 8) | 7  # 0x050607

    dut.init_act_valid.value = 1
    dut.init_act_data.value = activation_data

    await ClockCycles(dut.clk, 2)
    await Timer(1, units='ns')

    dut.init_act_valid.value = 0

    dut._log.info(f"MLP activation loading completed with data: 0x{activation_data:06X}")
    dut._log.info(f"State: {dut.state.value}")


@cocotb.test()
async def test_mlp_basic_operation(dut):
    """Test basic MLP operation flow"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize all inputs
    dut.rst_n.value = 0
    dut.wf_push_col0.value = 0
    dut.wf_push_col1.value = 0
    dut.wf_push_col2.value = 0
    dut.wf_data_in.value = 0
    dut.wf_reset.value = 0
    dut.init_act_valid.value = 0
    dut.init_act_data.value = 0
    dut.start_mlp.value = 0
    dut.weights_ready.value = 0
    dut.norm_gain.value = 256  # 1.0
    dut.norm_bias.value = 0
    dut.norm_shift.value = 0
    dut.q_inv_scale.value = 256  # 1.0
    dut.q_zero_point.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Load some weights
    dut.weights_ready.value = 1
    dut.wf_push_col0.value = 1
    dut.wf_data_in.value = 1  # Simple weight
    await ClockCycles(dut.clk, 2)
    dut.wf_push_col0.value = 0

    # Load activation data
    dut.init_act_valid.value = 1
    dut.init_act_data.value = (1 << 16) | (2 << 8) | 3  # [1, 2, 3]
    await ClockCycles(dut.clk, 2)
    dut.init_act_valid.value = 0

    # Start MLP computation
    dut.start_mlp.value = 1
    await ClockCycles(dut.clk, 1)
    dut.start_mlp.value = 0

    # Let it run for some cycles
    for i in range(20):
        await ClockCycles(dut.clk, 1)
        if i % 5 == 0:  # Log every 5 cycles
            dut._log.info(f"Cycle {i}: State={dut.state.value}, Layer={dut.current_layer.value}, "
                         f"Complete={dut.layer_complete.value}, CycleCnt={dut.cycle_cnt.value}")

    dut._log.info("MLP basic operation test completed")
    dut._log.info(f"Final state: {dut.state.value}, Final layer: {dut.current_layer.value}")
    dut._log.info(f"Accumulator values: acc0={dut.acc0.value}, acc1={dut.acc1.value}, acc2={dut.acc2.value}")
