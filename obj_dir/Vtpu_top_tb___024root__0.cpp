// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtpu_top_tb.h for the primary calling header

#include "Vtpu_top_tb__pch.h"

VL_ATTR_COLD void Vtpu_top_tb___024root___eval_initial__TOP(Vtpu_top_tb___024root* vlSelf);
VlCoroutine Vtpu_top_tb___024root___eval_initial__TOP__Vtiming__0(Vtpu_top_tb___024root* vlSelf);
VlCoroutine Vtpu_top_tb___024root___eval_initial__TOP__Vtiming__1(Vtpu_top_tb___024root* vlSelf);

void Vtpu_top_tb___024root___eval_initial(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_initial\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtpu_top_tb___024root___eval_initial__TOP(vlSelf);
    vlSelfRef.__Vm_traceActivity[1U] = 1U;
    Vtpu_top_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtpu_top_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

extern const VlWide<8>/*255:0*/ Vtpu_top_tb__ConstPool__CONST_h9e67c271_0;

VlCoroutine Vtpu_top_tb___024root___eval_initial__TOP__Vtiming__0(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.tpu_top_tb__DOT__rst_n = 0U;
    vlSelfRef.tpu_top_tb__DOT__dma_start_in = 0U;
    vlSelfRef.tpu_top_tb__DOT__dma_dir_in = 0U;
    vlSelfRef.tpu_top_tb__DOT__dma_ub_addr_in = 0U;
    vlSelfRef.tpu_top_tb__DOT__dma_length_in = 0U;
    vlSelfRef.tpu_top_tb__DOT__dma_elem_sz_in = 0U;
    vlSelfRef.tpu_top_tb__DOT__dma_data_in[0U] = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[0U];
    vlSelfRef.tpu_top_tb__DOT__dma_data_in[1U] = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[1U];
    vlSelfRef.tpu_top_tb__DOT__dma_data_in[2U] = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[2U];
    vlSelfRef.tpu_top_tb__DOT__dma_data_in[3U] = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[3U];
    vlSelfRef.tpu_top_tb__DOT__dma_data_in[4U] = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[4U];
    vlSelfRef.tpu_top_tb__DOT__dma_data_in[5U] = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[5U];
    vlSelfRef.tpu_top_tb__DOT__dma_data_in[6U] = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[6U];
    vlSelfRef.tpu_top_tb__DOT__dma_data_in[7U] = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[7U];
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "tpu_top_tb.sv", 
                                         155);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.tpu_top_tb__DOT__rst_n = 1U;
    VL_WRITEF_NX("Starting TPU testbench...\nTime: %0t - Reset deasserted\n",0,
                 64,VL_TIME_UNITED_Q(1000),-9);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "tpu_top_tb.sv", 
                                         161);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("Time: %0t - Loading weights for layer 1\n",0,
                 64,VL_TIME_UNITED_Q(1000),-9);
    vlSelfRef.tpu_top_tb__DOT__unnamedblk2__DOT__i = 0U;
    while (VL_GTS_III(32, 4U, vlSelfRef.tpu_top_tb__DOT__unnamedblk2__DOT__i)) {
        vlSelfRef.tpu_top_tb__DOT__dma_start_in = 1U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[0U] 
            = vlSelfRef.tpu_top_tb__DOT__weights_layer1
            [(3U & vlSelfRef.tpu_top_tb__DOT__unnamedblk2__DOT__i)];
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[1U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[2U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[3U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[4U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[5U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[6U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[7U] = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                             nullptr, 
                                             "tpu_top_tb.sv", 
                                             168);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
        vlSelfRef.tpu_top_tb__DOT__dma_start_in = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                             nullptr, 
                                             "tpu_top_tb.sv", 
                                             169);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
        vlSelfRef.tpu_top_tb__DOT__unnamedblk2__DOT__i 
            = ((IData)(1U) + vlSelfRef.tpu_top_tb__DOT__unnamedblk2__DOT__i);
    }
    VL_WRITEF_NX("Time: %0t - Loading activations\n",0,
                 64,VL_TIME_UNITED_Q(1000),-9);
    vlSelfRef.tpu_top_tb__DOT__unnamedblk3__DOT__i = 0U;
    while (VL_GTS_III(32, 4U, vlSelfRef.tpu_top_tb__DOT__unnamedblk3__DOT__i)) {
        vlSelfRef.tpu_top_tb__DOT__dma_start_in = 1U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[0U] 
            = vlSelfRef.tpu_top_tb__DOT__activations
            [(3U & vlSelfRef.tpu_top_tb__DOT__unnamedblk3__DOT__i)];
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[1U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[2U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[3U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[4U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[5U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[6U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dma_data_in[7U] = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                             nullptr, 
                                             "tpu_top_tb.sv", 
                                             177);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
        vlSelfRef.tpu_top_tb__DOT__dma_start_in = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                             nullptr, 
                                             "tpu_top_tb.sv", 
                                             178);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
        vlSelfRef.tpu_top_tb__DOT__unnamedblk3__DOT__i 
            = ((IData)(1U) + vlSelfRef.tpu_top_tb__DOT__unnamedblk3__DOT__i);
    }
    VL_WRITEF_NX("Time: %0t - Starting TPU program execution\n",0,
                 64,VL_TIME_UNITED_Q(1000),-9);
    vlSelfRef.tpu_top_tb__DOT__unnamedblk4__DOT__cycle_count = 0U;
    while ((0x03e8U > (IData)(vlSelfRef.tpu_top_tb__DOT__unnamedblk4__DOT__cycle_count))) {
        co_await vlSelfRef.__VtrigSched_h3d892c53__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge tpu_top_tb.clk)", 
                                                             "tpu_top_tb.sv", 
                                                             188);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
        vlSelfRef.tpu_top_tb__DOT__unnamedblk4__DOT__cycle_count 
            = (0x000003ffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__unnamedblk4__DOT__cycle_count)));
        if (VL_UNLIKELY(((1U == ((IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)
                                  ? 0U : ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid)
                                           ? 2U : 1U)))))) {
            VL_WRITEF_NX("Time: %0t - Stage 1: Fetch/Decode\n",0,
                         64,VL_TIME_UNITED_Q(1000),
                         -9);
        } else if (VL_UNLIKELY(((2U == ((IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)
                                         ? 0U : ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid)
                                                  ? 2U
                                                  : 1U)))))) {
            VL_WRITEF_NX("Time: %0t - Stage 2: Execute\n",0,
                         64,VL_TIME_UNITED_Q(1000),
                         -9);
        }
        if (VL_UNLIKELY((vlSelfRef.tpu_top_tb__DOT__tpu_busy))) {
            VL_WRITEF_NX("Time: %0t - HAZARD DETECTED: Pipeline stalled\n",0,
                         64,VL_TIME_UNITED_Q(1000),
                         -9);
        }
        if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)))))) {
            VL_WRITEF_NX("Time: %0t - TPU computation completed\n",0,
                         64,VL_TIME_UNITED_Q(1000),
                         -9);
            vlSelfRef.tpu_top_tb__DOT__unnamedblk4__DOT__cycle_count = 0x03e8U;
        }
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000001e8480ULL, 
                                         nullptr, "tpu_top_tb.sv", 
                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("Time: %0t - Checking results...\n",0,
                 64,VL_TIME_UNITED_Q(1000),-9);
    vlSelfRef.tpu_top_tb__DOT__dma_dir_in = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "tpu_top_tb.sv", 
                                         219);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("Time: %0t - Final result: %x\nTestbench completed\n",0,
                 64,VL_TIME_UNITED_Q(1000),-9,256,vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data.data());
    VL_FINISH_MT("tpu_top_tb.sv", 223, "");
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
}

