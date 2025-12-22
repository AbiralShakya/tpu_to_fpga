// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vsimple_test.h for the primary calling header

#include "Vsimple_test__pch.h"

void Vsimple_test___024root___ctor_var_reset(Vsimple_test___024root* vlSelf);

Vsimple_test___024root::Vsimple_test___024root(Vsimple_test__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vsimple_test___024root___ctor_var_reset(this);
}

void Vsimple_test___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vsimple_test___024root::~Vsimple_test___024root() {
}
