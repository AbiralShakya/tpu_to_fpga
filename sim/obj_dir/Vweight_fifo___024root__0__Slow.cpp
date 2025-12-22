// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vweight_fifo.h for the primary calling header

#include "Vweight_fifo__pch.h"

VL_ATTR_COLD void Vweight_fifo___024root___eval_static(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_static\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rst_n__0 = vlSelfRef.rst_n;
}

VL_ATTR_COLD void Vweight_fifo___024root___eval_initial(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_initial\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vweight_fifo___024root___eval_final(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_final\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vweight_fifo___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vweight_fifo___024root___eval_phase__stl(Vweight_fifo___024root* vlSelf);

VL_ATTR_COLD void Vweight_fifo___024root___eval_settle(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_settle\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vweight_fifo___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("../rtl/../rtl/weight_fifo.sv", 3, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vweight_fifo___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vweight_fifo___024root___eval_triggers__stl(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_triggers__stl\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vweight_fifo___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vweight_fifo___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vweight_fifo___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vweight_fifo___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vweight_fifo___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vweight_fifo___024root___stl_sequent__TOP__0(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___stl_sequent__TOP__0\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.rd_data = vlSelfRef.weight_fifo__DOT__selected_rd_data;
    if (vlSelfRef.wt_buf_sel) {
        vlSelfRef.wr_full = (0x1000U == (IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_count));
        vlSelfRef.wr_almost_full = (0x0ffcU <= (IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_count));
        vlSelfRef.rd_empty = ((IData)(vlSelfRef.weight_fifo__DOT__buf1_rd_count) 
                              == (IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_count));
        vlSelfRef.rd_almost_empty = (4U >= ((IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_count) 
                                            - (IData)(vlSelfRef.weight_fifo__DOT__buf1_rd_count)));
    } else {
        vlSelfRef.wr_full = (0x1000U == (IData)(vlSelfRef.weight_fifo__DOT__buf0_wr_count));
        vlSelfRef.wr_almost_full = (0x0ffcU <= (IData)(vlSelfRef.weight_fifo__DOT__buf0_wr_count));
        vlSelfRef.rd_empty = ((IData)(vlSelfRef.weight_fifo__DOT__buf0_rd_count) 
                              == (IData)(vlSelfRef.weight_fifo__DOT__buf0_wr_count));
        vlSelfRef.rd_almost_empty = (4U >= ((IData)(vlSelfRef.weight_fifo__DOT__buf0_wr_count) 
                                            - (IData)(vlSelfRef.weight_fifo__DOT__buf0_rd_count)));
    }
    vlSelfRef.wt_load_done = (((IData)(vlSelfRef.weight_fifo__DOT__buf0_wr_count) 
                               == (IData)(vlSelfRef.wt_num_tiles)) 
                              | ((IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_count) 
                                 == (IData)(vlSelfRef.wt_num_tiles)));
}

VL_ATTR_COLD void Vweight_fifo___024root___eval_stl(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_stl\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vweight_fifo___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vweight_fifo___024root___eval_phase__stl(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_phase__stl\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vweight_fifo___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vweight_fifo___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vweight_fifo___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vweight_fifo___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vweight_fifo___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(Vweight_fifo___024root___trigger_anySet__ico(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

bool Vweight_fifo___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vweight_fifo___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vweight_fifo___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge rst_n)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vweight_fifo___024root___ctor_var_reset(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___ctor_var_reset\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1638864771569018232ull);
    vlSelf->wt_buf_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3417007261464316093ull);
    vlSelf->wt_num_tiles = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4013186271224537459ull);
    vlSelf->wr_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7710928637576349896ull);
    vlSelf->wr_data = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 12812822527505751231ull);
    vlSelf->wr_full = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4538020038871708192ull);
    vlSelf->wr_almost_full = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7067467063107258612ull);
    vlSelf->rd_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3814484142505630662ull);
    vlSelf->rd_data = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 17824471296722538975ull);
    vlSelf->rd_empty = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13234839125597451765ull);
    vlSelf->rd_almost_empty = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16894303993188838797ull);
    vlSelf->wt_load_done = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14736486013023405747ull);
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->weight_fifo__DOT__buffer0[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 7492373306808633587ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->weight_fifo__DOT__buffer1[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2359576490050972605ull);
    }
    vlSelf->weight_fifo__DOT__buf0_wr_ptr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 3308068263930377547ull);
    vlSelf->weight_fifo__DOT__buf0_rd_ptr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 2496067828070595399ull);
    vlSelf->weight_fifo__DOT__buf0_wr_count = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 1199597196404585119ull);
    vlSelf->weight_fifo__DOT__buf0_rd_count = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 14557022949480201603ull);
    vlSelf->weight_fifo__DOT__buf1_wr_ptr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 3507823331704400195ull);
    vlSelf->weight_fifo__DOT__buf1_rd_ptr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6636261414727983640ull);
    vlSelf->weight_fifo__DOT__buf1_wr_count = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 14338997712155262293ull);
    vlSelf->weight_fifo__DOT__buf1_rd_count = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 580560580116205450ull);
    vlSelf->weight_fifo__DOT__selected_rd_data = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3001266304179353439ull);
    vlSelf->__Vdly__weight_fifo__DOT__buf0_wr_ptr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14115987411179030024ull);
    vlSelf->__Vdly__weight_fifo__DOT__buf1_wr_ptr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 12111535789570503114ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VicoTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9526919608049418986ull);
    vlSelf->__Vtrigprevexpr___TOP__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14803524876191471008ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
