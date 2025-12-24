#!/usr/bin/env python3
"""
TPU Coprocessor Driver - Complete UART Streaming Interface
Handles 32-bit instruction streaming with double-buffering support
for all 20 ISA instructions on Basys 3 FPGA
"""

import serial
import numpy as np
import time
import struct
import threading
from enum import IntEnum
from typing import List, Tuple, Optional, Union
from dataclasses import dataclass

# ============================================================================
# OPCODE DEFINITIONS - All 20 Instructions
# ============================================================================

class Opcode(IntEnum):
    """ISA Opcodes (6-bit, bits 31:26 of instruction)"""
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

# ============================================================================
# UART COMMAND PROTOCOL
# ============================================================================

class UARTCommand(IntEnum):
    """UART DMA command codes"""
    WRITE_UB      = 0x01  # Write to Unified Buffer
    WRITE_WT      = 0x02  # Write to Weight Memory
    WRITE_INSTR   = 0x03  # Write instruction to buffer
    READ_UB       = 0x04  # Read from Unified Buffer
    EXECUTE       = 0x05  # Start execution
    READ_STATUS   = 0x06  # Read TPU status
    STREAM_INSTR  = 0x07  # Enter instruction streaming mode (double-buffered)

# ============================================================================
# INSTRUCTION ENCODING/DECODING
# ============================================================================

@dataclass
class Instruction:
    """32-bit TPU instruction"""
    opcode: int      # 6-bit opcode (bits 31:26)
    arg1: int        # 8-bit argument (bits 25:18)
    arg2: int        # 8-bit argument (bits 17:10)
    arg3: int        # 8-bit argument (bits 9:2)
    flags: int = 0   # 2-bit flags (bits 1:0)
    
    def encode(self) -> int:
        """Encode instruction to 32-bit word"""
        return (self.opcode << 26) | (self.arg1 << 18) | (self.arg2 << 10) | (self.arg3 << 2) | self.flags
    
    def to_bytes(self) -> bytes:
        """Convert to 4-byte big-endian format"""
        return struct.pack('>I', self.encode())
    
    @classmethod
    def decode(cls, word: int) -> 'Instruction':
        """Decode 32-bit word to instruction"""
        return cls(
            opcode=(word >> 26) & 0x3F,
            arg1=(word >> 18) & 0xFF,
            arg2=(word >> 10) & 0xFF,
            arg3=(word >> 2) & 0xFF,
            flags=word & 0x03
        )
    
    def __str__(self):
        opcode_name = Opcode(self.opcode).name if self.opcode in Opcode.__members__.values() else f"0x{self.opcode:02X}"
        return f"{opcode_name}(arg1=0x{self.arg1:02X}, arg2=0x{self.arg2:02X}, arg3=0x{self.arg3:02X}, flags=0b{self.flags:02b})"

# ============================================================================
# STATUS MONITORING
# ============================================================================

@dataclass
class TPUStatus:
    """TPU status register"""
    sys_busy: bool
    sys_done: bool
    vpu_busy: bool
    vpu_done: bool
    ub_busy: bool
    ub_done: bool
    
    @classmethod
    def from_byte(cls, status_byte: int) -> 'TPUStatus':
        """Parse status byte from FPGA"""
        return cls(
            sys_busy=bool(status_byte & 0x01),
            sys_done=bool(status_byte & 0x02),
            vpu_busy=bool(status_byte & 0x04),
            vpu_done=bool(status_byte & 0x08),
            ub_busy=bool(status_byte & 0x10),
            ub_done=bool(status_byte & 0x20)
        )
    
    def __str__(self):
        return (f"Status: sys_busy={self.sys_busy}, vpu_busy={self.vpu_busy}, "
                f"ub_busy={self.ub_busy}, sys_done={self.sys_done}, "
                f"vpu_done={self.vpu_done}, ub_done={self.ub_done}")

# ============================================================================
# MAIN COPROCESSOR DRIVER
# ============================================================================

