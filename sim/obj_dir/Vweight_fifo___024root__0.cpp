// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vweight_fifo.h for the primary calling header

#include "Vweight_fifo__pch.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vweight_fifo___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vweight_fifo___024root___eval_triggers__ico(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_triggers__ico\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VicoTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VicoFirstIteration)));
    vlSelfRef.__VicoFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vweight_fifo___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
    }
#endif
}

bool Vweight_fifo___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___trigger_anySet__ico\n"); );
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

void Vweight_fifo___024root___ico_sequent__TOP__0(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___ico_sequent__TOP__0\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
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

void Vweight_fifo___024root___eval_ico(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_ico\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered[0U])) {
        Vweight_fifo___024root___ico_sequent__TOP__0(vlSelf);
    }
}

bool Vweight_fifo___024root___eval_phase__ico(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_phase__ico\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VicoExecute;
    // Body
    Vweight_fifo___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = Vweight_fifo___024root___trigger_anySet__ico(vlSelfRef.__VicoTriggered);
    if (__VicoExecute) {
        Vweight_fifo___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vweight_fifo___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vweight_fifo___024root___eval_triggers__act(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_triggers__act\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((((~ (IData)(vlSelfRef.rst_n)) 
                                                       & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__rst_n__0)) 
                                                      << 1U) 
                                                     | ((IData)(vlSelfRef.clk) 
                                                        & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__0))))));
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rst_n__0 = vlSelfRef.rst_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vweight_fifo___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vweight_fifo___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___trigger_anySet__act\n"); );
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

void Vweight_fifo___024root___nba_sequent__TOP__0(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___nba_sequent__TOP__0\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    SData/*11:0*/ __Vdly__weight_fifo__DOT__buf0_rd_ptr;
    __Vdly__weight_fifo__DOT__buf0_rd_ptr = 0;
    SData/*11:0*/ __Vdly__weight_fifo__DOT__buf1_rd_ptr;
    __Vdly__weight_fifo__DOT__buf1_rd_ptr = 0;
    // Body
    __Vdly__weight_fifo__DOT__buf0_rd_ptr = vlSelfRef.weight_fifo__DOT__buf0_rd_ptr;
    __Vdly__weight_fifo__DOT__buf1_rd_ptr = vlSelfRef.weight_fifo__DOT__buf1_rd_ptr;
    vlSelfRef.__Vdly__weight_fifo__DOT__buf0_wr_ptr 
        = vlSelfRef.weight_fifo__DOT__buf0_wr_ptr;
    vlSelfRef.__Vdly__weight_fifo__DOT__buf1_wr_ptr 
        = vlSelfRef.weight_fifo__DOT__buf1_wr_ptr;
    if (vlSelfRef.rst_n) {
        if ((((IData)(vlSelfRef.rd_en) & (~ (IData)(vlSelfRef.wt_buf_sel))) 
             & (~ (IData)(vlSelfRef.rd_empty)))) {
            __Vdly__weight_fifo__DOT__buf0_rd_ptr = 
                (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.weight_fifo__DOT__buf0_rd_ptr)));
            vlSelfRef.weight_fifo__DOT__buf0_rd_count 
                = (0x00001fffU & ((IData)(1U) + (IData)(vlSelfRef.weight_fifo__DOT__buf0_rd_count)));
        }
        if ((((IData)(vlSelfRef.rd_en) & (IData)(vlSelfRef.wt_buf_sel)) 
             & (~ (IData)(vlSelfRef.rd_empty)))) {
            __Vdly__weight_fifo__DOT__buf1_rd_ptr = 
                (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.weight_fifo__DOT__buf1_rd_ptr)));
            vlSelfRef.weight_fifo__DOT__buf1_rd_count 
                = (0x00001fffU & ((IData)(1U) + (IData)(vlSelfRef.weight_fifo__DOT__buf1_rd_count)));
        }
        if ((((IData)(vlSelfRef.wr_en) & (~ (IData)(vlSelfRef.wt_buf_sel))) 
             & (~ (IData)(vlSelfRef.wr_full)))) {
            vlSelfRef.__Vdly__weight_fifo__DOT__buf0_wr_ptr 
                = (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.weight_fifo__DOT__buf0_wr_ptr)));
            vlSelfRef.weight_fifo__DOT__buf0_wr_count 
                = (0x00001fffU & ((IData)(1U) + (IData)(vlSelfRef.weight_fifo__DOT__buf0_wr_count)));
        }
        if ((((IData)(vlSelfRef.wr_en) & (IData)(vlSelfRef.wt_buf_sel)) 
             & (~ (IData)(vlSelfRef.wr_full)))) {
            vlSelfRef.__Vdly__weight_fifo__DOT__buf1_wr_ptr 
                = (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_ptr)));
            vlSelfRef.weight_fifo__DOT__buf1_wr_count 
                = (0x00001fffU & ((IData)(1U) + (IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_count)));
        }
        vlSelfRef.weight_fifo__DOT__selected_rd_data 
            = ((IData)(vlSelfRef.wt_buf_sel) ? vlSelfRef.weight_fifo__DOT__buffer1
               [vlSelfRef.weight_fifo__DOT__buf1_rd_ptr]
                : vlSelfRef.weight_fifo__DOT__buffer0
               [vlSelfRef.weight_fifo__DOT__buf0_rd_ptr]);
    } else {
        __Vdly__weight_fifo__DOT__buf0_rd_ptr = 0U;
        __Vdly__weight_fifo__DOT__buf1_rd_ptr = 0U;
        vlSelfRef.__Vdly__weight_fifo__DOT__buf0_wr_ptr = 0U;
        vlSelfRef.__Vdly__weight_fifo__DOT__buf1_wr_ptr = 0U;
        vlSelfRef.weight_fifo__DOT__buf0_rd_count = 0U;
        vlSelfRef.weight_fifo__DOT__buf1_rd_count = 0U;
        vlSelfRef.weight_fifo__DOT__buf0_wr_count = 0U;
        vlSelfRef.weight_fifo__DOT__buf1_wr_count = 0U;
        vlSelfRef.weight_fifo__DOT__selected_rd_data = 0U;
    }
    vlSelfRef.weight_fifo__DOT__buf1_rd_ptr = __Vdly__weight_fifo__DOT__buf1_rd_ptr;
    vlSelfRef.weight_fifo__DOT__buf0_rd_ptr = __Vdly__weight_fifo__DOT__buf0_rd_ptr;
    if (vlSelfRef.wt_buf_sel) {
        vlSelfRef.wr_almost_full = (0x0ffcU <= (IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_count));
        vlSelfRef.rd_empty = ((IData)(vlSelfRef.weight_fifo__DOT__buf1_rd_count) 
                              == (IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_count));
        vlSelfRef.rd_almost_empty = (4U >= ((IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_count) 
                                            - (IData)(vlSelfRef.weight_fifo__DOT__buf1_rd_count)));
    } else {
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
    vlSelfRef.rd_data = vlSelfRef.weight_fifo__DOT__selected_rd_data;
}