VlCoroutine Vtpu_top_tb___024root___eval_initial__TOP__Vtiming__1(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "tpu_top_tb.sv", 
                                             13);
        vlSelfRef.tpu_top_tb__DOT__clk = (1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__clk)));
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtpu_top_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vtpu_top_tb___024root___eval_triggers__act(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_triggers__act\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                         << 3U) 
                                                        | (((~ (IData)(vlSelfRef.tpu_top_tb__DOT__rst_n)) 
                                                            & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__rst_n__0)) 
                                                           << 2U)) 
                                                       | ((((IData)(vlSelfRef.tpu_top_tb__DOT__clk) 
                                                            & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__clk__0))) 
                                                           << 1U) 
                                                          | ((IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy) 
                                                             != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__tpu_busy__0)))) 
                                                      << 4U) 
                                                     | (((((IData)(vlSelfRef.tpu_top_tb__DOT__pipeline_stage) 
                                                           != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__pipeline_stage__0)) 
                                                          << 3U) 
                                                         | (((IData)(vlSelfRef.tpu_top_tb__DOT__instr_addr_out) 
                                                             != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__instr_addr_out__0)) 
                                                            << 2U)) 
                                                        | ((((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel) 
                                                             != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__acc_buf_sel__1)) 
                                                            << 1U) 
                                                           | ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel) 
                                                              != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__wt_buf_sel__1)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__wt_buf_sel__1 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel;
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__acc_buf_sel__1 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel;
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__instr_addr_out__0 
        = vlSelfRef.tpu_top_tb__DOT__instr_addr_out;
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__pipeline_stage__0 
        = vlSelfRef.tpu_top_tb__DOT__pipeline_stage;
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__tpu_busy__0 
        = vlSelfRef.tpu_top_tb__DOT__tpu_busy;
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__clk__0 
        = vlSelfRef.tpu_top_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__rst_n__0 
        = vlSelfRef.tpu_top_tb__DOT__rst_n;
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.__VactDidInit)))))) {
        vlSelfRef.__VactDidInit = 1U;
        vlSelfRef.__VactTriggered[0U] = (1ULL | vlSelfRef.__VactTriggered
                                         [0U]);
        vlSelfRef.__VactTriggered[0U] = (2ULL | vlSelfRef.__VactTriggered
                                         [0U]);
        vlSelfRef.__VactTriggered[0U] = (4ULL | vlSelfRef.__VactTriggered
                                         [0U]);
        vlSelfRef.__VactTriggered[0U] = (8ULL | vlSelfRef.__VactTriggered
                                         [0U]);
        vlSelfRef.__VactTriggered[0U] = (0x0000000000000010ULL 
                                         | vlSelfRef.__VactTriggered
                                         [0U]);
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtpu_top_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vtpu_top_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vtpu_top_tb___024root___act_comb__TOP__0(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___act_comb__TOP__0\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_start 
        = vlSelfRef.tpu_top_tb__DOT__dma_start_in;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_dir = vlSelfRef.tpu_top_tb__DOT__dma_dir_in;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_ub_addr 
        = vlSelfRef.tpu_top_tb__DOT__dma_ub_addr_in;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_length 
        = vlSelfRef.tpu_top_tb__DOT__dma_length_in;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_elem_sz 
        = vlSelfRef.tpu_top_tb__DOT__dma_elem_sz_in;
    if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__use_uart_dma) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data 
            = (0x0000ffffU & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_data));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[0U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[1U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[2U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[3U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[3U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[4U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[4U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[5U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[5U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[6U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[6U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[7U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[7U];
    } else {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data 
            = (0x0000ffffU & vlSelfRef.tpu_top_tb__DOT__dma_data_in[0U]);
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[0U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[1U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[2U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[3U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[3U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[4U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[4U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[5U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[5U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[6U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[6U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[7U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[7U];
    }
}

void Vtpu_top_tb___024root___act_sequent__TOP__0(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___act_sequent__TOP__0\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid) {
        if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                      >> 5U)))) {
            if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                          >> 4U)))) {
                if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                              >> 3U)))) {
                    if ((4U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode))) {
                        if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                                      >> 1U)))) {
                            if ((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode)))) {
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel 
                                    = (1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel)));
                            }
                        }
                    }
                }
            }
        }
    }
}

void Vtpu_top_tb___024root___act_sequent__TOP__1(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___act_sequent__TOP__1\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid) {
        if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                      >> 5U)))) {
            if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                          >> 4U)))) {
                if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                              >> 3U)))) {
                    if ((4U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode))) {
                        if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                                      >> 1U)))) {
                            if ((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode)))) {
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel 
                                    = (1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel)));
                            }
                        }
                    }
                }
            }
        }
    }
    if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full 
            = (0x1000U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_empty 
            = ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_count) 
               == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count));
    } else {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full 
            = (0x1000U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_empty 
            = ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_count) 
               == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count));
    }
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_en 
        = ((~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_empty)) 
           & (2U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state)));
}