class TPU_Coprocessor:
    """
    Complete TPU Coprocessor Driver with UART Streaming Support
    
    Features:
    - 32-bit instruction encoding/decoding
    - Double-buffered instruction streaming
    - All 20 ISA instructions supported
    - Flow control and backpressure handling
    - Status monitoring
    - Unified Buffer and Weight Memory access
    """
    
    def __init__(self, port='/dev/ttyUSB0', baud=115200, timeout=2, verbose=True):
        """
        Initialize TPU coprocessor connection
        
        Args:
            port: Serial port (e.g., '/dev/ttyUSB0' or 'COM3')
            baud: Baud rate (default 115200)
            timeout: Serial timeout in seconds
            verbose: Print status messages
        """
        self.port = port
        self.baud = baud
        self.timeout = timeout
        self.verbose = verbose
        
        # Serial connection
        self.ser = None
        self._stream_mode = False
        self._stream_buffer_sel = 0
        
        # Statistics
        self.stats = {
            'instructions_sent': 0,
            'instructions_executed': 0,
            'bytes_sent': 0,
            'bytes_received': 0,
            'errors': 0
        }
        
        self._connect()
    
    def _connect(self):
        """Establish serial connection"""
        try:
            self.ser = serial.Serial(self.port, self.baud, timeout=self.timeout)
            time.sleep(2)  # Wait for FPGA initialization
            if self.verbose:
                print(f"✓ Connected to TPU coprocessor on {self.port} at {self.baud} baud")
        except Exception as e:
            print(f"✗ Connection failed: {e}")
            raise
    
    def close(self):
        """Close serial connection"""
        if self.ser and self.ser.is_open:
            if self._stream_mode:
                self.disable_stream_mode()
            self.ser.close()
            if self.verbose:
                print("✓ Connection closed")
    
    # ========================================================================
    # INSTRUCTION ENCODING HELPERS
    # ========================================================================
    
    def encode_instruction(self, opcode: Union[int, Opcode], arg1: int, arg2: int, 
                          arg3: int, flags: int = 0) -> Instruction:
        """Create instruction object"""
        if isinstance(opcode, Opcode):
            opcode = opcode.value
        return Instruction(opcode, arg1, arg2, arg3, flags)
    
    # ========================================================================
    # UART PROTOCOL METHODS
    # ========================================================================
    
    def _send_command(self, cmd: int, *args: int) -> None:
        """Send UART command with arguments"""
        data = struct.pack('B' * (1 + len(args)), cmd, *args)
        self.ser.write(data)
        self.stats['bytes_sent'] += len(data)
    
    def _read_response(self, length: int) -> bytes:
        """Read response from FPGA"""
        data = self.ser.read(length)
        self.stats['bytes_received'] += len(data)
        return data
    
    # ========================================================================
    # UNIFIED BUFFER OPERATIONS
    # ========================================================================
    
    def write_ub(self, addr: int, data: Union[np.ndarray, bytes]) -> None:
        """
        Write data to Unified Buffer
        
        Args:
            addr: UB address (0-255)
            data: numpy array or bytes (padded to multiple of 32 bytes)
        """
        if isinstance(data, np.ndarray):
            data = data.tobytes()
        
        # Pad to multiple of 32 bytes
        if len(data) % 32 != 0:
            pad_len = 32 - (len(data) % 32)
            data = data + b'\x00' * pad_len
        
        length = len(data)
        
        # Send command: CMD, addr_hi, addr_lo, length_hi, length_lo
        self._send_command(UARTCommand.WRITE_UB, 0, addr, length >> 8, length & 0xFF)
        
        # Send data
        self.ser.write(data)
        self.stats['bytes_sent'] += length
        
        if self.verbose:
            print(f"✓ Wrote {length} bytes to UB[{addr}]")
    
    def read_ub(self, addr: int, length: int) -> bytes:
        """
        Read data from Unified Buffer
        
        Args:
            addr: UB address (0-255)
            length: Number of bytes (rounded up to multiple of 32)
        
        Returns:
            bytes: Data read from UB
        """
        # Round up to multiple of 32
        if length % 32 != 0:
            length = ((length // 32) + 1) * 32
        
        # Send command
        self._send_command(UARTCommand.READ_UB, 0, addr, length >> 8, length & 0xFF)
        
        # Read response
        data = self._read_response(length)
        
        if self.verbose and len(data) != length:
            print(f"⚠ Warning: Expected {length} bytes, got {len(data)}")
        
        return data
    
    # ========================================================================
    # WEIGHT MEMORY OPERATIONS
    # ========================================================================
    
    def write_weights(self, addr: int, data: Union[np.ndarray, bytes]) -> None:
        """
        Write weights to Weight Memory
        
        Args:
            addr: Weight memory address (0-1023)
            data: numpy array or bytes (padded to multiple of 8 bytes)
        """
        if isinstance(data, np.ndarray):
            data = data.tobytes()
        
        # Pad to multiple of 8 bytes
        if len(data) % 8 != 0:
            pad_len = 8 - (len(data) % 8)
            data = data + b'\x00' * pad_len
        
        length = len(data)
        addr_hi = (addr >> 8) & 0xFF
        addr_lo = addr & 0xFF
        
        # Send command
        self._send_command(UARTCommand.WRITE_WT, addr_hi, addr_lo, length >> 8, length & 0xFF)
        
        # Send data
        self.ser.write(data)
        self.stats['bytes_sent'] += length
        
        if self.verbose:
            print(f"✓ Wrote {length} bytes to Weight Memory[{addr}]")
    
    # ========================================================================
    # INSTRUCTION BUFFER MODE (Legacy)
    # ========================================================================
    
    def write_instruction(self, addr: int, opcode: Union[int, Opcode], 
                         arg1: int, arg2: int, arg3: int, flags: int = 0) -> None:
        """
        Write single instruction to instruction buffer (legacy mode)
        
        Args:
            addr: Instruction address (0-31)
            opcode: 6-bit opcode
            arg1, arg2, arg3: 8-bit arguments
            flags: 2-bit flags
        """
        if isinstance(opcode, Opcode):
            opcode = opcode.value
        
        instr = Instruction(opcode, arg1, arg2, arg3, flags)
        
        # Send command: CMD, addr_hi, addr_lo
        self._send_command(UARTCommand.WRITE_INSTR, 0, addr)
        
        # Send instruction (4 bytes, big-endian)
        self.ser.write(instr.to_bytes())
        self.stats['bytes_sent'] += 4
        self.stats['instructions_sent'] += 1
        
        if self.verbose:
            print(f"✓ Wrote instruction to PC={addr}: {instr}")
    
    def start_execution(self) -> None:
        """Start TPU execution from instruction buffer"""
        self._send_command(UARTCommand.EXECUTE)
        if self.verbose:
            print("✓ Started execution")
    
    # ========================================================================
    # STREAMING MODE (Double-Buffered Coprocessor)
    # ========================================================================
    
    def enable_stream_mode(self) -> bool:
        """
        Enter instruction streaming mode (double-buffered)
        
        Returns:
            bool: True if streaming mode enabled successfully
        """
        try:
            # #region agent log
            import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:353','message':'enable_stream_mode called','data':{'current_stream_mode':self._stream_mode,'bytes_in_waiting':self.ser.in_waiting if self.ser else -1},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'E'})+'\n')
            # #endregion
            # Send stream mode command
            self._send_command(UARTCommand.STREAM_INSTR)
            # #region agent log
            import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:362','message':'Sent STREAM_INSTR command','data':{'command':UARTCommand.STREAM_INSTR,'bytes_sent':self.stats['bytes_sent']},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'E'})+'\n')
            # #endregion
            
            # Read response: 0x00=ready, 0xFF=not ready, 0x01=buffer select
            response = self._read_response(1)
            # #region agent log
            import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:369','message':'Received stream mode response','data':{'response_len':len(response),'response_hex':response.hex() if response else 'empty','status_byte':response[0] if response else None},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'E'})+'\n')
            # #endregion
            if len(response) == 0:
                if self.verbose:
                    print("⚠ No response from FPGA")
                # #region agent log
                import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:377','message':'Empty response from enable_stream_mode','data':{'timeout':self.timeout},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'E'})+'\n')
                # #endregion
                return False
            
            status = response[0]
            if status == 0xFF:
                if self.verbose:
                    print("✗ FPGA not ready for streaming")
                # #region agent log
                import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:387','message':'FPGA not ready','data':{'status':status},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'E'})+'\n')
                # #endregion
                return False
            
            self._stream_mode = True
            self._stream_buffer_sel = status & 0x01  # Extract buffer select
            # #region agent log
            import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:396','message':'Streaming mode enabled successfully','data':{'buffer_sel':self._stream_buffer_sel,'status':status},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'E'})+'\n')
            # #endregion
            
            if self.verbose:
                print(f"✓ Streaming mode enabled (active buffer: {self._stream_buffer_sel})")
            
            return True
        except Exception as e:
            if self.verbose:
                print(f"✗ Failed to enable streaming mode: {e}")
            self.stats['errors'] += 1
            # #region agent log
            import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:410','message':'Exception in enable_stream_mode','data':{'error':str(e)},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'E'})+'\n')
            # #endregion
            return False
    
    def disable_stream_mode(self) -> None:
        """Exit instruction streaming mode"""
        self._stream_mode = False
        if self.verbose:
            print("✓ Streaming mode disabled")
    
    def stream_instruction(self, opcode: Union[int, Opcode, Instruction], arg1: int = None, arg2: int = None, 
                          arg3: int = None, flags: int = 0, retry: bool = True) -> bool:
        """
        Stream single 32-bit instruction to FPGA (double-buffered mode)
        
        Args:
            opcode: 6-bit opcode, Opcode enum, or Instruction object
            arg1, arg2, arg3: 8-bit arguments (optional if opcode is Instruction)
            flags: 2-bit flags
            retry: Retry if buffer full
        
        Returns:
            bool: True if instruction accepted, False if buffer full
        """
        # #region agent log
        import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:396','message':'stream_instruction called','data':{'opcode_type':type(opcode).__name__,'opcode_value':str(opcode),'arg1':arg1,'arg2':arg2,'arg3':arg3,'flags':flags,'stream_mode':self._stream_mode,'retry':retry},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'C'})+'\n')
        # #endregion
        
        if not self._stream_mode:
            raise RuntimeError("Streaming mode not enabled. Call enable_stream_mode() first.")
        
        # Support both Instruction object and individual parameters (Hypothesis C)
        if isinstance(opcode, Instruction):
            instr = opcode
            # #region agent log
            import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:416','message':'Using Instruction object','data':{'opcode':instr.opcode,'arg1':instr.arg1,'arg2':instr.arg2,'arg3':instr.arg3,'flags':instr.flags},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'C'})+'\n')
            # #endregion
        else:
            if isinstance(opcode, Opcode):
                opcode = opcode.value
            instr = Instruction(opcode, arg1, arg2, arg3, flags)
            # #region agent log
            import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:426','message':'Created Instruction from params','data':{'opcode':opcode,'arg1':arg1,'arg2':arg2,'arg3':arg3,'flags':flags},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'C'})+'\n')
            # #endregion
        
        # Send instruction (4 bytes, big-endian)
        instr_bytes = instr.to_bytes()
        # #region agent log
        import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:435','message':'Sending instruction bytes','data':{'instr_bytes':instr_bytes.hex(),'bytes_in_waiting_before':self.ser.in_waiting if self.ser else -1},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'A,B'})+'\n')
        # #endregion
        self.ser.write(instr_bytes)
        self.stats['bytes_sent'] += 4
        
        # Read acknowledgment: 0x00=accepted, 0xFF=full, 0x01=buffer swapped
        # #region agent log
        import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:445','message':'About to read ACK','data':{'bytes_in_waiting':self.ser.in_waiting if self.ser else -1,'timeout':self.timeout},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'B,D'})+'\n')
        # #endregion
        response = self._read_response(1)
        # #region agent log
        import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:451','message':'Received ACK response','data':{'response_len':len(response),'response_hex':response.hex() if response else 'empty','ack_byte':response[0] if response else None},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'A,D'})+'\n')
        # #endregion
        if len(response) == 0:
            if self.verbose:
                print("⚠ No acknowledgment from FPGA")
            self.stats['errors'] += 1
            # #region agent log
            import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:460','message':'Empty response from FPGA','data':{'stats':self.stats},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'D'})+'\n')
            # #endregion
            return False
        
        ack = response[0]
        # #region agent log
        import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:467','message':'Processing ACK byte','data':{'ack':ack,'ack_hex':hex(ack),'expected_codes':{'accepted':'0x00','full':'0xff','swapped':'0x01'}},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'A'})+'\n')
        # #endregion
        
        if ack == 0x00:
            # Instruction accepted
            self.stats['instructions_sent'] += 1
            return True
        
        elif ack == 0xFF:
            # Buffer full - backpressure
            # #region agent log
            import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:481','message':'Buffer full - backpressure','data':{'retry':retry,'instr':str(instr)},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'B'})+'\n')
            # #endregion
            if self.verbose:
                print(f"⚠ Buffer full, instruction not accepted: {instr}")
            
            if retry:
                # Wait and retry
                time.sleep(0.01)
                # #region agent log
                import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:492','message':'Retrying after backpressure','data':{'sleep_time_ms':10},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'B'})+'\n')
                # #endregion
                # Need to pass Instruction object for retry
                if isinstance(opcode, Instruction):
                    return self.stream_instruction(opcode, retry=False)
                else:
                    return self.stream_instruction(opcode, arg1, arg2, arg3, flags, retry=False)
            
            return False
        
        elif ack == 0x01:
            # Buffer swapped
            self._stream_buffer_sel = 1 - self._stream_buffer_sel
            self.stats['instructions_sent'] += 1
            if self.verbose:
                print(f"✓ Buffer swapped (active: {self._stream_buffer_sel}), instruction accepted")
            return True
        
        else:
            # #region agent log
            import json,time;open('/Users/alanma/Downloads/tpu_to_fpga/.cursor/debug.log','a').write(json.dumps({'location':'tpu_coprocessor_driver.py:514','message':'Unexpected ACK byte','data':{'ack':ack,'ack_hex':hex(ack),'bytes_in_waiting_after':self.ser.in_waiting if self.ser else -1,'instr':str(instr)},'timestamp':int(time.time()*1000),'sessionId':'debug-session','hypothesisId':'A,B'})+'\n')
            # #endregion
            if self.verbose:
                print(f"⚠ Unexpected acknowledgment: 0x{ack:02X}")
            self.stats['errors'] += 1
            return False
    
    def stream_instructions(self, instructions: List[Tuple], 
                          flow_control: bool = True) -> int:
        """
        Stream multiple instructions
        
        Args:
            instructions: List of (opcode, arg1, arg2, arg3, flags) tuples
            flow_control: Wait for buffer space if full
        
        Returns:
            int: Number of instructions successfully sent
        """
        if not self._stream_mode:
            raise RuntimeError("Streaming mode not enabled")
        
        sent = 0
        for instr_tuple in instructions:
            if len(instr_tuple) == 4:
                opcode, arg1, arg2, arg3 = instr_tuple
                flags = 0
            else:
                opcode, arg1, arg2, arg3, flags = instr_tuple
            
            success = self.stream_instruction(opcode, arg1, arg2, arg3, flags, retry=flow_control)
            if success:
                sent += 1
            elif not flow_control:
                break  # Stop on first failure if no flow control
        
        return sent
    
    # ========================================================================
    # STATUS MONITORING
    # ========================================================================
    
    def read_status(self) -> Optional[TPUStatus]:
        """
        Read TPU status register
        
        Returns:
            TPUStatus object or None if read failed
        """
        self._send_command(UARTCommand.READ_STATUS)
        response = self._read_response(1)
        
        if len(response) == 0:
            return None
        
        return TPUStatus.from_byte(response[0])
    
    def wait_done(self, timeout: float = 10.0, poll_interval: float = 0.01) -> bool:
        """
        Wait until TPU is done executing
        
        Args:
            timeout: Maximum seconds to wait
            poll_interval: Status polling interval
        
        Returns:
            bool: True if done, False if timeout
        """
        start = time.time()
        while time.time() - start < timeout:
            status = self.read_status()
            if status and not status.sys_busy and not status.vpu_busy:
                return True
            time.sleep(poll_interval)
        return False
    
    # ========================================================================
    # HIGH-LEVEL INSTRUCTION HELPERS
    # ========================================================================
    
    def nop(self) -> Instruction:
        """No operation"""
        return self.encode_instruction(Opcode.NOP, 0, 0, 0, 0)
    
    def matmul(self, ub_addr: int, acc_addr: int, rows: int, 
              transpose: bool = False, signed: bool = False) -> Instruction:
        """Matrix multiplication"""
        flags = (1 if transpose else 0) | ((1 if signed else 0) << 1)
        return self.encode_instruction(Opcode.MATMUL, ub_addr, acc_addr, rows, flags)
    
    def relu(self, acc_addr: int, ub_out_addr: int, length: int, 
            leak_factor: bool = False) -> Instruction:
        """ReLU activation"""
        flags = 1 if leak_factor else 0
        return self.encode_instruction(Opcode.RELU, acc_addr, ub_out_addr, length, flags)
    
    def sync(self, mask: int = 0x01, timeout: int = 4096) -> Instruction:
        """Synchronize operations"""
        return self.encode_instruction(Opcode.SYNC, mask, timeout >> 8, timeout & 0xFF, 0)
    
    def halt(self) -> Instruction:
        """Program termination"""
        return self.encode_instruction(Opcode.HALT, 0, 0, 0, 0)
    
    # ========================================================================
    # STATISTICS
    # ========================================================================
    
    def get_stats(self) -> dict:
        """Get driver statistics"""
        return self.stats.copy()
    
    def print_stats(self) -> None:
        """Print driver statistics"""
        print("\n" + "="*60)
        print("TPU Coprocessor Driver Statistics")
        print("="*60)
        print(f"Instructions sent:     {self.stats['instructions_sent']}")
        print(f"Bytes sent:            {self.stats['bytes_sent']}")
        print(f"Bytes received:        {self.stats['bytes_received']}")
        print(f"Errors:                {self.stats['errors']}")
        print("="*60)

# ============================================================================
# EXAMPLE USAGE
# ============================================================================

if __name__ == '__main__':
    # Example: Streaming coprocessor usage
    print("TPU Coprocessor Driver Example")
    print("="*60)
    
    # Connect to TPU
    tpu = TPU_Coprocessor(port='/dev/ttyUSB0', verbose=True)
    
    try:
        # Example 1: Legacy buffer mode
        print("\n--- Example 1: Legacy Buffer Mode ---")
        tpu.write_instruction(0, Opcode.NOP, 0, 0, 0, 0)
        tpu.write_instruction(1, Opcode.CFG_REG, 0, 0x06, 0x66, 0)
        tpu.start_execution()
        time.sleep(0.1)
        
        # Example 2: Streaming mode
        print("\n--- Example 2: Streaming Mode ---")
        if tpu.enable_stream_mode():
            # Stream instructions
            instructions = [
                (Opcode.NOP, 0, 0, 0, 0),
                (Opcode.CFG_REG, 0, 0x06, 0x66, 0),
                (Opcode.MATMUL, 0, 0x20, 4, 0),
                (Opcode.SYNC, 0x01, 0x00, 0x10, 0),
                (Opcode.RELU, 0x20, 0x40, 4, 0),
                (Opcode.HALT, 0, 0, 0, 0)
            ]
            
            sent = tpu.stream_instructions(instructions)
            print(f"✓ Sent {sent}/{len(instructions)} instructions")
        
        # Check status
        print("\n--- Status Check ---")
        status = tpu.read_status()
        if status:
            print(status)
        
        # Print statistics
        tpu.print_stats()
        
    except KeyboardInterrupt:
        print("\nInterrupted by user")
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
    finally:
        tpu.close()

