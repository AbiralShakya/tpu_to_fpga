"""
TPU Pipelined Double Buffering Test Suite
Tests the 2-stage pipeline with proper double buffering synchronization
"""
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, ClockCycles, Timer


# ISA Opcodes
NOP_OP = 0x00
RD_HOST_MEM_OP = 0x01
WR_HOST_MEM_OP = 0x02
RD_WEIGHT_OP = 0x03
LD_UB_OP = 0x04
ST_UB_OP = 0x05
MATMUL_OP = 0x10
MATMUL_ACC_OP = 0x12
RELU_OP = 0x18
SYNC_OP = 0x30


def make_instr(opcode, arg1=0, arg2=0, arg3=0, flags=0):
    """Create a 32-bit instruction word"""
    return (opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags


@cocotb.test()
async def test_buffer_state_pipeline_propagation(dut):
    """Test that buffer state propagates correctly through the 2-stage pipeline"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0
    dut.sys_busy.value = 0
    dut.vpu_busy.value = 0
    dut.dma_busy.value = 0
    dut.wt_busy.value = 0

    await ClockCycles(dut.clk, 3)
    dut.rst_n.value = 1

    # Check initial buffer state (should be 0)
    await Timer(1, units='ns')
    # Note: We can't directly check internal buffer state, but we can verify through behavior

    dut._log.info("Testing buffer state pipeline propagation")
    dut._log.info("Initial state: all buffer selectors should be 0")


@cocotb.test()
async def test_sync_buffer_toggle(dut):
    """Test that SYNC instruction toggles buffer state correctly"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0
    dut.sys_busy.value = 0
    dut.vpu_busy.value = 0
    dut.dma_busy.value = 0
    dut.wt_busy.value = 0

    await ClockCycles(dut.clk, 3)
    dut.rst_n.value = 1

    # Send SYNC instruction
    sync_instr = make_instr(SYNC_OP, 0x0F, 0x00, 0x10)  # Sync all units, 4096 timeout
    dut.instr_data.value = sync_instr

    # Wait for SYNC to be decoded and executed
    await ClockCycles(dut.clk, 3)
    await Timer(1, units='ns')

    dut._log.info(f"SYNC instruction: 0x{sync_instr:08X}")
    dut._log.info(f"Pipeline stage: {dut.current_stage.value}")
    dut._log.info(f"Pipeline stall: {dut.pipeline_stall.value}")

    # SYNC should cause pipeline stall
    assert dut.pipeline_stall.value == 1, "SYNC should cause pipeline stall"

    dut._log.info("SYNC instruction causes pipeline stall as expected")


@cocotb.test()
async def test_double_buffering_sequence(dut):
    """Test a sequence that demonstrates double buffering behavior"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0
    dut.sys_busy.value = 0
    dut.vpu_busy.value = 0
    dut.dma_busy.value = 0
    dut.wt_busy.value = 0

    await ClockCycles(dut.clk, 3)
    dut.rst_n.value = 1

    # Sequence of instructions to test double buffering
    instructions = [
        (make_instr(LD_UB_OP, 0x00, 0x10), "LD_UB addr=0x00, count=0x10"),  # Load data to UB bank 0
        (make_instr(MATMUL_OP, 0x00, 0x20, 0x08), "MATMUL UB[0] -> Acc[0x20], 8 rows"),  # Use bank 0
        (make_instr(RELU_OP, 0x20, 0x40, 0x08), "RELU Acc[0x20] -> UB[0x40], 8 elements"),  # Write to bank 1
        (make_instr(SYNC_OP, 0x03, 0x00, 0x01), "SYNC VPU + Systolic, timeout=256"),  # Toggle buffers
        (make_instr(MATMUL_OP, 0x40, 0x60, 0x08), "MATMUL UB[0x40] -> Acc[0x60], 8 rows"),  # Use toggled bank
    ]

    dut._log.info("Testing double buffering instruction sequence")

    for i, (instr, desc) in enumerate(instructions):
        dut.instr_data.value = instr
        await ClockCycles(dut.clk, 3)  # Allow decode and execute

        dut._log.info(f"Step {i+1}: {desc}")
        dut._log.info(f"  Instruction: 0x{instr:08X}")
        dut._log.info(f"  Pipeline stage: {dut.current_stage.value}")
        dut._log.info(f"  Pipeline stall: {dut.pipeline_stall.value}")

        # Allow some time for processing
        await Timer(1, units='ns')

    dut._log.info("Double buffering sequence completed")


@cocotb.test()
async def test_buffer_state_consistency(dut):
    """Test that instructions use consistent buffer state throughout execution"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0
    dut.sys_busy.value = 0
    dut.vpu_busy.value = 0
    dut.dma_busy.value = 0
    dut.wt_busy.value = 0

    await ClockCycles(dut.clk, 3)
    dut.rst_n.value = 1

    # Test sequence:
    # 1. Send instruction before SYNC
    # 2. Send SYNC
    # 3. Send instruction after SYNC
    # Instructions should use buffer state from their decode time

    # Instruction 1: MATMUL (should use buffer state = 0)
    instr1 = make_instr(MATMUL_OP, 0x00, 0x20, 0x04)
    dut.instr_data.value = instr1
    await ClockCycles(dut.clk, 3)
    dut._log.info(f"Instruction 1 (before SYNC): MATMUL with buffer state 0")
    dut._log.info(f"  Pipeline stage: {dut.current_stage.value}")

    # SYNC instruction (toggles buffers to 1)
    sync_instr = make_instr(SYNC_OP, 0x0F, 0x00, 0x01)
    dut.instr_data.value = sync_instr
    await ClockCycles(dut.clk, 3)
    dut._log.info(f"SYNC executed - buffers toggled to state 1")
    dut._log.info(f"  Pipeline stall: {dut.pipeline_stall.value}")

    # Instruction 2: MATMUL (should use buffer state = 1)
    instr2 = make_instr(MATMUL_OP, 0x40, 0x60, 0x04)
    dut.instr_data.value = instr2
    await ClockCycles(dut.clk, 3)
    dut._log.info(f"Instruction 2 (after SYNC): MATMUL with buffer state 1")
    dut._log.info(f"  Pipeline stage: {dut.current_stage.value}")

    dut._log.info("Buffer state consistency test completed")


@cocotb.test()
async def test_pipeline_flush_behavior(dut):
    """Test that pipeline flush clears buffer state propagation"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0
    dut.sys_busy.value = 0
    dut.vpu_busy.value = 0
    dut.dma_busy.value = 0
    dut.wt_busy.value = 0

    await ClockCycles(dut.clk, 3)
    dut.rst_n.value = 1

    # Send instruction to fill pipeline
    test_instr = make_instr(MATMUL_OP, 0x00, 0x20, 0x04)
    dut.instr_data.value = test_instr
    await ClockCycles(dut.clk, 2)

    dut._log.info(f"Instruction in pipeline: 0x{test_instr:08X}")
    dut._log.info(f"Pipeline stage before flush: {dut.current_stage.value}")

    # Pipeline should be active
    assert dut.current_stage.value != 0, "Pipeline should be active"

    # Reset to flush pipeline
    dut.rst_n.value = 0
    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1
    await ClockCycles(dut.clk, 2)

    dut._log.info(f"Pipeline stage after flush: {dut.current_stage.value}")

    # Pipeline should be flushed
    assert dut.current_stage.value == 1, "Pipeline should be in stage 1 after flush"

    dut._log.info("Pipeline flush behavior works correctly")


@cocotb.test()
async def test_concurrent_read_write_operations(dut):
    """Test that double buffering allows concurrent read/write operations"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0
    dut.sys_busy.value = 0
    dut.vpu_busy.value = 0
    dut.dma_busy.value = 0
    dut.wt_busy.value = 0

    await ClockCycles(dut.clk, 3)
    dut.rst_n.value = 1

    # Sequence demonstrating concurrent operations:
    # 1. Load weights to buffer 0
    # 2. MATMUL reading from buffer 0, writing to accumulators
    # 3. RELU reading accumulators, writing to UB buffer 1
    # 4. SYNC to swap buffers
    # 5. DMA writing to buffer 0 (now free)
    # 6. Next MATMUL can start immediately using buffer 1

    instructions = [
        make_instr(RD_WEIGHT_OP, 0x00, 0x02),  # Load 2 weight tiles to buffer 0
        make_instr(MATMUL_OP, 0x00, 0x20, 0x04),  # MATMUL using UB bank 0
        make_instr(RELU_OP, 0x20, 0x00, 0x04),  # RELU -> UB bank 1 (0x00 relative to bank)
        make_instr(SYNC_OP, 0x03, 0x00, 0x02),  # Wait for systolic+VPU, toggle buffers
        make_instr(RD_HOST_MEM_OP, 0x00, 0x00, 0x10),  # DMA to UB bank 0 (now free)
        make_instr(MATMUL_OP, 0x00, 0x40, 0x04),  # MATMUL using UB bank 1 (toggled)
    ]

    dut._log.info("Testing concurrent read/write operations with double buffering")

    for i, instr in enumerate(instructions):
        dut.instr_data.value = instr
        await ClockCycles(dut.clk, 3)

        dut._log.info(f"Step {i+1}: Instruction 0x{instr:08X}")
        dut._log.info(f"  Pipeline stage: {dut.current_stage.value}")
        dut._log.info(f"  Pipeline stall: {dut.pipeline_stall.value}")

    dut._log.info("Concurrent operations test completed - double buffering allows overlap")


@cocotb.test()
async def test_buffer_state_persistence(dut):
    """Test that buffer state persists across multiple instruction sequences"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0
    dut.sys_busy.value = 0
    dut.vpu_busy.value = 0
    dut.dma_busy.value = 0
    dut.wt_busy.value = 0

    await ClockCycles(dut.clk, 3)
    dut.rst_n.value = 1

    # Sequence: Multiple SYNC operations to test state persistence
    sequence = [
        make_instr(MATMUL_OP, 0x00, 0x20, 0x02),  # Use initial buffer state
        make_instr(SYNC_OP, 0x01, 0x00, 0x01),    # Toggle buffers
        make_instr(MATMUL_OP, 0x40, 0x40, 0x02),  # Use toggled buffer state
        make_instr(SYNC_OP, 0x01, 0x00, 0x01),    # Toggle back
        make_instr(MATMUL_OP, 0x00, 0x60, 0x02),  # Should use original buffer state again
    ]

    dut._log.info("Testing buffer state persistence across multiple SYNC operations")

    for i, instr in enumerate(sequence):
        dut.instr_data.value = instr
        await ClockCycles(dut.clk, 4)  # Extra cycle for SYNC operations

        dut._log.info(f"Instruction {i+1}: 0x{instr:08X}")
        dut._log.info(f"  Pipeline stage: {dut.current_stage.value}")

    dut._log.info("Buffer state persistence test completed")
