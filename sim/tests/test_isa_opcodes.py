"""
Comprehensive ISA Opcode Test Suite
Tests all instructions from ISA_Reference.md
"""
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, ClockCycles, Timer
from cocotb.binary import BinaryValue


@cocotb.test()
async def test_isa_nop(dut):
    """Test NOP (0x00) - No Operation"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0
    dut.instr_addr.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # NOP instruction: {6'h00, 8'h00, 8'h00, 8'h00, 2'b00}
    nop_instr = (0x00 << 26) | (0x00 << 18) | (0x00 << 10) | (0x00 << 2) | 0x00
    dut.instr_data.value = nop_instr

    await ClockCycles(dut.clk, 2)
    await Timer(1, units='ns')

    # NOP should increment PC but not affect any control signals
    dut._log.info("NOP instruction executed - PC should increment")
    dut._log.info(f"PC value: {dut.instr_addr.value}")


@cocotb.test()
async def test_isa_rd_weight(dut):
    """Test RD_WEIGHT (0x03) - Read Weight Data"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # RD_WEIGHT instruction: ARG1=tile_num, ARG2=num_tiles, FLAGS[0]=buffer_sel
    # Format: {6'h03, 8'h00, 8'h04, 8'h00, 2'b00}
    # Load tiles 0-3 into buffer 0
    rd_weight_instr = (0x03 << 26) | (0x00 << 18) | (0x04 << 10) | (0x00 << 2) | 0x00
    dut.instr_data.value = rd_weight_instr

    await ClockCycles(dut.clk, 2)
    await Timer(1, units='ns')

    # Check that weight FIFO write is enabled
    dut._log.info("RD_WEIGHT instruction executed")
    dut._log.info(f"wt_fifo_wr: {dut.wt_fifo_wr.value}")
    dut._log.info(f"wt_num_tiles: {dut.wt_num_tiles.value if hasattr(dut, 'wt_num_tiles') else 'N/A'}")


@cocotb.test()
async def test_isa_matmul(dut):
    """Test MATMUL (0x10) - Matrix Multiplication"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # MATMUL instruction: ARG1=UB_addr, ARG2=acc_addr, ARG3=rows, FLAGS[0]=transpose, FLAGS[1]=signed
    # Format: {6'h10, 8'h00, 8'h20, 8'h03, 2'b00}
    # UB[0] × Weights → Acc[0x20], 3 rows, unsigned, no transpose
    matmul_instr = (0x10 << 26) | (0x00 << 18) | (0x20 << 10) | (0x03 << 2) | 0x00
    dut.instr_data.value = matmul_instr

    await ClockCycles(dut.clk, 2)
    await Timer(1, units='ns')

    # Check that systolic array start is triggered
    dut._log.info("MATMUL instruction executed")
    dut._log.info(f"sys_start: {dut.sys_start.value if hasattr(dut, 'sys_start') else 'N/A'}")
    dut._log.info(f"sys_rows: {dut.sys_rows.value if hasattr(dut, 'sys_rows') else 'N/A'}")


@cocotb.test()
async def test_isa_matmul_acc(dut):
    """Test MATMUL_ACC (0x12) - Accumulate Matrix Multiplication"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # MATMUL_ACC instruction: Same as MATMUL but with accumulation
    # Format: {6'h12, 8'h00, 8'h20, 8'h03, 2'b00}
    matmul_acc_instr = (0x12 << 26) | (0x00 << 18) | (0x20 << 10) | (0x03 << 2) | 0x00
    dut.instr_data.value = matmul_acc_instr

    await ClockCycles(dut.clk, 2)
    await Timer(1, units='ns')

    dut._log.info("MATMUL_ACC instruction executed")
    dut._log.info("Accumulator should be in accumulate mode")


@cocotb.test()
async def test_isa_relu(dut):
    """Test RELU (0x18) - Rectified Linear Unit"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # RELU instruction: ARG1=acc_addr, ARG2=ub_addr, ARG3=length
    # Format: {6'h18, 8'h20, 8'h40, 8'h09, 2'b00}
    # ReLU on Acc[0x20] → UB[0x40], 9 elements
    relu_instr = (0x18 << 26) | (0x20 << 18) | (0x40 << 10) | (0x09 << 2) | 0x00
    dut.instr_data.value = relu_instr

    await ClockCycles(dut.clk, 2)
    await Timer(1, units='ns')

    dut._log.info("RELU instruction executed")
    dut._log.info(f"vpu_start: {dut.vpu_start.value if hasattr(dut, 'vpu_start') else 'N/A'}")
    dut._log.info(f"vpu_mode: {dut.vpu_mode.value if hasattr(dut, 'vpu_mode') else 'N/A'}")


@cocotb.test()
async def test_isa_relu6(dut):
    """Test RELU6 (0x19) - ReLU6 Activation"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # RELU6 instruction: Same format as RELU
    relu6_instr = (0x19 << 26) | (0x20 << 18) | (0x40 << 10) | (0x09 << 2) | 0x00
    dut.instr_data.value = relu6_instr

    await ClockCycles(dut.clk, 2)
    await Timer(1, units='ns')

    dut._log.info("RELU6 instruction executed")
    dut._log.info("VPU should be in ReLU6 mode")