void Vtpu_top_tb___024root___eval_act(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_act\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x00000000000000a0ULL & vlSelfRef.__VactTriggered
         [0U])) {
        Vtpu_top_tb___024root___act_comb__TOP__0(vlSelf);
        vlSelfRef.__Vm_traceActivity[3U] = 1U;
    }
    if ((2ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtpu_top_tb___024root___act_sequent__TOP__0(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtpu_top_tb___024root___act_sequent__TOP__1(vlSelf);
    }
}

void Vtpu_top_tb___024root___nba_sequent__TOP__0(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___nba_sequent__TOP__0\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0 = 0U;
    vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0 = 0U;
    vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0 = 0U;
    vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0 = 0U;
    if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_fifo_wr) {
        if ((1U & ((~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel)) 
                   & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full))))) {
            vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data;
            vlSelfRef.__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_ptr;
            vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0 = 1U;
        }
        if ((1U & (~ ((~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel)) 
                      & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full)))))) {
            if (((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel) 
                 & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full)))) {
                vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data;
                vlSelfRef.__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_ptr;
                vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0 = 1U;
            }
        }
    }
    if ((2U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))) {
        if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel) {
            vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0[0U] 
                = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U])) 
                            << 0x00000020U) | (QData)((IData)(
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U]))));
            vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0[1U] 
                = (IData)(((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U])) 
                             << 0x00000020U) | (QData)((IData)(
                                                               vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U]))) 
                           >> 0x00000020U));
            vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0[2U] = 0U;
            vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0 = 1U;
        }
        if ((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel)))) {
            vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0[0U] 
                = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U])) 
                            << 0x00000020U) | (QData)((IData)(
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U]))));
            vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0[1U] 
                = (IData)(((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U])) 
                             << 0x00000020U) | (QData)((IData)(
                                                               vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U]))) 
                           >> 0x00000020U));
            vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0[2U] = 0U;
            vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0 = 1U;
        }
    }
}

extern const VlWide<8>/*255:0*/ Vtpu_top_tb__ConstPool__CONST_h5a979007_0;
extern const VlUnpacked<CData/*0:0*/, 256> Vtpu_top_tb__ConstPool__TABLE_h2e55c492_0;
extern const VlUnpacked<CData/*0:0*/, 256> Vtpu_top_tb__ConstPool__TABLE_had101f60_0;
extern const VlUnpacked<CData/*0:0*/, 256> Vtpu_top_tb__ConstPool__TABLE_h2ef52e1e_0;

