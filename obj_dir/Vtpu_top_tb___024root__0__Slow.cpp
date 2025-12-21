// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtpu_top_tb.h for the primary calling header

#include "Vtpu_top_tb__pch.h"

VL_ATTR_COLD void Vtpu_top_tb___024root___eval_static__TOP(Vtpu_top_tb___024root* vlSelf);
VL_ATTR_COLD void Vtpu_top_tb___024root____Vm_traceActivitySetAll(Vtpu_top_tb___024root* vlSelf);

VL_ATTR_COLD void Vtpu_top_tb___024root___eval_static(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_static\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtpu_top_tb___024root___eval_static__TOP(vlSelf);
    Vtpu_top_tb___024root____Vm_traceActivitySetAll(vlSelf);
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__wt_buf_sel__0 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel;
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__acc_buf_sel__0 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel;
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
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__rst_n__0 
        = vlSelfRef.tpu_top_tb__DOT__rst_n;
}

extern const VlWide<8>/*255:0*/ Vtpu_top_tb__ConstPool__CONST_h9e67c271_0;

VL_ATTR_COLD void Vtpu_top_tb___024root___eval_static__TOP(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_static__TOP\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.tpu_top_tb__DOT__clk = 0U;
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
    vlSelfRef.tpu_top_tb__DOT__weights_layer1[0U] = 0x0100U;
    vlSelfRef.tpu_top_tb__DOT__weights_layer1[1U] = 0x0200U;
    vlSelfRef.tpu_top_tb__DOT__weights_layer1[2U] = 0x0300U;
    vlSelfRef.tpu_top_tb__DOT__weights_layer1[3U] = 0x0400U;
    vlSelfRef.tpu_top_tb__DOT__weights_layer2[0U] = 0x0100U;
    vlSelfRef.tpu_top_tb__DOT__weights_layer2[1U] = 0x0100U;
    vlSelfRef.tpu_top_tb__DOT__weights_layer2[2U] = 0x0100U;
    vlSelfRef.tpu_top_tb__DOT__weights_layer2[3U] = 0x0100U;
    vlSelfRef.tpu_top_tb__DOT__activations[0U] = 0x0500U;
    vlSelfRef.tpu_top_tb__DOT__activations[1U] = 0x0600U;
    vlSelfRef.tpu_top_tb__DOT__activations[2U] = 0x0700U;
    vlSelfRef.tpu_top_tb__DOT__activations[3U] = 0x0800U;
}

VL_ATTR_COLD void Vtpu_top_tb___024root___eval_initial__TOP(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_initial__TOP\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in[0U][0U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in[0U][1U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in[0U][2U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0U] = 0x08000000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[1U] = 0x08040000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[2U] = 0x08080000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[3U] = 0x080c0000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[4U] = 0x08100000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[5U] = 0x08140000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[6U] = 0x08180000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[7U] = 0x081c0000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[8U] = 0x08200000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[9U] = 0x0400000cU;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x0aU] = 0x0c000000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x0bU] = 0x10000000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x0cU] = 0x08240000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x0dU] = 0x08280000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x0eU] = 0x082c0000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x0fU] = 0x08300000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x10U] = 0x08340000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x11U] = 0x08380000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x12U] = 0x083c0000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x13U] = 0x08400000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x14U] = 0x08440000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x15U] = 0x0400000cU;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x16U] = 0x0c000000U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x0dU] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x0eU] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x0fU] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x10U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x11U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x12U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x13U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x14U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x15U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x16U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x17U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x18U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x19U] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x1aU] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x1bU] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x1cU] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x1dU] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x1eU] = 0U;
    vlSelfRef.tpu_top_tb__DOT__instr_memory[0x1fU] = 0U;
    vlSelfRef.tpu_top_tb__DOT__unnamedblk1__DOT__i = 0x00000020U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_start = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_rows = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_addr = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_fifo_wr = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_start = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_mode = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_start = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_dir = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_ub_addr = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_length = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_elem_sz = 0U;
}

