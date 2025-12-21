"""
Processing Element (PE) testbench for 3x3 TPU
Based on tinytinyTPU but adapted for our design
Tests weight loading, MAC operation, and activation propagation
"""
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, ClockCycles
import os
import shutil


@cocotb.test()
async def test_pe_reset(dut):
    """Test that reset initializes all outputs to zero"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Apply reset (active low in our design)
    dut.rst_n.value = 0
    dut.en_weight_pass.value = 0
    dut.en_weight_capture.value = 0
    dut.act_in.value = 0
    dut.psum_in.value = 0

    await ClockCycles(dut.clk, 2)

    # Release reset
    dut.rst_n.value = 1
    await ClockCycles(dut.clk, 1)

    # Check outputs are zero after reset
    assert dut.act_out.value == 0, f"act_out should be 0 after reset, got {dut.act_out.value}"
    assert dut.psum_out.value == 0, f"psum_out should be 0 after reset, got {dut.psum_out.value}"


@cocotb.test()
async def test_pe_simple(dut):
    """Simple test of PE basic functionality"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # CRITICAL: Force ALL signals to known state at start of test
    # cocotb doesn't reset signals between tests!
    dut.rst_n.value = 0
    dut.en_weight_pass.value = 0
    dut.en_weight_capture.value = 0
    dut.act_in.value = 0
    dut.psum_in.value = 0

    dut._log.info("=== STARTING test_pe_simple ===")
    await ClockCycles(dut.clk, 2)

    # Test 1: Reset works
    assert dut.act_out.value == 0, f"Reset failed: act_out = {dut.act_out.value}"
    assert dut.psum_out.value == 0, f"Reset failed: psum_out = {dut.psum_out.value}"

    # Test 2: Basic compute operation
    dut._log.info("=== SETTING COMPUTE MODE ===")
    dut.rst_n.value = 1
    dut.en_weight_pass.value = 0  # Explicitly set compute mode
    dut.en_weight_capture.value = 0
    dut.act_in.value = 10
    dut.psum_in.value = 5

    # Wait a tiny bit for signals to settle, then clock
    await cocotb.triggers.Timer(1, units='ns')
    dut._log.info(f"Before compute clock: rst_n={dut.rst_n.value}, en_weight_pass={dut.en_weight_pass.value}, act_in={dut.act_in.value}, psum_in={dut.psum_in.value}")
    await ClockCycles(dut.clk, 1)
    # Wait for signals to propagate
    await cocotb.triggers.Timer(1, units='ns')
    dut._log.info(f"After compute clock + delay: rst_n={dut.rst_n.value}, act_out={dut.act_out.value}, psum_out={dut.psum_out.value}")

    # In compute mode, act_out should equal act_in
    assert dut.act_out.value == 10, f"act_out should pass act_in, got {dut.act_out.value}"
    # MAC result: psum_out = psum_in + (act_in * weight_reg) = 5 + (10 * 0) = 5
    assert dut.psum_out.value == 5, f"psum_out should be 5, got {dut.psum_out.value}"


@cocotb.test()
async def test_pe_mac_operation(dut):
    """Test MAC operation: psum_out = act_in * weight + psum_in"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # CRITICAL: Force ALL signals to known state at start of test
    dut.rst_n.value = 0
    dut.en_weight_pass.value = 0
    dut.en_weight_capture.value = 0
    dut.act_in.value = 0
    dut.psum_in.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # First load weight
    dut.en_weight_pass.value = 1
    dut.en_weight_capture.value = 1
    dut.act_in.value = 0
    dut.psum_in.value = 5  # Load weight = 5

    await ClockCycles(dut.clk, 1)
    await cocotb.triggers.Timer(1, units='ns')  # Let signals settle
    dut._log.info(f"After weight load: act_out={dut.act_out.value}, psum_out={dut.psum_out.value}")

    # Now perform MAC operation
    dut.en_weight_pass.value = 0  # Exit weight loading mode
    dut.en_weight_capture.value = 0
    dut.act_in.value = 10         # Activation = 10
    dut.psum_in.value = 0         # Initial sum = 0

    await ClockCycles(dut.clk, 1)
    await cocotb.triggers.Timer(1, units='ns')  # Let signals settle

    # Check result: 10 * 5 + 0 = 50
    expected = 10 * 5 + 0
    dut._log.info(f"After MAC: act_out={dut.act_out.value}, psum_out={dut.psum_out.value}, expected_psum={expected}")
    assert dut.act_out.value == 10, f"act_out should pass activation, got {dut.act_out.value}"
    assert dut.psum_out.value == expected, f"psum_out should be {expected}, got {dut.psum_out.value}"

    # Test with non-zero initial sum
    dut.act_in.value = 3         # Activation = 3
    dut.psum_in.value = 32       # Initial sum = 32

    await ClockCycles(dut.clk, 1)
    await cocotb.triggers.Timer(1, units='ns')  # Let signals settle

    # Check result: 3 * 5 + 32 = 47
    expected = 3 * 5 + 32
    dut._log.info(f"Second MAC: act_out={dut.act_out.value}, psum_out={dut.psum_out.value}, expected={expected}")
    assert dut.act_out.value == 3, f"act_out should pass activation, got {dut.act_out.value}"
    assert dut.psum_out.value == expected, f"psum_out should be {expected}, got {dut.psum_out.value}"


@cocotb.test()
async def test_pe_activation_flow(dut):
    """Test that activation flows through systolic array"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # CRITICAL: Force ALL signals to known state at start of test
    dut.rst_n.value = 0
    dut.en_weight_pass.value = 0
    dut.en_weight_capture.value = 0
    dut.act_in.value = 0
    dut.psum_in.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Test activation flow
    dut.en_weight_pass.value = 0
    dut.en_weight_capture.value = 0
    dut.act_in.value = 255  # Test value (0xFF)
    dut.psum_in.value = 100

    await ClockCycles(dut.clk, 1)
    await cocotb.triggers.Timer(1, units='ns')  # Let signals settle

    # Check that activation flows through unchanged
    dut._log.info(f"Activation flow: act_in={dut.act_in.value}, act_out={dut.act_out.value}, psum_in={dut.psum_in.value}, psum_out={dut.psum_out.value}")
    assert dut.act_out.value == 255, f"act_out should equal act_in, got {dut.act_out.value}"
    assert dut.psum_out.value == (255 * 0) + 100, f"MAC with zero weight should pass psum_in, got {dut.psum_out.value}"