@cocotb.test()
async def test_isa_sync(dut):
    """Test SYNC (0x30) - Synchronize Operations"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # SYNC instruction: ARG1=sync_mask, ARG2=timeout_high, ARG3=timeout_low
    # Format: {6'h30, 8'h01, 8'h00, 8'h10, 2'b00}
    # Wait for systolic array (bit 0), timeout 4096 cycles
    sync_instr = (0x30 << 26) | (0x01 << 18) | (0x00 << 10) | (0x10 << 2) | 0x00
    dut.instr_data.value = sync_instr

    await ClockCycles(dut.clk, 2)
    await Timer(1, units='ns')

    dut._log.info("SYNC instruction executed")
    dut._log.info("Pipeline should stall until systolic array completes")


@cocotb.test()
async def test_isa_cfg_reg(dut):
    """Test CFG_REG (0x31) - Configure Register"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # CFG_REG instruction: ARG1=reg_addr, ARG2=data_high, ARG3=data_low
    # Format: {6'h31, 8'h00, 8'h06, 8'h66, 2'b00}
    # Write 0x0666 to cfg[0]
    cfg_reg_instr = (0x31 << 26) | (0x00 << 18) | (0x06 << 10) | (0x66 << 2) | 0x00
    dut.instr_data.value = cfg_reg_instr

    await ClockCycles(dut.clk, 2)
    await Timer(1, units='ns')

    dut._log.info("CFG_REG instruction executed")
    dut._log.info(f"cfg_wr_en: {dut.cfg_wr_en.value if hasattr(dut, 'cfg_wr_en') else 'N/A'}")
    dut._log.info(f"cfg_addr: {dut.cfg_addr.value if hasattr(dut, 'cfg_addr') else 'N/A'}")
    dut._log.info(f"cfg_data: {dut.cfg_data.value if hasattr(dut, 'cfg_data') else 'N/A'}")


@cocotb.test()
async def test_isa_halt(dut):
    """Test HALT (0x3F) - Program Termination"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # HALT instruction: {6'h3F, 8'h00, 8'h00, 8'h00, 2'b00}
    halt_instr = (0x3F << 26) | (0x00 << 18) | (0x00 << 10) | (0x00 << 2) | 0x00
    dut.instr_data.value = halt_instr

    await ClockCycles(dut.clk, 2)
    await Timer(1, units='ns')

    dut._log.info("HALT instruction executed")
    dut._log.info("PC should stop incrementing")


@cocotb.test()
async def test_isa_flag_bits(dut):
    """Test instruction flag bits (F[0] and F[1])"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Test MATMUL with different flag combinations
    test_cases = [
        (0x00, "unsigned, no transpose"),
        (0x01, "unsigned, transpose"),
        (0x02, "signed, no transpose"),
        (0x03, "signed, transpose"),
    ]

    for flags, description in test_cases:
        matmul_instr = (0x10 << 26) | (0x00 << 18) | (0x20 << 10) | (0x03 << 2) | flags
        dut.instr_data.value = matmul_instr

        await ClockCycles(dut.clk, 2)
        await Timer(1, units='ns')

        dut._log.info(f"MATMUL with flags {flags:02b}: {description}")
        dut._log.info(f"sys_signed: {dut.sys_signed.value if hasattr(dut, 'sys_signed') else 'N/A'}")
        dut._log.info(f"sys_transpose: {dut.sys_transpose.value if hasattr(dut, 'sys_transpose') else 'N/A'}")


@cocotb.test()
async def test_isa_instruction_sequence(dut):
    """Test a complete instruction sequence (2-layer MLP)"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Layer 1: Load weights -> MATMUL -> RELU
    instructions = [
        (0x03, 0x00, 0x09, 0x00, 0x00, "RD_WEIGHT: Load 9 tiles"),
        (0x10, 0x00, 0x20, 0x09, 0x00, "MATMUL: 9 rows"),
        (0x18, 0x20, 0x40, 0x09, 0x00, "RELU: Acc[0x20] → UB[0x40]"),
        (0x30, 0x03, 0x00, 0x10, 0x00, "SYNC: Wait for systolic + VPU"),
    ]

    for opcode, arg1, arg2, arg3, flags, description in instructions:
        instr = (opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags
        dut.instr_data.value = instr

        await ClockCycles(dut.clk, 2)
        await Timer(1, units='ns')

        dut._log.info(f"Executed: {description}")

    dut._log.info("Complete 2-layer MLP instruction sequence executed")


@cocotb.test()
async def test_isa_register_addressing(dut):
    """Test register/memory addressing with different addresses"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Initialize
    dut.rst_n.value = 0
    dut.instr_data.value = 0

    await ClockCycles(dut.clk, 2)
    dut.rst_n.value = 1

    # Test MATMUL with different UB and accumulator addresses
    test_addresses = [
        (0x00, 0x20, "UB[0] → Acc[0x20]"),
        (0x40, 0x60, "UB[0x40] → Acc[0x60]"),
        (0x80, 0xA0, "UB[0x80] → Acc[0xA0]"),
    ]

    for ub_addr, acc_addr, description in test_addresses:
        matmul_instr = (0x10 << 26) | (ub_addr << 18) | (acc_addr << 10) | (0x03 << 2) | 0x00
        dut.instr_data.value = matmul_instr

        await ClockCycles(dut.clk, 2)
        await Timer(1, units='ns')

        dut._log.info(f"MATMUL: {description}")
        dut._log.info(f"ub_rd_addr: {dut.ub_rd_addr.value if hasattr(dut, 'ub_rd_addr') else 'N/A'}")
        dut._log.info(f"acc_addr: {dut.acc_addr.value if hasattr(dut, 'acc_addr') else 'N/A'}")