VL_ATTR_COLD void Vtpu_top_tb___024root___eval_final(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_final\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtpu_top_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 2> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtpu_top_tb___024root___eval_phase__stl(Vtpu_top_tb___024root* vlSelf);

VL_ATTR_COLD void Vtpu_top_tb___024root___eval_settle(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_settle\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtpu_top_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("tpu_top_tb.sv", 3, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vtpu_top_tb___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vtpu_top_tb___024root___eval_triggers__stl(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_triggers__stl\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[1U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [1U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
    vlSelfRef.__VstlTriggered[0U] = (QData)((IData)(
                                                    ((((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel) 
                                                       != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__acc_buf_sel__0)) 
                                                      << 1U) 
                                                     | ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel) 
                                                        != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__wt_buf_sel__0)))));
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__wt_buf_sel__0 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel;
    vlSelfRef.__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__acc_buf_sel__0 
        = vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel;
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.__VstlDidInit)))))) {
        vlSelfRef.__VstlDidInit = 1U;
        vlSelfRef.__VstlTriggered[0U] = (1ULL | vlSelfRef.__VstlTriggered
                                         [0U]);
        vlSelfRef.__VstlTriggered[0U] = (2ULL | vlSelfRef.__VstlTriggered
                                         [0U]);
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtpu_top_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vtpu_top_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 2> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtpu_top_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 2> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtpu_top_tb___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @([hybrid] tpu_top_tb.dut.wt_buf_sel)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([hybrid] tpu_top_tb.dut.acc_buf_sel)\n");
    }
    if ((1U & (IData)(triggers[1U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 64 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtpu_top_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 2> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___trigger_anySet__stl\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((2U > n));
    return (0U);
}

extern const VlUnpacked<CData/*0:0*/, 256> Vtpu_top_tb__ConstPool__TABLE_h2e55c492_0;
extern const VlUnpacked<CData/*0:0*/, 256> Vtpu_top_tb__ConstPool__TABLE_had101f60_0;
extern const VlUnpacked<CData/*0:0*/, 256> Vtpu_top_tb__ConstPool__TABLE_h2ef52e1e_0;

VL_ATTR_COLD void Vtpu_top_tb___024root___stl_sequent__TOP__0(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___stl_sequent__TOP__0\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
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
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_fifo_wr = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_start = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_mode = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_rows = 0U;
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_addr = 0U;
    vlSelfRef.tpu_top_tb__DOT__tpu_busy = ((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state)) 
                                           | (0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state)));
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
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_start = 1U;
                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_mode = 1U;
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
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[0U][0U] 
        = (0x000000ffU & vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[0U]);
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
    vlSelfRef.tpu_top_tb__DOT__dut__DOT__use_uart_dma 
        = ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en) 
           | ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en) 
              | ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_en) 
                 | (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_en))));
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
    vlSelfRef.tpu_top_tb__DOT__pipeline_stage = ((IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)
                                                  ? 0U
                                                  : 
                                                 ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid)
                                                   ? 2U
                                                   : 1U));
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
}

void Vtpu_top_tb___024root___act_sequent__TOP__0(Vtpu_top_tb___024root* vlSelf);
void Vtpu_top_tb___024root___act_sequent__TOP__1(Vtpu_top_tb___024root* vlSelf);

VL_ATTR_COLD void Vtpu_top_tb___024root___eval_stl(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_stl\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[1U])) {
        Vtpu_top_tb___024root___stl_sequent__TOP__0(vlSelf);
        Vtpu_top_tb___024root____Vm_traceActivitySetAll(vlSelf);
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (2ULL 
                                                   & vlSelfRef.__VstlTriggered
                                                   [0U]))) {
        Vtpu_top_tb___024root___act_sequent__TOP__0(vlSelf);
    }
    if ((1ULL & (vlSelfRef.__VstlTriggered[1U] | vlSelfRef.__VstlTriggered
                 [0U]))) {
        Vtpu_top_tb___024root___act_sequent__TOP__1(vlSelf);
    }
}

