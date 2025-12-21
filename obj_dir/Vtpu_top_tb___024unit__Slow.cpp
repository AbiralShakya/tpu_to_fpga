// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtpu_top_tb.h for the primary calling header

#include "Vtpu_top_tb__pch.h"

void Vtpu_top_tb___024unit___ctor_var_reset(Vtpu_top_tb___024unit* vlSelf);

Vtpu_top_tb___024unit::Vtpu_top_tb___024unit(Vtpu_top_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vtpu_top_tb___024unit___ctor_var_reset(this);
}

void Vtpu_top_tb___024unit::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtpu_top_tb___024unit::~Vtpu_top_tb___024unit() {
}