void Vtpu_top_tb___024root___nba_sequent__TOP__1(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___nba_sequent__TOP__1\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    CData/*7:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg;
    __Vdly__tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg = 0;
    CData/*7:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0;
    VlWide<8>/*255:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer;
    VL_ZERO_W(256, __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer);
    CData/*7:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index = 0;
    SData/*15:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count = 0;
    IData/*31:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer = 0;
    QData/*63:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer = 0;
    CData/*4:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index = 0;
    CData/*7:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo = 0;
    CData/*7:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi = 0;
    VlWide<8>/*255:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer;
    VL_ZERO_W(256, __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer);
    SData/*15:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count = 0;
    CData/*1:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state = 0;
    CData/*2:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index = 0;
    SData/*15:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count = 0;
    CData/*1:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state = 0;
    CData/*2:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index = 0;
    SData/*11:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr = 0;
    SData/*11:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr = 0;
    IData/*31:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result = 0;
    IData/*31:0*/ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result;
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result = 0;
    IData/*31:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result = 0;
    IData/*31:0*/ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result;
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result = 0;
    IData/*31:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result = 0;
    IData/*31:0*/ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result;
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result = 0;
    IData/*31:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result = 0;
    IData/*31:0*/ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result;
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result = 0;
    IData/*31:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result = 0;
    IData/*31:0*/ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result;
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result = 0;
    IData/*31:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result = 0;
    IData/*31:0*/ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result;
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result = 0;
    IData/*31:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result = 0;
    IData/*31:0*/ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result;
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result = 0;
    IData/*31:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result = 0;
    IData/*31:0*/ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result;
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result = 0;
    IData/*31:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result = 0;
    IData/*31:0*/ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result;
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result = 0;
    CData/*1:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state = 0;
    CData/*7:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter = 0;
    CData/*1:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state = 0;
    CData/*7:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr = 0;
    CData/*7:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count = 0;
    CData/*1:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state = 0;
    CData/*7:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr = 0;
    CData/*7:0*/ __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count = 0;
    IData/*31:0*/ __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0;
    __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0 = 0;
    CData/*2:0*/ __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0;
    __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0 = 0;
    CData/*0:0*/ __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0;
    __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0 = 0;
    IData/*31:0*/ __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v1;
    __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v1 = 0;
    CData/*2:0*/ __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v1;
    __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v1 = 0;
    IData/*31:0*/ __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2;
    __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2 = 0;
    CData/*2:0*/ __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2;
    __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2 = 0;
    CData/*0:0*/ __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2;
    __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2 = 0;
    IData/*31:0*/ __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v3;
    __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v3 = 0;
    CData/*2:0*/ __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v3;
    __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v3 = 0;
    VlWide<8>/*255:0*/ __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0;
    VL_ZERO_W(256, __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0);
    CData/*7:0*/ __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0;
    __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0 = 0;
    CData/*0:0*/ __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0;
    __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0 = 0;
    VlWide<8>/*255:0*/ __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1;
    VL_ZERO_W(256, __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1);
    CData/*7:0*/ __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1;
    __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1 = 0;
    CData/*0:0*/ __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1;
    __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1 = 0;
    // Body
    __Vdly__tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state;
    __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0 = 0U;
    __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2 = 0U;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr;
    __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count;
    __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0 = 0U;
    __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1 = 0U;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[0U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[1U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[2U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[3U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[3U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[4U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[4U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[5U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[5U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[6U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[6U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[7U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[7U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[7U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[7U];
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state;
    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index;
    if ((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__rst_n)))) {
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result = 0U;
        __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result = 0xffffffffU;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result = 0U;
        __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result = 0xffffffffU;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result = 0U;
        __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result = 0xffffffffU;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result = 0U;
        __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result = 0xffffffffU;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result = 0U;
        __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result = 0xffffffffU;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result = 0U;
        __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result = 0xffffffffU;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result = 0U;
        __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result = 0xffffffffU;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result = 0U;
        __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result = 0xffffffffU;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result = 0U;
        __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result = 0xffffffffU;
    }
    if (vlSelfRef.tpu_top_tb__DOT__rst_n) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mult_result 
            = (0x0000ffffU & VL_MULS_III(16, (0x0000ffffU 
                                              & VL_EXTENDS_II(16,8, 
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                                              [0U]
                                                              [0U])), 
                                         (0x0000ffffU 
                                          & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__weight_reg)))));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result 
            = (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
               [0U][0U] + VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mult_result)));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mult_result 
            = (0x0000ffffU & VL_MULS_III(16, (0x0000ffffU 
                                              & VL_EXTENDS_II(16,8, 
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                                              [0U]
                                                              [1U])), 
                                         (0x0000ffffU 
                                          & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__weight_reg)))));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result 
            = (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
               [0U][1U] + VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mult_result)));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mult_result 
            = (0x0000ffffU & VL_MULS_III(16, (0x0000ffffU 
                                              & VL_EXTENDS_II(16,8, 
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                                              [1U]
                                                              [0U])), 
                                         (0x0000ffffU 
                                          & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__weight_reg)))));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result 
            = (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
               [1U][0U] + VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mult_result)));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mult_result 
            = (0x0000ffffU & VL_MULS_III(16, (0x0000ffffU 
                                              & VL_EXTENDS_II(16,8, 
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                                              [1U]
                                                              [1U])), 
                                         (0x0000ffffU 
                                          & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__weight_reg)))));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result 
            = (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
               [1U][1U] + VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mult_result)));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mult_result 
            = (0x0000ffffU & VL_MULS_III(16, (0x0000ffffU 
                                              & VL_EXTENDS_II(16,8, 
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                                              [0U]
                                                              [2U])), 
                                         (0x0000ffffU 
                                          & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__weight_reg)))));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result 
            = (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
               [0U][2U] + VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mult_result)));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mult_result 
            = (0x0000ffffU & VL_MULS_III(16, (0x0000ffffU 
                                              & VL_EXTENDS_II(16,8, 
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                                              [1U]
                                                              [2U])), 
                                         (0x0000ffffU 
                                          & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__weight_reg)))));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result 
            = (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
               [1U][2U] + VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mult_result)));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mult_result 
            = (0x0000ffffU & VL_MULS_III(16, (0x0000ffffU 
                                              & VL_EXTENDS_II(16,8, 
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                                              [2U]
                                                              [0U])), 
                                         (0x0000ffffU 
                                          & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__weight_reg)))));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result 
            = (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
               [2U][0U] + VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mult_result)));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mult_result 
            = (0x0000ffffU & VL_MULS_III(16, (0x0000ffffU 
                                              & VL_EXTENDS_II(16,8, 
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                                              [2U]
                                                              [1U])), 
                                         (0x0000ffffU 
                                          & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__weight_reg)))));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result 
            = (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
               [2U][1U] + VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mult_result)));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mult_result 
            = (0x0000ffffU & VL_MULS_III(16, (0x0000ffffU 
                                              & VL_EXTENDS_II(16,8, 
                                                              vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                                              [2U]
                                                              [2U])), 
                                         (0x0000ffffU 
                                          & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__weight_reg)))));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result 
            = (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
               [2U][2U] + VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mult_result)));
        if ((((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_en) 
              & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel))) 
             & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_empty)))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr 
                = (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr)));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_count 
                = (0x00001fffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_count)));
        }
        if ((((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_en) 
              & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel)) 
             & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_empty)))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr 
                = (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr)));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_count 
                = (0x00001fffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_count)));
        }
        if ((((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_fifo_wr) 
              & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel))) 
             & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full)))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_ptr 
                = (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_ptr)));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count 
                = (0x00001fffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count)));
        }
        if ((((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_fifo_wr) 
              & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel)) 
             & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full)))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_ptr 
                = (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_ptr)));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count 
                = (0x00001fffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count)));
        }
        if ((1U & (~ ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state)) 
                      & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_start))))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__compute_counter 
                = ((2U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))
                    ? (0x000000ffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__compute_counter)))
                    : 0U);
        }
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__weight_load_counter 
            = ((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))
                ? (0x000000ffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__weight_load_counter)))
                : 0U);
        if (((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state)) 
             & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_start))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__total_rows 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_rows;
        }
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_00__act_out 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
            [0U][0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_01__act_out 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
            [0U][1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_10__act_out 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
            [1U][0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_11__act_out 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
            [1U][1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_02__act_out 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
            [0U][2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_12__act_out 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
            [1U][2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_20__act_out 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
            [2U][0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_21__act_out 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
            [2U][1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_22__act_out 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
            [2U][2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row1_delayed 
            = (0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[0U] 
                              >> 8U));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_flags 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_flags;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg2 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg2;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg1 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg1;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg3 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg3;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_opcode;
        if ((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg 
                = (0x000000ffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg)));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_unit_sel = 0U;
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_pc 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg;
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_flags 
                = (3U & vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg);
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg2 
                = (0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                                  >> 0x0000000aU));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg1 
                = (0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                                  >> 0x00000012U));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg3 
                = (0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                                  >> 2U));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_opcode 
                = (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                   >> 0x0000001aU);
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg = 0U;
        }
        if ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_b_ready = 1U;
            if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_valid) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state = 1U;
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr = 0U;
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count = 1U;
            }
        } else if ((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count 
                = (0x000000ffU & ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count) 
                                  - (IData)(1U)));
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[0U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[0U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[1U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[1U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[2U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[2U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[3U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[3U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[4U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[4U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[5U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[5U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[6U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[6U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[7U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[7U];
            __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr;
            __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0 = 1U;
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_b_ready = 1U;
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state 
                = ((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count))
                    ? 0U : 2U);
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr 
                = (0x000000ffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr)));
        } else if ((2U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count 
                = (0x000000ffU & ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count) 
                                  - (IData)(1U)));
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[0U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[0U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[1U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[1U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[2U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[2U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[3U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[3U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[4U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[4U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[5U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[5U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[6U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[6U];
            __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[7U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[7U];
            __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr;
            __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1 = 1U;
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_b_ready = 1U;
            if ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count))) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state = 0U;
            }
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr 
                = (0x000000ffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr)));
        } else {
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state = 0U;
        }
        if ((2U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state))) {
            if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state))) {
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_tx = 1U;
                if ((0x0363U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count))) {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count = 0U;
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state = 0U;
                } else {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count)));
                }
            } else {
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_tx 
                    = (1U & ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__tx_byte) 
                             >> (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index)));
                if ((0x0363U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count))) {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count = 0U;
                    if ((7U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index))) {
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index = 0U;
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state = 3U;
                    } else {
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index 
                            = (7U & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index)));
                    }
                } else {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count)));
                }
            }
        } else if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_tx = 0U;
            if ((0x0363U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count))) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count = 0U;
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state = 2U;
            } else {
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count)));
            }
        } else {
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count = 0U;
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index = 0U;
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_tx = 1U;
            if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_valid) {
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__tx_byte 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_data;
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state = 1U;
            }
        }
        if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col0) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__weight_reg 
                = (0x000000ffU & vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data);
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__weight_reg 
                = (0x000000ffU & vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data);
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__weight_reg 
                = (0x000000ffU & vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data);
        }
        if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col1) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__weight_reg 
                = (0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
                                  >> 8U));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__weight_reg 
                = (0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
                                  >> 8U));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__weight_reg 
                = (0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
                                  >> 8U));
        }
        if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col2) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__weight_reg 
                = (0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
                                  >> 0x00000010U));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__weight_reg 
                = (0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
                                  >> 0x00000010U));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__weight_reg 
                = (0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
                                  >> 0x00000010U));
        }
        if ((2U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state))) {
            if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state))) {
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_valid = 0U;
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state = 0U;
            } else {
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[0U] 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                    [0U];
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[1U] 
                    = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                [2U])) 
                                << 0x00000020U) | (QData)((IData)(
                                                                  vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                                  [1U]))));
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[2U] 
                    = (IData)(((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                 [2U])) 
                                 << 0x00000020U) | (QData)((IData)(
                                                                   vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                                   [1U]))) 
                               >> 0x00000020U));
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[3U] 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                    [3U];
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[4U] 
                    = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                [5U])) 
                                << 0x00000020U) | (QData)((IData)(
                                                                  vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                                  [4U]))));
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[5U] 
                    = (IData)(((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                 [5U])) 
                                 << 0x00000020U) | (QData)((IData)(
                                                                   vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                                   [4U]))) 
                               >> 0x00000020U));
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[6U] 
                    = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                [7U])) 
                                << 0x00000020U) | (QData)((IData)(
                                                                  vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                                  [6U]))));
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[7U] 
                    = (IData)(((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                 [7U])) 
                                 << 0x00000020U) | (QData)((IData)(
                                                                   vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer
                                                                   [6U]))) 
                               >> 0x00000020U));
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_valid = 1U;
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state = 3U;
            }
        } else if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state))) {
            if ((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_mode))) {
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result0 
                    = (VL_LTS_III(32, 0U, vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[0U])
                        ? vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[0U]
                        : 0U);
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result1 
                    = (VL_LTS_III(32, 0U, vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[1U])
                        ? vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[1U]
                        : 0U);
                __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result0;
                __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0 
                    = (7U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter));
                __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0 = 1U;
                __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v1 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result1;
                __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v1 
                    = (7U & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter)));
            } else {
                __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[0U];
                __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2 
                    = (7U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter));
                __VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2 = 1U;
                __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v3 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[1U];
                __VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v3 
                    = (7U & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter)));
            }
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter 
                = (0x000000ffU & ((IData)(2U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter)));
            if ((6U <= (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter))) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state = 2U;
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter = 0U;
            }
        } else {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_valid = 0U;
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter = 0U;
            if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_start) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state = 1U;
            }
        }
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_en = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_en = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_start_execution = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_valid = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_state 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_cmd 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_byte_count 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count;
        if ((0x00000080U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
        } else if ((0x00000040U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
        } else if ((0x00000020U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
        } else if ((0x00000010U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
        } else if ((8U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
            if ((4U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
            } else if ((2U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
                if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_start_execution = 1U;
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
                } else if ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state))) {
                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_valid = 1U;
                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_data 
                        = (0x0000002aU | (((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state)) 
                                           << 2U) | 
                                          (0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
                }
            } else if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
                if ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count))) {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[0U] 
                        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[0U];
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[1U] 
                        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[1U];
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[2U] 
                        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[2U];
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[3U] 
                        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[3U];
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[4U] 
                        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[4U];
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[5U] 
                        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[5U];
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[6U] 
                        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[6U];
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[7U] 
                        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[7U];
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index = 0U;
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count = 1U;
                } else if (((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state)) 
                            & (0x20U > (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index)))) {
                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_valid = 1U;
                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_data 
                        = (0x000000ffU & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[0U]);
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index 
                        = (0x000000ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index)));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[0U] 
                        = (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[0U] 
                           & ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[1U] 
                               << 0x00000018U) | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[0U] 
                                                  >> 8U)));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[1U] 
                        = (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[1U] 
                           & ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[2U] 
                               << 0x00000018U) | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[1U] 
                                                  >> 8U)));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[2U] 
                        = (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[2U] 
                           & ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[3U] 
                               << 0x00000018U) | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[2U] 
                                                  >> 8U)));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[3U] 
                        = (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[3U] 
                           & ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[4U] 
                               << 0x00000018U) | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[3U] 
                                                  >> 8U)));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[4U] 
                        = (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[4U] 
                           & ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[5U] 
                               << 0x00000018U) | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[4U] 
                                                  >> 8U)));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[5U] 
                        = (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[5U] 
                           & ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[6U] 
                               << 0x00000018U) | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[5U] 
                                                  >> 8U)));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[6U] 
                        = (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[6U] 
                           & ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[7U] 
                               << 0x00000018U) | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[6U] 
                                                  >> 8U)));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[7U] 
                        = (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[7U] 
                           & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[7U] 
                              >> 8U));
                    if ((0x1fU == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index))) {
                        if (((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count) 
                             >= (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length))) {
                            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
                        } else {
                            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count 
                                = (0x0000ffffU & ((IData)(0x0020U) 
                                                  + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count)));
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_addr 
                                = (0x000000ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_addr)));
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en = 1U;
                            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index = 0U;
                        }
                    }
                }
            } else if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer 
                    = ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer 
                        << 8U) | (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data));
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count)));
                if ((3U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count))) {
                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_en = 1U;
                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_addr 
                        = (0x0000001fU & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo));
                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_data 
                        = ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer 
                            << 8U) | (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
                }
            }
        } else if ((4U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
            if ((2U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
                if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
                    if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid) {
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer 
                            = ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer 
                                << 8U) | (QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data)));
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index 
                            = (0x0000001fU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index)));
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count)));
                        if ((7U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index))) {
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_en = 1U;
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_addr 
                                = ((0x00000300U & ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi) 
                                                   << 8U)) 
                                   | (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo));
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_data 
                                = ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer 
                                    << 8U) | (QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data)));
                            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index = 0U;
                            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo 
                                = (0x000000ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo)));
                            if ((0xffU == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo))) {
                                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi 
                                    = (0x000000ffU 
                                       & ((IData)(1U) 
                                          + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi)));
                            }
                        }
                        if ((((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count)) 
                             >= (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length))) {
                            if ((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index))) {
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_en = 1U;
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_addr 
                                    = ((0x00000300U 
                                        & ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi) 
                                           << 8U)) 
                                       | (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo));
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_data 
                                    = ((vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer 
                                        << 8U) | (QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data)));
                            }
                            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
                        }
                    }
                } else if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid) {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U] 
                        = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[0U] 
                             & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U]) 
                            << 8U) | (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U] 
                        = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[0U] 
                             & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U]) 
                            >> 0x00000018U) | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[1U] 
                                                & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U]) 
                                               << 8U));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U] 
                        = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[1U] 
                             & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U]) 
                            >> 0x00000018U) | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[2U] 
                                                & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U]) 
                                               << 8U));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U] 
                        = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[2U] 
                             & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U]) 
                            >> 0x00000018U) | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[3U] 
                                                & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U]) 
                                               << 8U));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U] 
                        = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[3U] 
                             & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U]) 
                            >> 0x00000018U) | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[4U] 
                                                & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U]) 
                                               << 8U));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U] 
                        = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[4U] 
                             & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U]) 
                            >> 0x00000018U) | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[5U] 
                                                & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U]) 
                                               << 8U));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U] 
                        = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[5U] 
                             & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U]) 
                            >> 0x00000018U) | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[6U] 
                                                & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U]) 
                                               << 8U));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[7U] 
                        = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[6U] 
                             & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U]) 
                            >> 0x00000018U) | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[7U] 
                                                & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[7U]) 
                                               << 8U));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index 
                        = (0x0000001fU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index)));
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count)));
                    if ((0x1fU == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index))) {
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en = 1U;
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_addr 
                            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo;
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[0U] 
                            = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[0U] 
                                 & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U]) 
                                << 8U) | (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data));
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[1U] 
                            = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[0U] 
                                 & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U]) 
                                >> 0x00000018U) | (
                                                   (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[1U] 
                                                    & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U]) 
                                                   << 8U));
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[2U] 
                            = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[1U] 
                                 & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U]) 
                                >> 0x00000018U) | (
                                                   (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[2U] 
                                                    & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U]) 
                                                   << 8U));
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[3U] 
                            = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[2U] 
                                 & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U]) 
                                >> 0x00000018U) | (
                                                   (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[3U] 
                                                    & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U]) 
                                                   << 8U));
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[4U] 
                            = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[3U] 
                                 & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U]) 
                                >> 0x00000018U) | (
                                                   (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[4U] 
                                                    & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U]) 
                                                   << 8U));
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[5U] 
                            = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[4U] 
                                 & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U]) 
                                >> 0x00000018U) | (
                                                   (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[5U] 
                                                    & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U]) 
                                                   << 8U));
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[6U] 
                            = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[5U] 
                                 & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U]) 
                                >> 0x00000018U) | (
                                                   (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[6U] 
                                                    & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U]) 
                                                   << 8U));
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[7U] 
                            = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[6U] 
                                 & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U]) 
                                >> 0x00000018U) | (
                                                   (Vtpu_top_tb__ConstPool__CONST_h5a979007_0[7U] 
                                                    & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[7U]) 
                                                   << 8U));
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index = 0U;
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo)));
                    }
                    if ((((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count)) 
                         >= (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length))) {
                        if ((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index))) {
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en = 1U;
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_addr 
                                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo;
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[0U] 
                                = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[0U] 
                                     & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U]) 
                                    << 8U) | (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data));
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[1U] 
                                = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[0U] 
                                     & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U]) 
                                    >> 0x00000018U) 
                                   | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[1U] 
                                       & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U]) 
                                      << 8U));
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[2U] 
                                = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[1U] 
                                     & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U]) 
                                    >> 0x00000018U) 
                                   | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[2U] 
                                       & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U]) 
                                      << 8U));
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[3U] 
                                = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[2U] 
                                     & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U]) 
                                    >> 0x00000018U) 
                                   | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[3U] 
                                       & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U]) 
                                      << 8U));
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[4U] 
                                = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[3U] 
                                     & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U]) 
                                    >> 0x00000018U) 
                                   | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[4U] 
                                       & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U]) 
                                      << 8U));
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[5U] 
                                = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[4U] 
                                     & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U]) 
                                    >> 0x00000018U) 
                                   | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[5U] 
                                       & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U]) 
                                      << 8U));
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[6U] 
                                = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[5U] 
                                     & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U]) 
                                    >> 0x00000018U) 
                                   | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[6U] 
                                       & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U]) 
                                      << 8U));
                            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[7U] 
                                = (((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[6U] 
                                     & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U]) 
                                    >> 0x00000018U) 
                                   | ((Vtpu_top_tb__ConstPool__CONST_h5a979007_0[7U] 
                                       & vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[7U]) 
                                      << 8U));
                        }
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
                    }
                }
            } else if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
                if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid) {
                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length 
                        = ((0xff00U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length)) 
                           | (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data));
                    if ((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command))) {
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 6U;
                    } else if ((2U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command))) {
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 7U;
                    } else if ((4U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command))) {
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_addr 
                            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo;
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en = 1U;
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 9U;
                    } else {
                        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
                    }
                }
            } else if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid) {
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length 
                    = ((0x00ffU & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length)) 
                       | ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data) 
                          << 8U));
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 5U;
            }
        } else if ((2U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
            if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
                if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid) {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo 
                        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data;
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state 
                        = ((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command))
                            ? 4U : ((2U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command))
                                     ? 4U : ((3U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command))
                                              ? 8U : 
                                             ((4U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command))
                                               ? 4U
                                               : 0U))));
                }
            } else if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data;
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 3U;
            }
        } else if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
        } else if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data;
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count = 0U;
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index = 0U;
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state 
                = ((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data))
                    ? 2U : ((2U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data))
                             ? 2U : ((3U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data))
                                      ? 2U : ((4U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data))
                                               ? 2U
                                               : ((5U 
                                                   == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data))
                                                   ? 0x0bU
                                                   : 
                                                  ((6U 
                                                    == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data))
                                                    ? 0x0aU
                                                    : 0U))))));
        }
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
            = ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel)
                ? vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1
               [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr]
                : vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0
               [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr]);
        if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_start) {
            if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel) {
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[0U] 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1
                    [0U][0U];
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[1U] 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1
                    [0U][1U];
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[2U] 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1
                    [0U][2U];
            } else {
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[0U] 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0
                    [0U][0U];
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[1U] 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0
                    [0U][1U];
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[2U] 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0
                    [0U][2U];
            }
        }
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid = 0U;
        if ((2U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state))) {
            if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state))) {
                if ((0x0363U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count))) {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count = 0U;
                    if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_2) {
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data 
                            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_byte;
                        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid = 1U;
                    }
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state = 0U;
                } else {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count)));
                }
            } else if ((0x0363U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count))) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count = 0U;
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_byte 
                    = (((~ ((IData)(1U) << (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index))) 
                        & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_byte)) 
                       | (0x00ffU & ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_2) 
                                     << (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index))));
                if ((7U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index))) {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index = 0U;
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state = 3U;
                } else {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index 
                        = (7U & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index)));
                }
            } else {
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count)));
            }
        } else if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state))) {
            if ((0x01b1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count))) {
                if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_2) {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state = 0U;
                } else {
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count = 0U;
                    __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state = 2U;
                }
            } else {
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count)));
            }
        } else {
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index = 0U;
            __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count = 0U;
            if ((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_2)))) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state = 1U;
            }
        }
        if ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_a_valid = 0U;
            if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_start) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state = 1U;
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_addr;
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count 
                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_rows;
            }
        } else if ((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state))) {
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count 
                = (0x000000ffU & ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count) 
                                  - (IData)(1U)));
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[0U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][0U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[1U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][1U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[2U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][2U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[3U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][3U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[4U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][4U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[5U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][5U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[6U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][6U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[7U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][7U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_a_valid = 1U;
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state 
                = ((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count))
                    ? 0U : 2U);
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr 
                = (0x000000ffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr)));
        } else if ((2U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[0U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][0U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[1U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][1U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[2U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][2U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[3U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][3U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[4U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][4U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[5U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][5U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[6U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][6U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[7U] 
                = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory
                [vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr][7U];
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_a_valid = 1U;
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr 
                = (0x000000ffU & ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr)));
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count 
                = (0x000000ffU & ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count) 
                                  - (IData)(1U)));
            if ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count))) {
                __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state = 0U;
                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_a_valid = 0U;
            }
        } else {
            __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state = 0U;
        }
    } else {
        __Vdly__tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_ptr = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_ptr = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_count = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_count = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__compute_counter = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__weight_load_counter = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_unit_sel = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_pc = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__total_rows = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_00__act_out = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_01__act_out = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_10__act_out = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_11__act_out = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_02__act_out = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_12__act_out = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_20__act_out = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_21__act_out = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_22__act_out = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row1_delayed = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_flags = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg2 = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg1 = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg3 = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_b_ready = 1U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_tx = 1U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__tx_byte = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__weight_reg = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__weight_reg = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__weight_reg = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__weight_reg = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__weight_reg = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__weight_reg = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__weight_reg = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__weight_reg = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__weight_reg = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_flags = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg2 = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg1 = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg3 = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_opcode = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_valid = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[0U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[1U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[2U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[3U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[3U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[4U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[4U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[5U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[5U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[6U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[6U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data[7U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[7U];
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_en = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_en = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_start_execution = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_valid = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_state = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_cmd = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_byte_count = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[0U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[1U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[2U] = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_byte = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count = 0U;
        __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr = 0U;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[0U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[1U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[2U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[3U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[3U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[4U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[4U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[5U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[5U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[6U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[6U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[7U] 
            = Vtpu_top_tb__ConstPool__CONST_h9e67c271_0[7U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_a_valid = 0U;
    }
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result 
        = ((__Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result 
            & __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result) 
           | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result 
              & (~ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result)));
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result 
        = ((__Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result 
            & __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result) 
           | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result 
              & (~ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result)));
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result 
        = ((__Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result 
            & __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result) 
           | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result 
              & (~ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result)));
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result 
        = ((__Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result 
            & __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result) 
           | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result 
              & (~ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result)));
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result 
        = ((__Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result 
            & __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result) 
           | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result 
              & (~ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result)));
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result 
        = ((__Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result 
            & __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result) 
           | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result 
              & (~ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result)));
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result 
        = ((__Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result 
            & __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result) 
           | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result 
              & (~ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result)));
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result 
        = ((__Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result 
            & __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result) 
           | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result 
              & (~ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result)));
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result 
        = ((__Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result 
            & __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result) 
           | (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result 
              & (~ __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result)));
    __VdlyMask__tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row2_delayed = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out[0U][0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out[0U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out[1U][0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out[1U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out[0U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out[1U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out[2U][0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out[2U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out[2U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U] 
        = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                    [1U][0U])) << 0x00000020U) 
                   | (QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                     [0U][0U]))));
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U] 
        = (IData)(((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                     [1U][0U])) << 0x00000020U) 
                    | (QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                      [0U][0U]))) >> 0x00000020U));
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
        [2U][0U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in[1U][0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
        [0U][0U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in[1U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
        [0U][1U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in[1U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
        [0U][2U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in[2U][0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
        [1U][0U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in[2U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
        [1U][1U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in[2U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
        [1U][2U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out[0U][0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_00__act_out;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out[0U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_01__act_out;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out[1U][0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_10__act_out;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out[1U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_11__act_out;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out[0U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_02__act_out;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out[1U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_12__act_out;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out[2U][0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_20__act_out;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out[2U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_21__act_out;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out[2U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_22__act_out;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[1U][0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row1_delayed;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[2U][0U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row2_delayed;
    __Vtableidx1 = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__weight_load_counter;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col0 
        = Vtpu_top_tb__ConstPool__TABLE_h2e55c492_0
        [__Vtableidx1];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col1 
        = Vtpu_top_tb__ConstPool__TABLE_had101f60_0
        [__Vtableidx1];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col2 
        = Vtpu_top_tb__ConstPool__TABLE_h2ef52e1e_0
        [__Vtableidx1];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[0U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
        [0U][0U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[0U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
        [0U][1U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[1U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
        [1U][0U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[1U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
        [1U][1U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[2U][1U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
        [2U][0U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[2U][2U] 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
        [2U][1U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter;
    if (__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v0;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v1] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v1;
    }
    if (__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v2;
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v3] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer__v3;
    }
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[0U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[0U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[1U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[1U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[2U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[2U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[3U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[3U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[4U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[4U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[5U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[5U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[6U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[6U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[7U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer[7U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[0U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[1U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[2U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[3U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[4U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[5U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[6U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[7U] 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer[7U];
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__use_uart_dma 
        = ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en) 
           | ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en) 
              | ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_en) 
                 | (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_en))));
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count 
        = __Vdly__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count;
    if (__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0][0U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0][1U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0][2U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0][3U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[3U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0][4U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[4U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0][5U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[5U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0][6U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[6U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0][7U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v0[7U];
    }
    if (__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1][0U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1][1U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1][2U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1][3U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[3U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1][4U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[4U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1][5U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[5U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1][6U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[6U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1][7U] 
            = __VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory__v1[7U];
    }
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[0U][0U] 
        = (0x000000ffU & vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[0U]);
}

void Vtpu_top_tb___024root___nba_sequent__TOP__2(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___nba_sequent__TOP__2\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSymsp->TOP____024unit.__VmonitorOff)))))) {
        VL_WRITEF_NX("Time: %0t | PC: %x | Stage: %b | Hazard: %b | Busy: %b | Done: %b\n",0,
                     64,VL_TIME_UNITED_Q(1000),-9,8,
                     (IData)(vlSelfRef.tpu_top_tb__DOT__instr_addr_out),
                     2,((IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)
                         ? 0U : ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid)
                                  ? 2U : 1U)),1,(IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy),
                     1,vlSelfRef.tpu_top_tb__DOT__tpu_busy,
                     1,(1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy))));
    }
}

void Vtpu_top_tb___024root___nba_comb__TOP__0(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___nba_comb__TOP__0\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_start 
        = vlSelfRef.tpu_top_tb__DOT__dma_start_in;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_dir = vlSelfRef.tpu_top_tb__DOT__dma_dir_in;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_ub_addr 
        = vlSelfRef.tpu_top_tb__DOT__dma_ub_addr_in;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_length 
        = vlSelfRef.tpu_top_tb__DOT__dma_length_in;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_elem_sz 
        = vlSelfRef.tpu_top_tb__DOT__dma_elem_sz_in;
}

void Vtpu_top_tb___024root___nba_sequent__TOP__3(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___nba_sequent__TOP__3\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1[vlSelfRef.__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0] 
            = vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0;
    }
    if (vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0[vlSelfRef.__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0] 
            = vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0;
    }
    if (vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1[0U][0U] 
            = vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0[0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1[0U][1U] 
            = vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0[1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1[0U][2U] 
            = vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0[2U];
    }
    if (vlSelfRef.__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0[0U][0U] 
            = vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0[0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0[0U][1U] 
            = vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0[1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0[0U][2U] 
            = vlSelfRef.__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0[2U];
    }
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_2 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_1;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_1 = 0U;
}

void Vtpu_top_tb___024root___nba_comb__TOP__1(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___nba_comb__TOP__1\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__use_uart_dma) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data 
            = (0x0000ffffU & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_data));
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[0U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[1U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[2U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[3U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[3U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[4U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[4U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[5U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[5U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[6U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[6U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[7U] 
            = vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data[7U];
    } else {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data 
            = (0x0000ffffU & vlSelfRef.tpu_top_tb__DOT__dma_data_in[0U]);
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[0U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[0U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[1U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[1U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[2U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[2U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[3U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[3U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[4U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[4U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[5U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[5U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[6U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[6U];
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data[7U] 
            = vlSelfRef.tpu_top_tb__DOT__dma_data_in[7U];
    }
}

void Vtpu_top_tb___024root___nba_sequent__TOP__4(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___nba_sequent__TOP__4\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid 
        = ((IData)(vlSelfRef.tpu_top_tb__DOT__rst_n) 
           && (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_valid));
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_fifo_wr = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_mode = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_start = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_rows = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_addr = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_start = 0U;
    if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid) {
        if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                      >> 5U)))) {
            if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                          >> 4U)))) {
                if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                              >> 3U)))) {
                    if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                                  >> 2U)))) {
                        if ((2U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode))) {
                            if ((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode)))) {
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_fifo_wr = 1U;
                            }
                            if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode))) {
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_mode = 1U;
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_start = 1U;
                            }
                        }
                        if ((1U & (~ ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode) 
                                      >> 1U)))) {
                            if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode))) {
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_rows 
                                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg3;
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_addr 
                                    = vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg1;
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_start = 1U;
                            }
                        }
                    }
                }
            }
        }
    }
    if (vlSelfRef.tpu_top_tb__DOT__rst_n) {
        if ((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_valid = 1U;
        }
    } else {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_valid = 0U;
    }
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state;
    if ((4U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state = 0U;
    } else if ((2U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))) {
        if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state = 4U;
        } else if (((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__compute_counter) 
                    >= ((IData)(1U) + (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__total_rows)))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state = 3U;
        }
    } else if ((1U & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))) {
        if ((3U <= (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__weight_load_counter))) {
            vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state = 2U;
        }
    } else if (vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_start) {
        vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state = 1U;
    }
    vlSelfRef.tpu_top_tb__DOT__tpu_busy = ((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state)) 
                                           | (0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state)));
    vlSelfRef.tpu_top_tb__DOT__pipeline_stage = ((IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)
                                                  ? 0U
                                                  : 
                                                 ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid)
                                                   ? 2U
                                                   : 1U));
}

void Vtpu_top_tb___024root___eval_nba(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_nba\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000020ULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vtpu_top_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((0x0000000000000060ULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vtpu_top_tb___024root___nba_sequent__TOP__1(vlSelf);
        vlSelfRef.__Vm_traceActivity[4U] = 1U;
    }
    if ((0x000000000000001cULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vtpu_top_tb___024root___nba_sequent__TOP__2(vlSelf);
    }
    if ((0x00000000000000a0ULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vtpu_top_tb___024root___nba_comb__TOP__0(vlSelf);
    }
    if ((0x0000000000000020ULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vtpu_top_tb___024root___nba_sequent__TOP__3(vlSelf);
    }
    if ((0x00000000000000e0ULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vtpu_top_tb___024root___nba_comb__TOP__1(vlSelf);
        vlSelfRef.__Vm_traceActivity[5U] = 1U;
    }
    if ((0x0000000000000060ULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vtpu_top_tb___024root___nba_sequent__TOP__4(vlSelf);
        vlSelfRef.__Vm_traceActivity[6U] = 1U;
    }
    if ((0x0000000000000061ULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vtpu_top_tb___024root___act_sequent__TOP__1(vlSelf);
    }
    if ((0x0000000000000062ULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vtpu_top_tb___024root___act_sequent__TOP__0(vlSelf);
    }
}

void Vtpu_top_tb___024root___timing_commit(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___timing_commit\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((! (0x0000000000000020ULL & vlSelfRef.__VactTriggered
            [0U]))) {
        vlSelfRef.__VtrigSched_h3d892c53__0.commit(
                                                   "@(posedge tpu_top_tb.clk)");
    }
}

void Vtpu_top_tb___024root___timing_resume(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___timing_resume\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered
         [0U])) {
        vlSelfRef.__VtrigSched_h3d892c53__0.resume(
                                                   "@(posedge tpu_top_tb.clk)");
    }
    if ((0x0000000000000080ULL & vlSelfRef.__VactTriggered
         [0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtpu_top_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtpu_top_tb___024root___eval_phase__act(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_phase__act\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtpu_top_tb___024root___eval_triggers__act(vlSelf);
    Vtpu_top_tb___024root___timing_commit(vlSelf);
    Vtpu_top_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtpu_top_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vtpu_top_tb___024root___timing_resume(vlSelf);
        Vtpu_top_tb___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

void Vtpu_top_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtpu_top_tb___024root___eval_phase__nba(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_phase__nba\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtpu_top_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtpu_top_tb___024root___eval_nba(vlSelf);
        Vtpu_top_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtpu_top_tb___024root___eval(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtpu_top_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("tpu_top_tb.sv", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vtpu_top_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("tpu_top_tb.sv", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vtpu_top_tb___024root___eval_phase__act(vlSelf));
    } while (Vtpu_top_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vtpu_top_tb___024root___eval_debug_assertions(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_debug_assertions\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