void Vweight_fifo___024root___nba_sequent__TOP__1(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___nba_sequent__TOP__1\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*23:0*/ __VdlyVal__weight_fifo__DOT__buffer0__v0;
    __VdlyVal__weight_fifo__DOT__buffer0__v0 = 0;
    SData/*11:0*/ __VdlyDim0__weight_fifo__DOT__buffer0__v0;
    __VdlyDim0__weight_fifo__DOT__buffer0__v0 = 0;
    CData/*0:0*/ __VdlySet__weight_fifo__DOT__buffer0__v0;
    __VdlySet__weight_fifo__DOT__buffer0__v0 = 0;
    IData/*23:0*/ __VdlyVal__weight_fifo__DOT__buffer1__v0;
    __VdlyVal__weight_fifo__DOT__buffer1__v0 = 0;
    SData/*11:0*/ __VdlyDim0__weight_fifo__DOT__buffer1__v0;
    __VdlyDim0__weight_fifo__DOT__buffer1__v0 = 0;
    CData/*0:0*/ __VdlySet__weight_fifo__DOT__buffer1__v0;
    __VdlySet__weight_fifo__DOT__buffer1__v0 = 0;
    // Body
    __VdlySet__weight_fifo__DOT__buffer0__v0 = 0U;
    __VdlySet__weight_fifo__DOT__buffer1__v0 = 0U;
    if (vlSelfRef.wr_en) {
        if ((1U & ((~ (IData)(vlSelfRef.wt_buf_sel)) 
                   & (~ (IData)(vlSelfRef.wr_full))))) {
            __VdlyVal__weight_fifo__DOT__buffer0__v0 
                = vlSelfRef.wr_data;
            __VdlyDim0__weight_fifo__DOT__buffer0__v0 
                = vlSelfRef.weight_fifo__DOT__buf0_wr_ptr;
            __VdlySet__weight_fifo__DOT__buffer0__v0 = 1U;
        }
        if ((1U & (~ ((~ (IData)(vlSelfRef.wt_buf_sel)) 
                      & (~ (IData)(vlSelfRef.wr_full)))))) {
            if (((IData)(vlSelfRef.wt_buf_sel) & (~ (IData)(vlSelfRef.wr_full)))) {
                __VdlyVal__weight_fifo__DOT__buffer1__v0 
                    = vlSelfRef.wr_data;
                __VdlyDim0__weight_fifo__DOT__buffer1__v0 
                    = vlSelfRef.weight_fifo__DOT__buf1_wr_ptr;
                __VdlySet__weight_fifo__DOT__buffer1__v0 = 1U;
            }
        }
    }
    if (__VdlySet__weight_fifo__DOT__buffer0__v0) {
        vlSelfRef.weight_fifo__DOT__buffer0[__VdlyDim0__weight_fifo__DOT__buffer0__v0] 
            = __VdlyVal__weight_fifo__DOT__buffer0__v0;
    }
    if (__VdlySet__weight_fifo__DOT__buffer1__v0) {
        vlSelfRef.weight_fifo__DOT__buffer1[__VdlyDim0__weight_fifo__DOT__buffer1__v0] 
            = __VdlyVal__weight_fifo__DOT__buffer1__v0;
    }
}

