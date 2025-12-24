#!/usr/bin/env python3
"""
Instruction Encoder/Decoder Utilities
Helper functions for encoding and decoding 32-bit TPU instructions
"""

from typing import Tuple, List, Optional
from enum import IntEnum
import struct

class Opcode(IntEnum):
    """ISA Opcodes"""
    NOP          = 0x00
    RD_HOST_MEM  = 0x01
    WR_HOST_MEM  = 0x02
    RD_WEIGHT    = 0x03
    LD_UB        = 0x04
    ST_UB        = 0x05
    MATMUL       = 0x10
    CONV2D       = 0x11
    MATMUL_ACC   = 0x12
    RELU         = 0x18
    RELU6        = 0x19
    SIGMOID      = 0x1A
    TANH         = 0x1B
    MAXPOOL      = 0x20
    AVGPOOL      = 0x21
    ADD_BIAS     = 0x22
    BATCH_NORM   = 0x23
    SYNC         = 0x30
    CFG_REG      = 0x31
    HALT         = 0x3F

def encode_instruction(opcode: int, arg1: int, arg2: int, arg3: int, flags: int = 0) -> int:
    """
    Encode instruction to 32-bit word
    
    Format: [31:26] opcode, [25:18] arg1, [17:10] arg2, [9:2] arg3, [1:0] flags
    
    Args:
        opcode: 6-bit opcode (0-63)
        arg1: 8-bit argument (0-255)
        arg2: 8-bit argument (0-255)
        arg3: 8-bit argument (0-255)
        flags: 2-bit flags (0-3)
    
    Returns:
        32-bit instruction word
    """
    return (opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags

def decode_instruction(word: int) -> Tuple[int, int, int, int, int]:
    """
    Decode 32-bit word to instruction components
    
    Returns:
        (opcode, arg1, arg2, arg3, flags)
    """
    opcode = (word >> 26) & 0x3F
    arg1 = (word >> 18) & 0xFF
    arg2 = (word >> 10) & 0xFF
    arg3 = (word >> 2) & 0xFF
    flags = word & 0x03
    return (opcode, arg1, arg2, arg3, flags)

def instruction_to_bytes(word: int) -> bytes:
    """Convert 32-bit instruction to 4-byte big-endian format"""
    return struct.pack('>I', word)

def bytes_to_instruction(data: bytes) -> int:
    """Convert 4-byte big-endian format to 32-bit instruction"""
    return struct.unpack('>I', data)[0]

def format_instruction(word: int) -> str:
    """Format instruction as human-readable string"""
    opcode, arg1, arg2, arg3, flags = decode_instruction(word)
    
    try:
        opcode_name = Opcode(opcode).name
    except ValueError:
        opcode_name = f"0x{opcode:02X}"
    
    return (f"{opcode_name}(arg1=0x{arg1:02X}, arg2=0x{arg2:02X}, "
            f"arg3=0x{arg3:02X}, flags=0b{flags:02b})")

# ============================================================================
# INSTRUCTION BUILDERS (High-level helpers)
# ============================================================================

def build_nop() -> int:
    """Build NOP instruction"""
    return encode_instruction(Opcode.NOP, 0, 0, 0, 0)

def build_rd_host_mem(ub_addr: int, length: int, elem_size: int = 0, burst: bool = False) -> int:
    """Build RD_HOST_MEM instruction"""
    flags = elem_size | ((1 if burst else 0) << 1)
    return encode_instruction(Opcode.RD_HOST_MEM, ub_addr, length >> 8, length & 0xFF, flags)

def build_wr_host_mem(ub_addr: int, length: int, elem_size: int = 0) -> int:
    """Build WR_HOST_MEM instruction"""
    return encode_instruction(Opcode.WR_HOST_MEM, ub_addr, length >> 8, length & 0xFF, elem_size)

def build_rd_weight(start_tile: int, num_tiles: int, buffer_sel: int = 0) -> int:
    """Build RD_WEIGHT instruction"""
    return encode_instruction(Opcode.RD_WEIGHT, start_tile, num_tiles, 0, buffer_sel)

def build_ld_ub(ub_addr: int, num_blocks: int) -> int:
    """Build LD_UB instruction"""
    return encode_instruction(Opcode.LD_UB, ub_addr, num_blocks, 0, 0)

def build_st_ub(ub_addr: int, num_blocks: int) -> int:
    """Build ST_UB instruction"""
    return encode_instruction(Opcode.ST_UB, ub_addr, num_blocks, 0, 0)

def build_matmul(ub_addr: int, acc_addr: int, rows: int, 
                transpose: bool = False, signed: bool = False) -> int:
    """Build MATMUL instruction"""
    flags = (1 if transpose else 0) | ((1 if signed else 0) << 1)
    return encode_instruction(Opcode.MATMUL, ub_addr, acc_addr, rows, flags)

def build_conv2d(ub_addr: int, acc_addr: int, config: int, 
                padding: int = 0, signed: bool = False) -> int:
    """Build CONV2D instruction"""
    flags = padding | ((1 if signed else 0) << 1)
    return encode_instruction(Opcode.CONV2D, ub_addr, acc_addr, config, flags)

def build_matmul_acc(ub_addr: int, acc_addr: int, rows: int,
                    transpose: bool = False, signed: bool = False) -> int:
    """Build MATMUL_ACC instruction"""
    flags = (1 if transpose else 0) | ((1 if signed else 0) << 1)
    return encode_instruction(Opcode.MATMUL_ACC, ub_addr, acc_addr, rows, flags)

def build_relu(acc_addr: int, ub_out_addr: int, length: int, leak_factor: bool = False) -> int:
    """Build RELU instruction"""
    flags = 1 if leak_factor else 0
    return encode_instruction(Opcode.RELU, acc_addr, ub_out_addr, length, flags)

def build_relu6(acc_addr: int, ub_out_addr: int, length: int, leak_factor: bool = False) -> int:
    """Build RELU6 instruction"""
    flags = 1 if leak_factor else 0
    return encode_instruction(Opcode.RELU6, acc_addr, ub_out_addr, length, flags)

def build_sigmoid(acc_addr: int, ub_out_addr: int, length: int, leak_factor: bool = False) -> int:
    """Build SIGMOID instruction"""
    flags = 1 if leak_factor else 0
    return encode_instruction(Opcode.SIGMOID, acc_addr, ub_out_addr, length, flags)

def build_tanh(acc_addr: int, ub_out_addr: int, length: int, leak_factor: bool = False) -> int:
    """Build TANH instruction"""
    flags = 1 if leak_factor else 0
    return encode_instruction(Opcode.TANH, acc_addr, ub_out_addr, length, flags)

def build_maxpool(ub_in_addr: int, ub_out_addr: int, flags: int = 0) -> int:
    """Build MAXPOOL instruction"""
    return encode_instruction(Opcode.MAXPOOL, ub_in_addr, ub_out_addr, 0, flags)

def build_avgpool(ub_in_addr: int, ub_out_addr: int, flags: int = 0) -> int:
    """Build AVGPOOL instruction"""
    return encode_instruction(Opcode.AVGPOOL, ub_in_addr, ub_out_addr, 0, flags)

def build_add_bias(acc_addr: int, ub_bias_addr: int, length: int) -> int:
    """Build ADD_BIAS instruction"""
    return encode_instruction(Opcode.ADD_BIAS, acc_addr, ub_bias_addr, length, 0)

def build_batch_norm(ub_in_addr: int, ub_out_addr: int, cfg_base: int) -> int:
    """Build BATCH_NORM instruction"""
    return encode_instruction(Opcode.BATCH_NORM, ub_in_addr, ub_out_addr, cfg_base, 0)

def build_sync(mask: int, timeout: int) -> int:
    """Build SYNC instruction"""
    return encode_instruction(Opcode.SYNC, mask, timeout >> 8, timeout & 0xFF, 0)

def build_cfg_reg(reg_addr: int, data: int) -> int:
    """Build CFG_REG instruction"""
    return encode_instruction(Opcode.CFG_REG, reg_addr, data >> 8, data & 0xFF, 0)

def build_halt() -> int:
    """Build HALT instruction"""
    return encode_instruction(Opcode.HALT, 0, 0, 0, 0)

# ============================================================================
# PROGRAM ASSEMBLY
# ============================================================================

def assemble_program(instructions: List[Tuple]) -> List[int]:
    """
    Assemble list of instruction tuples into 32-bit words
    
    Args:
        instructions: List of (opcode, arg1, arg2, arg3, flags) tuples
    
    Returns:
        List of 32-bit instruction words
    """
    program = []
    for instr in instructions:
        if len(instr) == 4:
            opcode, arg1, arg2, arg3 = instr
            flags = 0
        else:
            opcode, arg1, arg2, arg3, flags = instr
        
        word = encode_instruction(opcode, arg1, arg2, arg3, flags)
        program.append(word)
    
    return program

def disassemble_program(program: List[int]) -> List[str]:
    """
    Disassemble list of 32-bit words to human-readable strings
    
    Args:
        program: List of 32-bit instruction words
    
    Returns:
        List of formatted instruction strings
    """
    return [format_instruction(word) for word in program]

# ============================================================================
# EXAMPLE
# ============================================================================

if __name__ == '__main__':
    print("Instruction Encoder/Decoder Example")
    print("="*60)
    
    # Example: Build a simple program
    program = [
        (Opcode.NOP, 0, 0, 0, 0),
        (Opcode.CFG_REG, 0, 0x06, 0x66, 0),
        (Opcode.MATMUL, 0, 0x20, 4, 0),
        (Opcode.SYNC, 0x01, 0x00, 0x10, 0),
        (Opcode.RELU, 0x20, 0x40, 4, 0),
        (Opcode.HALT, 0, 0, 0, 0)
    ]
    
    # Assemble
    words = assemble_program(program)
    print("\nAssembled program:")
    for i, word in enumerate(words):
        print(f"  [{i:2d}] 0x{word:08X} = {format_instruction(word)}")
    
    # Disassemble
    print("\nDisassembled program:")
    disasm = disassemble_program(words)
    for i, instr_str in enumerate(disasm):
        print(f"  [{i:2d}] {instr_str}")

