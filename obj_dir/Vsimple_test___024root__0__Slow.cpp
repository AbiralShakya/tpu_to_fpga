// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vsimple_test.h for the primary calling header

#include "Vsimple_test__pch.h"

VL_ATTR_COLD void Vsimple_test___024root___eval_static__TOP(Vsimple_test___024root* vlSelf);

VL_ATTR_COLD void Vsimple_test___024root___eval_static(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_static\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vsimple_test___024root___eval_static__TOP(vlSelf);
    vlSelfRef.__Vtrigprevexpr___TOP__simple_test__DOT__clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__simple_test__DOT__rst_n__0 = 0U;
}

VL_ATTR_COLD void Vsimple_test___024root___eval_static__TOP(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_static__TOP\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.simple_test__DOT__clk = 0U;
    vlSelfRef.simple_test__DOT__rst_n = 0U;
    vlSelfRef.simple_test__DOT__act_in = 0U;
    vlSelfRef.simple_test__DOT__weight_in = 0U;
    vlSelfRef.simple_test__DOT__psum_in = 0U;
    vlSelfRef.simple_test__DOT__en_weight_pass = 0U;
    vlSelfRef.simple_test__DOT__en_weight_capture = 0U;
}

VL_ATTR_COLD void Vsimple_test___024root___eval_final(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_final\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vsimple_test___024root___eval_settle(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___eval_settle\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

bool Vsimple_test___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsimple_test___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vsimple_test___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge simple_test.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge simple_test.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vsimple_test___024root___ctor_var_reset(Vsimple_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsimple_test___024root___ctor_var_reset\n"); );
    Vsimple_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->simple_test__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4099263053140583086ull);
    vlSelf->simple_test__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 681441767967128703ull);
    vlSelf->simple_test__DOT__act_in = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8136260327693224899ull);
    vlSelf->simple_test__DOT__weight_in = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17502878880671819393ull);
    vlSelf->simple_test__DOT__psum_in = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 6499666548836379556ull);
    vlSelf->simple_test__DOT__en_weight_pass = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 310102340954551308ull);
    vlSelf->simple_test__DOT__en_weight_capture = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3119568150801794265ull);
    vlSelf->simple_test__DOT__act_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16648825337499296547ull);
    vlSelf->simple_test__DOT__psum_out = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5488107463670505318ull);
    vlSelf->simple_test__DOT__pe_inst__DOT__weight_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13487822445737777527ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__simple_test__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15550655293891430499ull);
    vlSelf->__Vtrigprevexpr___TOP__simple_test__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10431402382041214113ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
