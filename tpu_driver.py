#!/usr/bin/env python3
"""
TPU UART Driver for Basys3
Provides Python interface to load data and run inference on FPGA TPU
"""

import serial
import numpy as np
import time
import struct
try:
    import matplotlib.pyplot as plt
    import cv2
    VISUALIZATION_AVAILABLE = True
except ImportError:
    VISUALIZATION_AVAILABLE = False

class TPU_Basys3:
    """UART interface to TPU on Basys3 FPGA"""

    # Command codes
    CMD_WRITE_UB    = 0x01
    CMD_WRITE_WT    = 0x02
    CMD_WRITE_INSTR = 0x03
    CMD_READ_UB     = 0x04
    CMD_EXECUTE     = 0x05
    CMD_READ_STATUS = 0x06

    # Opcodes (from ISA)
    OP_NOP          = 0x00
    OP_RD_HOST_MEM  = 0x01
    OP_WR_HOST_MEM  = 0x02
    OP_RD_WEIGHT    = 0x03
    OP_LD_UB        = 0x04
    OP_ST_UB        = 0x05
    OP_MATMUL       = 0x10
    OP_CONV2D       = 0x11
    OP_MATMUL_ACC   = 0x12
    OP_RELU         = 0x18
    OP_RELU6        = 0x19
    OP_SIGMOID      = 0x1A
    OP_TANH         = 0x1B
    OP_MAXPOOL      = 0x20
    OP_AVGPOOL      = 0x21
    OP_ADD_BIAS     = 0x22
    OP_BATCH_NORM   = 0x23
    OP_SYNC         = 0x30
    OP_CFG_REG      = 0x31
    OP_HALT         = 0x3F

    def __init__(self, port='/dev/ttyUSB0', baud=115200, timeout=2):
        """Initialize UART connection to TPU"""
        self.ser = serial.Serial(port, baud, timeout=timeout)
        time.sleep(2)  # Wait for FPGA to initialize
        print(f"Connected to TPU on {port} at {baud} baud")

    def close(self):
        """Close serial connection"""
        self.ser.close()

    def write_ub(self, addr, data):
        """Write data to Unified Buffer

        Args:
            addr: UB address (0-255)
            data: numpy array or bytes (will be padded to multiple of 32 bytes)
        """
        if isinstance(data, np.ndarray):
            data = data.tobytes()

        # Pad to multiple of 32 bytes
        if len(data) % 32 != 0:
            pad_len = 32 - (len(data) % 32)
            data = data + b'\x00' * pad_len

        length = len(data)

        # Send command
        cmd = struct.pack('BBBBH',
                         self.CMD_WRITE_UB,
                         0,            # addr_hi
                         addr,         # addr_lo
                         length >> 8,  # length_hi
                         length & 0xFF # length_lo
                         )
        self.ser.write(cmd)

        # Send data
        self.ser.write(data)

        print(f"Wrote {length} bytes to UB[{addr}]")

    def read_ub(self, addr, length):
        """Read data from Unified Buffer

        Args:
            addr: UB address (0-255)
            length: Number of bytes to read (will be rounded up to multiple of 32)

        Returns:
            bytes: Data read from UB
        """
        # Round up to multiple of 32
        if length % 32 != 0:
            length = ((length // 32) + 1) * 32

        # Send command
        cmd = struct.pack('BBBBH',
                         self.CMD_READ_UB,
                         0,            # addr_hi
                         addr,         # addr_lo
                         length >> 8,  # length_hi
                         length & 0xFF # length_lo
                         )
        self.ser.write(cmd)

        # Read response
        data = self.ser.read(length)

        if len(data) != length:
            print(f"Warning: Expected {length} bytes, got {len(data)}")

        return data

    def write_weights(self, addr, data):
        """Write weights to Weight Memory

        Args:
            addr: Weight memory address (0-1023 for 1024 blocks)
            data: numpy array or bytes (will be padded to multiple of 8 bytes)
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
        cmd = struct.pack('BBBBH',
                         self.CMD_WRITE_WT,
                         addr_hi,
                         addr_lo,
                         length >> 8,
                         length & 0xFF
                         )
        self.ser.write(cmd)

        # Send data
        self.ser.write(data)

        print(f"Wrote {length} bytes to Weight Memory[{addr}]")

    def write_instruction(self, addr, opcode, arg1, arg2, arg3, flags=0):
        """Write single instruction to instruction buffer

        Args:
            addr: Instruction address (0-31)
            opcode: 6-bit opcode
            arg1, arg2, arg3: 8-bit arguments
            flags: 2-bit flags
        """
        # Build 32-bit instruction
        instr = (opcode << 26) | (arg1 << 18) | (arg2 << 10) | (arg3 << 2) | flags

        # Send command
        cmd = struct.pack('BBB',
                         self.CMD_WRITE_INSTR,
                         0,        # addr_hi
                         addr      # addr_lo
                         )
        self.ser.write(cmd)

        # Send instruction (big-endian)
        instr_bytes = struct.pack('>I', instr)
        self.ser.write(instr_bytes)

        print(f"Wrote instruction to PC={addr}: op={opcode:02x} args=[{arg1},{arg2},{arg3}] flags={flags}")

    def start_execution(self):
        """Start TPU execution (jumps PC to 0)"""
        self.ser.write(bytes([self.CMD_EXECUTE]))
        print("Started execution")

    def read_status(self):
        """Read TPU status

        Returns:
            dict: Status flags {sys_busy, sys_done, vpu_busy, vpu_done, ub_busy, ub_done}
        """
        self.ser.write(bytes([self.CMD_READ_STATUS]))
        status_byte = self.ser.read(1)

        if len(status_byte) == 0:
            return None

        s = status_byte[0]
        status = {
            'sys_busy': bool(s & 0x01),
            'sys_done': bool(s & 0x02),
            'vpu_busy': bool(s & 0x04),
            'vpu_done': bool(s & 0x08),
            'ub_busy':  bool(s & 0x10),
            'ub_done':  bool(s & 0x20),
        }
        return status

    def wait_done(self, timeout=10):
        """Wait until TPU is done executing

        Args:
            timeout: Maximum seconds to wait

        Returns:
            bool: True if done, False if timeout
        """
        start = time.time()
        while time.time() - start < timeout:
            status = self.read_status()
            if status and not status['sys_busy'] and not status['vpu_busy']:
                return True
            time.sleep(0.01)
        return False

    # ========================================================================
    # High-level functions
    # ========================================================================

    def load_program(self, instructions):
        """Load a list of instructions

        Args:
            instructions: List of tuples (opcode, arg1, arg2, arg3, flags)
        """
        for i, (opcode, arg1, arg2, arg3, flags) in enumerate(instructions):
            self.write_instruction(i, opcode, arg1, arg2, arg3, flags)

    def matmul(self, activations, weights, ub_addr=0, acc_addr=0, batch_size=16):
        """Perform matrix multiplication

        Args:
            activations: Input activations (numpy array)
            weights: Weight matrix (numpy array)
            ub_addr: Where to load activations in UB
            acc_addr: Where to store results in accumulator
            batch_size: Number of rows to process

        Returns:
            None (results stay in accumulator, use VPU to read)
        """
        print("\n=== Matrix Multiply ===")

        # Load activations into UB
        print("Loading activations...")
        self.write_ub(ub_addr, activations)

        # Load weights
        print("Loading weights...")
        self.write_weights(0, weights)

        # Write MATMUL instruction
        program = [
            (self.OP_MATMUL, ub_addr, acc_addr, batch_size, 0b00),  # unsigned, no transpose
        ]
        self.load_program(program)

        # Execute
        self.start_execution()

        # Wait for completion
        print("Computing...")
        if self.wait_done(timeout=5):
            print("MATMUL complete!")
        else:
            print("Warning: Timeout waiting for MATMUL")

    def relu(self, acc_addr, ub_out_addr, length=16):
        """Apply ReLU activation

        Args:
            acc_addr: Read from accumulator
            ub_out_addr: Write to UB
            length: Number of elements
        """
        print("\n=== ReLU ===")

        program = [
            (self.OP_RELU, acc_addr, ub_out_addr, length, 0),
        ]
        self.load_program(program)

        self.start_execution()

        if self.wait_done(timeout=2):
            print("ReLU complete!")
        else:
            print("Warning: Timeout waiting for ReLU")

    def forward_pass(self, input_data, weights, with_relu=True):
        """Run complete forward pass: MATMUL + ReLU

        Args:
            input_data: Input activations (numpy array, shape (N, 256))
            weights: Weight matrix (numpy array, shape (256, 256))
            with_relu: Whether to apply ReLU

        Returns:
            numpy array: Output activations
        """
        print("\n" + "="*60)
        print("RUNNING FORWARD PASS")
        print("="*60)

        batch_size = input_data.shape[0]

        # Step 1: MATMUL
        self.matmul(input_data, weights, ub_addr=0, acc_addr=1, batch_size=batch_size)

        # Step 2: ReLU (if requested)
        if with_relu:
            self.relu(acc_addr=1, ub_out_addr=2, length=batch_size)

        # Step 3: Read results
        print("\nReading results...")
        result_bytes = self.read_ub(addr=2, length=batch_size * 256)
        results = np.frombuffer(result_bytes, dtype=np.uint8).reshape(batch_size, 256)

        print("\n" + "="*60)
        print("FORWARD PASS COMPLETE")
        print("="*60)

        return results

    def canny_edge_detect(self, image, kernel_size=3):
        """Run Canny edge detection using 3x3 systolic array

        Args:
            image: Input grayscale image (numpy array)
            kernel_size: Convolution kernel size (must be 3 for our TPU)

        Returns:
            numpy array: Edge-detected image
        """
        if kernel_size != 3:
            raise ValueError("Only 3x3 kernels supported by current TPU")

        print("\n" + "="*60)
        print("CANNY EDGE DETECTION ON 3x3 SYSTOLIC ARRAY")
        print("="*60)

        h, w = image.shape

        # Step 1: Gaussian blur (simplified 3x3 kernel)
        gaussian_kernel = np.array([[1, 2, 1],
                                   [2, 4, 2],
                                   [1, 2, 1]], dtype=np.uint8) // 4  # Normalize

        # Step 2: Sobel X and Y gradients
        sobel_x = np.array([[-1, 0, 1],
                           [-2, 0, 2],
                           [-1, 0, 1]], dtype=np.int8)

        sobel_y = np.array([[-1, -2, -1],
                           [0, 0, 0],
                           [1, 2, 1]], dtype=np.int8)

        # Process image in tiles that fit our systolic array
        tile_size = 8  # 8x8 tiles for our 3x3 array
        output = np.zeros_like(image, dtype=np.uint8)

        for y in range(0, h-tile_size+1, tile_size//2):
            for x in range(0, w-tile_size+1, tile_size//2):
                # Extract tile with padding
                tile = image[y:y+tile_size, x:x+tile_size]

                # Apply Gaussian blur using systolic array
                blurred = self.conv2d(tile, gaussian_kernel)

                # Apply Sobel X
                grad_x = self.conv2d(blurred, sobel_x)

                # Apply Sobel Y
                grad_y = self.conv2d(blurred, sobel_y)

                # Compute gradient magnitude
                grad_mag = np.sqrt(grad_x**2 + grad_y**2).astype(np.uint8)

                # Simple thresholding (non-maximum suppression would need more ops)
                edges = (grad_mag > 50).astype(np.uint8) * 255

                # Place back in output
                output[y:y+tile_size, x:x+tile_size] = edges

        return output

    def visualize_results(self, input_image, output_image, title="TPU Processing Results"):
        """Visualize input and output images

        Args:
            input_image: Original input image
            output_image: Processed output image
            title: Plot title
        """
        if not VISUALIZATION_AVAILABLE:
            print("Visualization libraries (matplotlib, opencv) not available")
            print(f"Input shape: {input_image.shape}, Output shape: {output_image.shape}")
            return

        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4))

        ax1.imshow(input_image, cmap='gray')
        ax1.set_title('Input Image')
        ax1.axis('off')

        ax2.imshow(output_image, cmap='gray')
        ax2.set_title('TPU Output')
        ax2.axis('off')

        plt.suptitle(title)
        plt.tight_layout()
        plt.show()

    def conv2d(self, image, kernel):
        """2D convolution using systolic array

        Args:
            image: Input image tile
            kernel: 3x3 convolution kernel

        Returns:
            numpy array: Convolved output
        """
        h, w = image.shape

        # Flatten kernel for systolic array (3x3 = 9 weights per output)
        kernel_flat = kernel.flatten()

        # Prepare input as sliding windows
        output = np.zeros((h-2, w-2), dtype=np.uint8)

        for i in range(1, h-1):
            for j in range(1, w-1):
                # Extract 3x3 window
                window = image[i-1:i+2, j-1:j+2].flatten()

                # Send to systolic array for convolution
                result = self.matmul(window.reshape(1, -1),
                                   kernel_flat.reshape(-1, 1),
                                   ub_addr=0, acc_addr=1, batch_size=1)

                output[i-1, j-1] = result[0, 0]

        return output


# ============================================================================
# Example usage
# ============================================================================

if __name__ == '__main__':
    # Connect to TPU
    tpu = TPU_Basys3(port='/dev/ttyUSB0', baud=115200)

    try:
        print("\n" + "="*70)
        print(" TPU Basys3 Test - Simple Matrix Multiply + ReLU")
        print("="*70)

        # Generate test data
        batch_size = 9  # Optimized for 3x3 systolic array
        input_dim = 256

        print(f"\nGenerating test data: {batch_size}x{input_dim} input")

        # Small values to avoid overflow with 8-bit math
        activations = np.random.randint(0, 8, (batch_size, input_dim), dtype=np.uint8)
        weights = np.random.randint(0, 8, (input_dim, input_dim), dtype=np.uint8)

        print(f"Input range: [{activations.min()}, {activations.max()}]")
        print(f"Weight range: [{weights.min()}, {weights.max()}]")

        # Run forward pass
        output = tpu.forward_pass(activations, weights, with_relu=True)

        # Display results
        print(f"\nOutput shape: {output.shape}")
        print(f"Output range: [{output.min()}, {output.max()}]")
        print(f"\nFirst output vector (first 16 elements):")
        print(output[0, :16])

        # Test status reading
        print("\n" + "="*70)
        print(" Testing Status Register")
        print("="*70)
        status = tpu.read_status()
        print(f"Status: {status}")

        # Test individual instruction
        print("\n" + "="*70)
        print(" Testing Individual Instructions")
        print("="*70)

        # Test CFG_REG: Set ReLU leak factor
        print("\nSetting config register 0 = 0x0666 (leak factor 0.1)")
        tpu.write_instruction(0, tpu.OP_CFG_REG, 0, 0x06, 0x66, 0)
        tpu.start_execution()
        time.sleep(0.1)

        print("\n" + "="*70)
        print(" All tests complete!")
        print("="*70)

        # ============================================================================
        # CANNY EDGE DETECTION EXAMPLE
        # ============================================================================
        print("\n" + "="*70)
        print(" CANNY EDGE DETECTION DEMO")
        print("="*70)

        # Generate test image (small for demo)
        test_image = np.random.randint(0, 255, (16, 16), dtype=np.uint8)

        print(f"\nProcessing {test_image.shape} image with Canny edge detection...")

        # Note: This is a conceptual demo - actual implementation would need
        # more sophisticated convolution and post-processing on the FPGA
        try:
            edges = tpu.canny_edge_detect(test_image)

            print(f"Edge detection complete! Output shape: {edges.shape}")
            print(f"Edge pixels found: {np.sum(edges > 0)}")

            # Visualize if possible
            if VISUALIZATION_AVAILABLE:
                tpu.visualize_results(test_image, edges, "Canny Edge Detection on TPU")

        except Exception as e:
            print(f"Edge detection demo failed: {e}")
            print("This is expected - full implementation needs more FPGA ops")

    except KeyboardInterrupt:
        print("\nInterrupted by user")
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
    finally:
        tpu.close()
        print("\nClosed connection to TPU")
