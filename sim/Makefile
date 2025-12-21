# 3x3 TPU FPGA Simulation Makefile
# Based on tinytinyTPU structure but extended for 3x3 systolic array

SHELL := /bin/bash

# Directories
SIM_DIR := $(shell pwd)
RTL_DIR := $(SIM_DIR)/../rtl
WAVE_DIR := $(SIM_DIR)/waves
TEST_DIR := $(SIM_DIR)/tests
BUILD_DIR := $(SIM_DIR)/sim_build

# Tools
IVERILOG := iverilog
VVP := vvp
VERILATOR := verilator
WAVE_VIEWER := gtkwave

# Waveform generation
WAVES ?= 0
export COCOTB_WAVEFORM_VIEWER := $(WAVE_VIEWER)

# Ensure waves directory exists
$(shell mkdir -p $(WAVE_DIR))

# Our RTL source files (extended tinytinyTPU design)
RTL_SOURCES := \
    $(RTL_DIR)/pe.sv \
    $(RTL_DIR)/mmu.sv \
    $(RTL_DIR)/dual_weight_fifo.sv \
    $(RTL_DIR)/accumulator.sv \
    $(RTL_DIR)/accumulator_align.sv \
    $(RTL_DIR)/accumulator_mem.sv \
    $(RTL_DIR)/activation_func.sv \
    $(RTL_DIR)/activation_pipeline.sv \
    $(RTL_DIR)/normalizer.sv \
    $(RTL_DIR)/loss_block.sv \
    $(RTL_DIR)/unified_buffer.sv \
    $(RTL_DIR)/mlp_top.sv

# cocotb-test environment variables
export VERILOG_SOURCES = $(RTL_SOURCES)
export TOPLEVEL_LANG = verilog
export COCOTB_TEST_MODULES = $(MODULE)
export COCOTB_WAVEFORM_VIEWER = $(WAVE_VIEWER)
export PYTHONPATH := $(TEST_DIR)

.PHONY: help test test_pe test_mmu test_weight_fifo test_dual_fifo test_accumulator \
        test_activation_func test_normalizer test_activation_pipeline test_mlp \
        lint waves clean clean_waves

# Include cocotb-test makefile
include $(shell cocotb-test --inc-makefile)

help:
	@echo "3x3 TPU FPGA Simulation (Extended tinytinyTPU)"
	@echo "=============================================="
	@echo ""
	@echo "Test Commands:"
	@echo "  make test              Run all tests"
	@echo "  make test WAVES=1      Run all tests with waveform generation"
	@echo "  make test_pe           Run PE tests only"
	@echo "  make test_mmu          Run MMU tests only"
	@echo "  make test_mlp          Run MLP integration tests"
	@echo ""
	@echo "Waveform Commands:"
	@echo "  make waves             List available waveforms"
	@echo "  make waves MODULE=pe   Open specific waveform in viewer"
	@echo ""
	@echo "Other:"
	@echo "  make lint              Verilator lint check"
	@echo "  make clean             Remove build artifacts"
	@echo ""
	@echo "Environment Variables:"
	@echo "  WAVES=1                  Enable waveform generation"
	@echo "  WAVE_VIEWER=gtkwave      Use GTKWave instead of Surfer"
	@echo "  MODULE=<name>            Specify module for make waves"

# Test targets
test_pe:
	export TOPLEVEL=pe && export MODULE=test_pe && WAVES=$(WAVES) make

test_mmu:
	export TOPLEVEL=mmu && export MODULE=test_mmu && WAVES=$(WAVES) make

test_weight_fifo:
	export TOPLEVEL=weight_fifo && export MODULE=test_weight_fifo && WAVES=$(WAVES) make

test_dual_fifo:
	export TOPLEVEL=dual_weight_fifo && export MODULE=test_dual_weight_fifo && WAVES=$(WAVES) make

test_accumulator:
	export TOPLEVEL=accumulator && export MODULE=test_accumulator && WAVES=$(WAVES) make

test_activation_func:
	export TOPLEVEL=activation_func && export MODULE=test_activation_func && WAVES=$(WAVES) make

test_normalizer:
	export TOPLEVEL=normalizer && export MODULE=test_normalizer && WAVES=$(WAVES) make

test_activation_pipeline:
	export TOPLEVEL=activation_pipeline && export MODULE=test_activation_pipeline && WAVES=$(WAVES) make

test_mlp:
	export TOPLEVEL=mlp_top && export MODULE=test_mlp_integration && WAVES=$(WAVES) make

# Lint
lint:
	$(VERILATOR) --lint-only -Wall -Wno-PINCONNECTEMPTY -Wno-UNUSEDSIGNAL -Wno-MULTITOP $(RTL_SOURCES)

# Waveform viewer
waves:
ifdef MODULE
	@if [ -f $(WAVE_DIR)/$(MODULE).vcd ]; then \
		echo "Opening $(MODULE).vcd"; \
		$(WAVE_VIEWER) $(WAVE_DIR)/$(MODULE).vcd; \
	else \
		echo "Error: $(WAVE_DIR)/$(MODULE).vcd not found"; \
		echo "Generate it with: make test_$(MODULE) WAVES=1"; \
	fi
else
	@echo ""
	@echo "Available waveforms:"
	@echo ""
	@for f in $$(ls $(WAVE_DIR)/*.vcd 2>/dev/null | sort); do \
		name=$$(basename $$f .vcd); \
		echo "  - $$name"; \
	done
	@if [ -z "$$(ls $(WAVE_DIR)/*.vcd 2>/dev/null)" ]; then \
		echo "  No waveforms found."; \
		echo ""; \
		echo "Generate waveforms with: make test WAVES=1"; \
	else \
		echo ""; \
		echo "Usage: make waves MODULE=<name>"; \
		echo "Example: make waves MODULE=pe"; \
	fi
endif

# Clean
clean_waves:
	rm -f $(WAVE_DIR)/*.vcd $(WAVE_DIR)/*.fst

clean:
	rm -rf $(BUILD_DIR) __pycache__ .pytest_cache
	rm -rf $(TEST_DIR)/__pycache__ $(TEST_DIR)/.pytest_cache
	rm -rf obj_dir
	rm -f results.xml dump.fst dump.vcd
