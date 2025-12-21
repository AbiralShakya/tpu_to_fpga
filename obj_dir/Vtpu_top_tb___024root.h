// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtpu_top_tb.h for the primary calling header

#ifndef VERILATED_VTPU_TOP_TB___024ROOT_H_
#define VERILATED_VTPU_TOP_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"
class Vtpu_top_tb___024unit;


class Vtpu_top_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtpu_top_tb___024root final : public VerilatedModule {
  public:
    // CELLS
    Vtpu_top_tb___024unit* __PVT____024unit;

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*0:0*/ tpu_top_tb__DOT__clk;
        CData/*0:0*/ tpu_top_tb__DOT__rst_n;
        CData/*7:0*/ tpu_top_tb__DOT__instr_addr_out;
        CData/*0:0*/ tpu_top_tb__DOT__dma_start_in;
        CData/*0:0*/ tpu_top_tb__DOT__dma_dir_in;
        CData/*7:0*/ tpu_top_tb__DOT__dma_ub_addr_in;
        CData/*1:0*/ tpu_top_tb__DOT__dma_elem_sz_in;
        CData/*0:0*/ tpu_top_tb__DOT__tpu_busy;
        CData/*1:0*/ tpu_top_tb__DOT__pipeline_stage;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__uart_tx;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_debug_state;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_debug_cmd;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__sys_start;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__sys_rows;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__ub_rd_addr;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__wt_fifo_wr;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__vpu_start;
        CData/*3:0*/ tpu_top_tb__DOT__dut__DOT__vpu_mode;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__wt_buf_sel;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__acc_buf_sel;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_ub_wr_addr;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_ub_rd_addr;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__uart_wt_wr_en;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__uart_instr_wr_en;
        CData/*4:0*/ tpu_top_tb__DOT__dut__DOT__uart_instr_wr_addr;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__uart_start_execution;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__dma_start;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__dma_dir;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__dma_ub_addr;
        CData/*1:0*/ tpu_top_tb__DOT__dut__DOT__dma_elem_sz;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__use_uart_dma;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_valid;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_pc;
        CData/*5:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_opcode;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg1;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg2;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg3;
        CData/*1:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_flags;
        CData/*3:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_unit_sel;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid;
        CData/*5:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg1;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg2;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg3;
        CData/*1:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_flags;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_data;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_valid;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo;
        CData/*4:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index;
        CData/*1:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state;
        CData/*2:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_byte;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_1;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_2;
        CData/*1:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state;
    };
    struct {
        CData/*2:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__tx_byte;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col0;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col1;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col2;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_valid;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_a_valid;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_b_ready;
        CData/*2:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state;
        CData/*2:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__weight_load_counter;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__compute_counter;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__total_rows;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_en;
        CData/*0:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_empty;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row1_delayed;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row2_delayed;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_00__act_out;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_01__act_out;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_10__act_out;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_11__act_out;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_02__act_out;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_12__act_out;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_20__act_out;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_21__act_out;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_22__act_out;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__weight_reg;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__weight_reg;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__weight_reg;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__weight_reg;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__weight_reg;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__weight_reg;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__weight_reg;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__weight_reg;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__weight_reg;
        CData/*1:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter;
        CData/*1:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr;
        CData/*1:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count;
        CData/*7:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr;
        CData/*0:0*/ __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0;
        CData/*0:0*/ __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0;
        CData/*0:0*/ __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0;
        CData/*0:0*/ __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__wt_buf_sel__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__acc_buf_sel__0;
        CData/*0:0*/ __VstlDidInit;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__wt_buf_sel__1;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__acc_buf_sel__1;
        CData/*7:0*/ __Vtrigprevexpr___TOP__tpu_top_tb__DOT__instr_addr_out__0;
        CData/*1:0*/ __Vtrigprevexpr___TOP__tpu_top_tb__DOT__pipeline_stage__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tpu_top_tb__DOT__tpu_busy__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tpu_top_tb__DOT__clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tpu_top_tb__DOT__rst_n__0;
        CData/*0:0*/ __VactDidInit;
        SData/*15:0*/ tpu_top_tb__DOT__dma_length_in;
        SData/*9:0*/ tpu_top_tb__DOT__unnamedblk4__DOT__cycle_count;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__uart_debug_byte_count;
        SData/*9:0*/ tpu_top_tb__DOT__dut__DOT__uart_wt_wr_addr;
    };
    struct {
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__dma_length;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data;
        SData/*11:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_ptr;
        SData/*11:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr;
        SData/*12:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count;
        SData/*12:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_count;
        SData/*11:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_ptr;
        SData/*11:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr;
        SData/*12:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count;
        SData/*12:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_count;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mult_result;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mult_result;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mult_result;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mult_result;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mult_result;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mult_result;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mult_result;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mult_result;
        SData/*15:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mult_result;
        SData/*11:0*/ __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0;
        SData/*11:0*/ __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0;
        VlWide<8>/*255:0*/ tpu_top_tb__DOT__dma_data_in;
        IData/*31:0*/ tpu_top_tb__DOT__unnamedblk1__DOT__i;
        IData/*31:0*/ tpu_top_tb__DOT__unnamedblk2__DOT__i;
        IData/*31:0*/ tpu_top_tb__DOT__unnamedblk3__DOT__i;
        VlWide<8>/*255:0*/ tpu_top_tb__DOT__dut__DOT__ub_rd_data;
        VlWide<8>/*255:0*/ tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__uart_instr_wr_data;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg;
        VlWide<8>/*255:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer;
        VlWide<8>/*255:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer;
        VlWide<3>/*95:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out;
        VlWide<8>/*255:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data;
        IData/*23:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_wr_data;
        IData/*23:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result;
        VlWide<3>/*95:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result0;
        IData/*31:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result1;
        VlWide<8>/*255:0*/ tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data;
        IData/*23:0*/ __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0;
        IData/*23:0*/ __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0;
        VlWide<3>/*95:0*/ __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0;
        VlWide<3>/*95:0*/ __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0;
        IData/*31:0*/ __VactIterCount;
        QData/*63:0*/ tpu_top_tb__DOT__dut__DOT__uart_wt_wr_data;
        QData/*63:0*/ tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer;
        VlUnpacked<IData/*31:0*/, 32> tpu_top_tb__DOT__instr_memory;
        VlUnpacked<SData/*15:0*/, 4> tpu_top_tb__DOT__weights_layer1;
        VlUnpacked<SData/*15:0*/, 4> tpu_top_tb__DOT__weights_layer2;
        VlUnpacked<SData/*15:0*/, 4> tpu_top_tb__DOT__activations;
    };
    struct {
        VlUnpacked<IData/*23:0*/, 4096> tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0;
        VlUnpacked<IData/*23:0*/, 4096> tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1;
        VlUnpacked<VlUnpacked<CData/*7:0*/, 3>, 3> tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in;
        VlUnpacked<VlUnpacked<CData/*7:0*/, 3>, 3> tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out;
        VlUnpacked<VlUnpacked<IData/*31:0*/, 3>, 3> tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in;
        VlUnpacked<VlUnpacked<IData/*31:0*/, 3>, 3> tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out;
        VlUnpacked<VlWide<3>/*95:0*/, 256> tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0;
        VlUnpacked<VlWide<3>/*95:0*/, 256> tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1;
        VlUnpacked<IData/*31:0*/, 8> tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer;
        VlUnpacked<VlWide<8>/*255:0*/, 256> tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory;
        VlUnpacked<QData/*63:0*/, 2> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
        VlUnpacked<CData/*0:0*/, 7> __Vm_traceActivity;
    };
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h3d892c53__0;

    // INTERNAL VARIABLES
    Vtpu_top_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vtpu_top_tb___024root(Vtpu_top_tb__Syms* symsp, const char* v__name);
    ~Vtpu_top_tb___024root();
    VL_UNCOPYABLE(Vtpu_top_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
