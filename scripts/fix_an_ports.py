#!/usr/bin/env python3
"""
Pre-pack script to fix an[1], an[2], an[3] port naming issue in nextpnr-xilinx
This script adds IOSTANDARD constraints to PAD cells that nextpnr creates with double-bracketed names
"""
import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: fix_an_ports.py <context>")
        sys.exit(1)
    
    ctx = sys.argv[1]
    
    # This will be called by nextpnr-xilinx during pre-pack phase
    # We need to find PAD cells with names like an[1][1] and add IOSTANDARD
    # However, this requires access to the Context object which isn't easily available
    
    # Alternative: Just add IOSTANDARD to all an ports in XDC using a workaround
    # Actually, the real fix needs to be in the XDC file or in nextpnr itself
    
    print("Note: This is a placeholder. The real fix needs to be in nextpnr-xilinx or XDC constraints.")
    return 0

if __name__ == "__main__":
    sys.exit(main())