void Vweight_fifo___024root___nba_sequent__TOP__2(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___nba_sequent__TOP__2\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.weight_fifo__DOT__buf0_wr_ptr = vlSelfRef.__Vdly__weight_fifo__DOT__buf0_wr_ptr;
    vlSelfRef.weight_fifo__DOT__buf1_wr_ptr = vlSelfRef.__Vdly__weight_fifo__DOT__buf1_wr_ptr;
    vlSelfRef.wr_full = ((IData)(vlSelfRef.wt_buf_sel)
                          ? (0x1000U == (IData)(vlSelfRef.weight_fifo__DOT__buf1_wr_count))
                          : (0x1000U == (IData)(vlSelfRef.weight_fifo__DOT__buf0_wr_count)));
}

void Vweight_fifo___024root___eval_nba(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_nba\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vweight_fifo___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vweight_fifo___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vweight_fifo___024root___nba_sequent__TOP__2(vlSelf);
    }
}

void Vweight_fifo___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vweight_fifo___024root___eval_phase__act(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_phase__act\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vweight_fifo___024root___eval_triggers__act(vlSelf);
    Vweight_fifo___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    return (0U);
}

void Vweight_fifo___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vweight_fifo___024root___eval_phase__nba(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_phase__nba\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vweight_fifo___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vweight_fifo___024root___eval_nba(vlSelf);
        Vweight_fifo___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vweight_fifo___024root___eval(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VicoIterCount;
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VicoIterCount = 0U;
    vlSelfRef.__VicoFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VicoIterCount)))) {
#ifdef VL_DEBUG
            Vweight_fifo___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
            VL_FATAL_MT("../rtl/../rtl/weight_fifo.sv", 3, "", "Input combinational region did not converge after 100 tries");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
    } while (Vweight_fifo___024root___eval_phase__ico(vlSelf));
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vweight_fifo___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("../rtl/../rtl/weight_fifo.sv", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vweight_fifo___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("../rtl/../rtl/weight_fifo.sv", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vweight_fifo___024root___eval_phase__act(vlSelf));
    } while (Vweight_fifo___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vweight_fifo___024root___eval_debug_assertions(Vweight_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root___eval_debug_assertions\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");
    }
    if (VL_UNLIKELY(((vlSelfRef.rst_n & 0xfeU)))) {
        Verilated::overWidthError("rst_n");
    }
    if (VL_UNLIKELY(((vlSelfRef.wt_buf_sel & 0xfeU)))) {
        Verilated::overWidthError("wt_buf_sel");
    }
    if (VL_UNLIKELY(((vlSelfRef.wr_en & 0xfeU)))) {
        Verilated::overWidthError("wr_en");
    }
    if (VL_UNLIKELY(((vlSelfRef.wr_data & 0xff000000U)))) {
        Verilated::overWidthError("wr_data");
    }
    if (VL_UNLIKELY(((vlSelfRef.rd_en & 0xfeU)))) {
        Verilated::overWidthError("rd_en");
    }
}
#endif  // VL_DEBUG
