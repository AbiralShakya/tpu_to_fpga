// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vsimple_test.h for the primary calling header

#include "Vsimple_test__pch.h"

VlCoroutine Vsimple_test___024root___eval_initial__TOP__Vtiming__0(Vsimple_test___024root* vlSelf);
VlCoroutine Vsimple_test___024root___eval_initial__TOP__Vtiming__1(Vsimple_test___024root* vlSelf);

void Vsimple_test___024root___eval_initial(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_initial\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vsimple_test___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vsimple_test___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vsimple_test___024root___eval_initial__TOP__Vtiming__0(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    VL_WRITEF_NX("Starting simple TPU PE test...\n",0);
    vlSymsp->_vm_contextp__->dumpfile("simple_test.vcd"s);
    VL_PRINTF_MT("-Info: simple_test.v:43: $dumpvar ignored, as Verilated without --trace\n");
    vlSelfRef.simple_test__DOT__rst_n = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "simple_test.v", 
                                         47);
    vlSelfRef.simple_test__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "simple_test.v", 
                                         49);
    vlSelfRef.simple_test__DOT__weight_in = 5U;
    vlSelfRef.simple_test__DOT__en_weight_capture = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "simple_test.v", 
                                         54);
    vlSelfRef.simple_test__DOT__en_weight_capture = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "simple_test.v", 
                                         56);
    vlSelfRef.simple_test__DOT__act_in = 0x0aU;
    vlSelfRef.simple_test__DOT__psum_in = 0U;
    vlSelfRef.simple_test__DOT__en_weight_pass = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "simple_test.v", 
                                         62);
    VL_WRITEF_NX("Input: act=%3#, weight=%3#, psum_in=%10#\nOutput: act_out=%3#, psum_out=%10#\nExpected: 10 * 5 + 0 = 50\n",0,
                 8,vlSelfRef.simple_test__DOT__act_in,
                 8,(IData)(vlSelfRef.simple_test__DOT__weight_in),
                 32,vlSelfRef.simple_test__DOT__psum_in,
                 8,(IData)(vlSelfRef.simple_test__DOT__act_out),
                 32,vlSelfRef.simple_test__DOT__psum_out);
    vlSelfRef.simple_test__DOT__act_in = 3U;
    vlSelfRef.simple_test__DOT__psum_in = 0x00000020U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "simple_test.v", 
                                         71);
    VL_WRITEF_NX("Input: act=%3#, weight=%3#, psum_in=%10#\nOutput: act_out=%3#, psum_out=%10#\nExpected: 3 * 5 + 32 = 47\n",0,
                 8,vlSelfRef.simple_test__DOT__act_in,
                 8,(IData)(vlSelfRef.simple_test__DOT__weight_in),
                 32,vlSelfRef.simple_test__DOT__psum_in,
                 8,(IData)(vlSelfRef.simple_test__DOT__act_out),
                 32,vlSelfRef.simple_test__DOT__psum_out);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "simple_test.v", 
                                         77);
    VL_FINISH_MT("simple_test.v", 78, "");
}

VlCoroutine Vsimple_test___024root___eval_initial__TOP__Vtiming__1(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "simple_test.v", 
                                             14);
        vlSelfRef.simple_test__DOT__clk = (1U & (~ (IData)(vlSelfRef.simple_test__DOT__clk)));
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsimple_test___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vsimple_test___024root___eval_triggers__act(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_triggers__act\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 2U) 
                                                     | ((((~ (IData)(vlSelfRef.simple_test__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__simple_test__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.simple_test__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__simple_test__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__simple_test__DOT__clk__0 
        = vlSelfRef.simple_test__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__simple_test__DOT__rst_n__0 
        = vlSelfRef.simple_test__DOT__rst_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vsimple_test___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vsimple_test___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___trigger_anySet__act\n"); );
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

void Vsimple_test___024root___nba_sequent__TOP__0(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___nba_sequent__TOP__0\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.simple_test__DOT__rst_n) {
        if (vlSelfRef.simple_test__DOT__en_weight_pass) {
            vlSelfRef.simple_test__DOT__act_out = vlSelfRef.simple_test__DOT__act_in;
            vlSelfRef.simple_test__DOT__psum_out = 
                (vlSelfRef.simple_test__DOT__psum_in 
                 + ((IData)(vlSelfRef.simple_test__DOT__act_in) 
                    * (IData)(vlSelfRef.simple_test__DOT__pe_inst__DOT__weight_reg)));
        }
        if (vlSelfRef.simple_test__DOT__en_weight_capture) {
            vlSelfRef.simple_test__DOT__pe_inst__DOT__weight_reg 
                = vlSelfRef.simple_test__DOT__weight_in;
        }
    } else {
        vlSelfRef.simple_test__DOT__act_out = 0U;
        vlSelfRef.simple_test__DOT__psum_out = 0U;
        vlSelfRef.simple_test__DOT__pe_inst__DOT__weight_reg = 0U;
    }
}

void Vsimple_test___024root___eval_nba(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_nba\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vsimple_test___024root___nba_sequent__TOP__0(vlSelf);
    }
}

void Vsimple_test___024root___timing_resume(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___timing_resume\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vsimple_test___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vsimple_test___024root___eval_phase__act(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_phase__act\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vsimple_test___024root___eval_triggers__act(vlSelf);
    Vsimple_test___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vsimple_test___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vsimple_test___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

void Vsimple_test___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vsimple_test___024root___eval_phase__nba(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_phase__nba\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vsimple_test___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vsimple_test___024root___eval_nba(vlSelf);
        Vsimple_test___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vsimple_test___024root___eval(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vsimple_test___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("simple_test.v", 7, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vsimple_test___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("simple_test.v", 7, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vsimple_test___024root___eval_phase__act(vlSelf));
    } while (Vsimple_test___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vsimple_test___024root___eval_debug_assertions(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_debug_assertions\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
