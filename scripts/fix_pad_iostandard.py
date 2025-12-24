#!/usr/bin/env python3
"""
Pre-pack script for nextpnr-xilinx to add IOSTANDARD to PAD cells
that nextpnr creates with double-bracketed names (an[1][1], etc.)

This is a workaround for a nextpnr-xilinx bug where array ports with indices > 0
get double-bracketed names in PAD cells.
"""
import re

def pre_pack(ctx):
    """Called by nextpnr-xilinx before packing"""
    # Find all PAD cells and add IOSTANDARD if missing
    pad_count = 0
    for cell_name, cell in ctx.cells.items():
        if cell.type == ctx.id("PAD"):
            cell_name_str = cell_name.str(ctx)
            
            # Check if it has IOSTANDARD attribute
            if not cell.attrs.count(ctx.id("IOSTANDARD")):
                # Add default IOSTANDARD for all PAD cells
                cell.attrs[ctx.id("IOSTANDARD")] = "LVCMOS33"
                pad_count += 1
                
                # Special handling for double-bracketed ports (an[1][1], etc.)
                # This is a workaround for nextpnr-xilinx bug
                if "][" in cell_name_str:
                    # Extract base name and index: an[1][1] -> an, 1
                    match = re.match(r'([a-zA-Z_][a-zA-Z0-9_]*)\[(\d+)\]\[(\d+)\]', cell_name_str)
                    if match:
                        base_name, idx1, idx2 = match.groups()
                        if idx1 == idx2:  # Only fix if indices match (an[1][1])
                            # The port should be an[1], but nextpnr created an[1][1]
                            # We've already set IOSTANDARD above, so we're done
                            pass
    
    if pad_count > 0:
        print(f"Added IOSTANDARD=LVCMOS33 to {pad_count} PAD cells")
