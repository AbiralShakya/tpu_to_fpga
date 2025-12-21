// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtpu_top_tb.h for the primary calling header

#include "Vtpu_top_tb__pch.h"

void Vtpu_top_tb___024root___ctor_var_reset(Vtpu_top_tb___024root* vlSelf);

Vtpu_top_tb___024root::Vtpu_top_tb___024root(Vtpu_top_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vtpu_top_tb___024root___ctor_var_reset(this);
}

void Vtpu_top_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtpu_top_tb___024root::~Vtpu_top_tb___024root() {
}
