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
async def test_pe_weight_loading(dut):
    """Test weight loading through psum path (tinytinyTPU style)"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # First load weight
    dut.rst_n.value = 1
    dut.en_weight_pass.value = 1  # Enable weight pass mode
    dut.en_weight_capture.value = 1
    dut.act_in.value = 0
    dut.psum_in.value = 0x05  # Load weight = 5

    await ClockCycles(dut.clk, 1)

    # Check that activation output is reset during weight loading
    assert dut.act_out.value == 0, f"act_out should be 0 during weight loading, got {dut.act_out.value}"
    assert dut.psum_out.value == 0x05, f"psum_out should pass weight value, got {dut.psum_out.value}"


@cocotb.test()
async def test_pe_mac_operation(dut):
    """Test MAC operation: psum_out = act_in * weight + psum_in"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # First load weight
    dut.rst_n.value = 1
    dut.en_weight_pass.value = 1
    dut.en_weight_capture.value = 1
    dut.act_in.value = 0
    dut.psum_in.value = 0x05  # Load weight = 5

    await ClockCycles(dut.clk, 1)

    # Now perform MAC operation
    dut.en_weight_pass.value = 0  # Exit weight loading mode
    dut.en_weight_capture.value = 0
    dut.act_in.value = 0x0A      # Activation = 10
    dut.psum_in.value = 0         # Initial sum = 0

    await ClockCycles(dut.clk, 1)

    # Check result: 10 * 5 + 0 = 50
    expected = 10 * 5 + 0
    assert dut.act_out.value == 0x0A, f"act_out should pass activation, got {dut.act_out.value}"
    assert dut.psum_out.value == expected, f"psum_out should be {expected}, got {dut.psum_out.value}"

    # Test with non-zero initial sum
    dut.act_in.value = 0x03      # Activation = 3
    dut.psum_in.value = 32        # Initial sum = 32

    await ClockCycles(dut.clk, 1)

    # Check result: 3 * 5 + 32 = 47
    expected = 3 * 5 + 32
    assert dut.act_out.value == 0x03, f"act_out should pass activation, got {dut.act_out.value}"
    assert dut.psum_out.value == expected, f"psum_out should be {expected}, got {dut.psum_out.value}"


@cocotb.test()
async def test_pe_activation_flow(dut):
    """Test that activation flows through systolic array"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 1
    dut.en_weight_pass.value = 0
    dut.en_weight_capture.value = 0
    dut.act_in.value = 0xFF  # Test value
    dut.psum_in.value = 100

    await ClockCycles(dut.clk, 1)

    # Check that activation flows through unchanged
    assert dut.act_out.value == 0xFF, f"act_out should equal act_in, got {dut.act_out.value}"
    assert dut.psum_out.value == (0xFF * 0) + 100, f"MAC with zero weight should pass psum_in, got {dut.psum_out.value}"