VL_ATTR_COLD bool Vtpu_top_tb___024root___eval_phase__stl(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___eval_phase__stl\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtpu_top_tb___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vtpu_top_tb___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtpu_top_tb___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtpu_top_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtpu_top_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtpu_top_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @([hybrid] tpu_top_tb.dut.wt_buf_sel)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([hybrid] tpu_top_tb.dut.acc_buf_sel)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @( tpu_top_tb.instr_addr_out)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @( tpu_top_tb.pipeline_stage)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @( tpu_top_tb.tpu_busy)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @(posedge tpu_top_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 6U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 6 is active: @(negedge tpu_top_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 7U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 7 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtpu_top_tb___024root____Vm_traceActivitySetAll(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root____Vm_traceActivitySetAll\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vm_traceActivity[0U] = 1U;
    vlSelfRef.__Vm_traceActivity[1U] = 1U;
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.__Vm_traceActivity[3U] = 1U;
    vlSelfRef.__Vm_traceActivity[4U] = 1U;
    vlSelfRef.__Vm_traceActivity[5U] = 1U;
    vlSelfRef.__Vm_traceActivity[6U] = 1U;
}

VL_ATTR_COLD void Vtpu_top_tb___024root___ctor_var_reset(Vtpu_top_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root___ctor_var_reset\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->tpu_top_tb__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10321511614197254095ull);
    vlSelf->tpu_top_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15201226599777105698ull);
    vlSelf->tpu_top_tb__DOT__instr_addr_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5622369489244515313ull);
    vlSelf->tpu_top_tb__DOT__dma_start_in = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12851747182689783826ull);
    vlSelf->tpu_top_tb__DOT__dma_dir_in = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12285788403672379413ull);
    vlSelf->tpu_top_tb__DOT__dma_ub_addr_in = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9256788101346440759ull);
    vlSelf->tpu_top_tb__DOT__dma_length_in = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17254368260649198308ull);
    vlSelf->tpu_top_tb__DOT__dma_elem_sz_in = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 1775457322716453083ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->tpu_top_tb__DOT__dma_data_in, __VscopeHash, 15871376375885338315ull);
    vlSelf->tpu_top_tb__DOT__tpu_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3556007938142019088ull);
    vlSelf->tpu_top_tb__DOT__pipeline_stage = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 4315772103564334410ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->tpu_top_tb__DOT__instr_memory[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 9501793911094618783ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->tpu_top_tb__DOT__weights_layer1[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4014617207824399919ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->tpu_top_tb__DOT__weights_layer2[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16435366931547914966ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->tpu_top_tb__DOT__activations[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7609373116769050208ull);
    }
    vlSelf->tpu_top_tb__DOT__unnamedblk1__DOT__i = 0;
    vlSelf->tpu_top_tb__DOT__unnamedblk2__DOT__i = 0;
    vlSelf->tpu_top_tb__DOT__unnamedblk3__DOT__i = 0;
    vlSelf->tpu_top_tb__DOT__unnamedblk4__DOT__cycle_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 9179801409858005629ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_tx = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6455576403123825456ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_debug_state = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8835303156060098562ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_debug_cmd = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17706631893409010788ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_debug_byte_count = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9141759795301164489ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__sys_start = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6944760400258683558ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__sys_rows = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17833440704496875346ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__ub_rd_addr = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 283230073227714579ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__wt_fifo_wr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10357943914589865891ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__vpu_start = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15839969394865971934ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__vpu_mode = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13452731196175140710ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__wt_buf_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8010181403765388159ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__acc_buf_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2339737027653279608ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->tpu_top_tb__DOT__dut__DOT__ub_rd_data, __VscopeHash, 16813128780789504266ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18059082706312558052ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_ub_wr_addr = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4723188403963184002ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data, __VscopeHash, 10754187737948025797ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2532985214856331230ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_ub_rd_addr = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2955889641005457867ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_wt_wr_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4148885919224012737ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_wt_wr_addr = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 15285952426610305594ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_wt_wr_data = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 1906928304154349242ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_instr_wr_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14988662728224743296ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_instr_wr_addr = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2963993340408122986ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_instr_wr_data = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7446660971816038764ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_start_execution = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8721828039516359766ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__dma_start = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7336026924183171183ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__dma_dir = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8775015472793006509ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__dma_ub_addr = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14807151576564482476ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__dma_length = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1313485345429804232ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__dma_elem_sz = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 5944794444505579492ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__use_uart_dma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5758867546393386905ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18406883040210007037ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5067333518769501633ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10800589755446430672ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_pc = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7545431011331697194ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_opcode = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 3287243620756940781ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg1 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5636246739390025789ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15848401017483547643ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg3 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17567050618439504151ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_flags = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 7678617167377165207ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_unit_sel = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16248221878563624544ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8942435007154883470ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 5880169053050939120ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg1 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17350395553278954934ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4334537201772687895ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg3 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6684840271452154748ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_flags = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 9404954532567599209ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2229095108116654570ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5726785576597082587ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9819811250718172057ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15982457230188643383ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6628921513098639299ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7478628954447375520ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9687203152027771575ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 333458343673548516ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14037413764100585195ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14590240597408766658ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2287586560630507548ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer, __VscopeHash, 9662558623606432979ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 18088906998878236621ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 745259948707559148ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer, __VscopeHash, 16185587482736473650ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2556446068182984727ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 525861111515067307ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10874676624691358832ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 295471079172696447ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_byte = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6162879677791578772ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6933598743092517403ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2167619392564434887ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 7404624051240562186ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6446344735359437845ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 15248468017791760270ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__tx_byte = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16731308547359079432ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 12913486350758791318ull);
    VL_SCOPED_RAND_RESET_W(96, vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out, __VscopeHash, 3548877860320339617ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13324809980326574950ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7259674952901576161ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14800138728427128381ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data, __VscopeHash, 398843915060373275ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18354554523917926597ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_a_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6072521868589374693ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_b_ready = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13729210941622434386ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 1889298575101185241ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 3793764253596073064ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__weight_load_counter = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10085636303330905372ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__compute_counter = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 840950722985485133ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__total_rows = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3338926902120407568ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5017673785648647069ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10600046526663052470ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_empty = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8029390796215049153ull);
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 17940960494435585374ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 421954265784537249ull);
    }
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_ptr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 11291411209453592109ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 7374857626705780952ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 9274976447454161292ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_count = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 8138664409681390971ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_ptr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 4673556424292211367ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 2453619338902529183ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 780036996131549232ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_count = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 11353152750506433015ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_wr_data = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 1496416184154850010ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3466524079683625132ull);
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        for (int __Vi1 = 0; __Vi1 < 3; ++__Vi1) {
            vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in[__Vi0][__Vi1] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17340868791072777477ull);
        }
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        for (int __Vi1 = 0; __Vi1 < 3; ++__Vi1) {
            vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out[__Vi0][__Vi1] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1386596438306889985ull);
        }
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        for (int __Vi1 = 0; __Vi1 < 3; ++__Vi1) {
            vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in[__Vi0][__Vi1] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 10621184148233780320ull);
        }
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        for (int __Vi1 = 0; __Vi1 < 3; ++__Vi1) {
            vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out[__Vi0][__Vi1] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14607833290403500629ull);
        }
    }
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row1_delayed = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14922820525341091798ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row2_delayed = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7804293997349436157ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_00__act_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4337581275532800426ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_01__act_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3699681438563650114ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_10__act_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15294835778285413924ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_11__act_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4012750940740160764ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_02__act_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9771424088182624317ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_12__act_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16168454451805273775ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_20__act_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14380304002536307493ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_21__act_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1170517702605110157ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_22__act_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10361850443752289538ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__weight_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8618265535911907027ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mult_result = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14613375596730495186ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 13621052477022379364ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__weight_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17239155898405011242ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mult_result = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 296027111911721237ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 3706291188623819397ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__weight_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10928089622943555875ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mult_result = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 702425163691325626ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 15544408443083276083ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__weight_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10597733977456741100ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mult_result = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16622782766534423852ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4595417888166410425ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__weight_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1326399942112672229ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mult_result = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6841899813895433810ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 313437194954071777ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__weight_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2049654517916816681ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mult_result = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8984617518785479798ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7553188230954088142ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__weight_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5059812273922981194ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mult_result = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14896626727886354029ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4291494169314414233ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__weight_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7683771746830657410ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mult_result = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10518321357038685624ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4094928094471158049ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__weight_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11264017929868367362ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mult_result = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5532530769060693758ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 13867066858067690683ull);
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        VL_SCOPED_RAND_RESET_W(96, vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0[__Vi0], __VscopeHash, 16934861557518647762ull);
    }
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        VL_SCOPED_RAND_RESET_W(96, vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1[__Vi0], __VscopeHash, 3089837425735493809ull);
    }
    VL_SCOPED_RAND_RESET_W(96, vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data, __VscopeHash, 14267381493291151256ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 10134812581777871609ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2278823683543334181ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 13031667313431297600ull);
    }
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result0 = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 3089601140242411538ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result1 = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5565450675447167806ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data, __VscopeHash, 15828663915699734949ull);
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        VL_SCOPED_RAND_RESET_W(256, vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__memory[__Vi0], __VscopeHash, 17377037819775261768ull);
    }
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 12493283023423951078ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14663921296589353727ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5302821867179824303ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 372181559247108159ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1205071729675584447ull);
    vlSelf->tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1474850449812569069ull);
    vlSelf->__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 17729191774060958010ull);
    vlSelf->__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 4786403835642071648ull);
    vlSelf->__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer0__v0 = 0;
    vlSelf->__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3857563424363193411ull);
    vlSelf->__VdlyDim0__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 17758628945574948577ull);
    vlSelf->__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buffer1__v0 = 0;
    VL_SCOPED_RAND_RESET_W(96, vlSelf->__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0, __VscopeHash, 5583166807523401389ull);
    vlSelf->__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer1__v0 = 0;
    VL_SCOPED_RAND_RESET_W(96, vlSelf->__VdlyVal__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0, __VscopeHash, 1133967399199743638ull);
    vlSelf->__VdlySet__tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__buffer0__v0 = 0;
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__wt_buf_sel__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8392479988878895930ull);
    vlSelf->__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__acc_buf_sel__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 646502128204229405ull);
    vlSelf->__VstlDidInit = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__wt_buf_sel__1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3028798019583047799ull);
    vlSelf->__Vtrigprevexpr___TOP__tpu_top_tb__DOT__dut__DOT__acc_buf_sel__1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8887098729232848483ull);
    vlSelf->__Vtrigprevexpr___TOP__tpu_top_tb__DOT__instr_addr_out__0 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18246205291293054497ull);
    vlSelf->__Vtrigprevexpr___TOP__tpu_top_tb__DOT__pipeline_stage__0 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 5370765850127360410ull);
    vlSelf->__Vtrigprevexpr___TOP__tpu_top_tb__DOT__tpu_busy__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4544268343907123245ull);
    vlSelf->__Vtrigprevexpr___TOP__tpu_top_tb__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 788972744497195621ull);
    vlSelf->__Vtrigprevexpr___TOP__tpu_top_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16405256816619277969ull);
    vlSelf->__VactDidInit = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 7; ++__Vi0) {
        vlSelf->__Vm_traceActivity[__Vi0] = 0;
    }
}
