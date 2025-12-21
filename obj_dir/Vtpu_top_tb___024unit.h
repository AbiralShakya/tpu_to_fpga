// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtpu_top_tb.h for the primary calling header

#ifndef VERILATED_VTPU_TOP_TB___024UNIT_H_
#define VERILATED_VTPU_TOP_TB___024UNIT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtpu_top_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtpu_top_tb___024unit final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ __VmonitorOff;

    // INTERNAL VARIABLES
    Vtpu_top_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vtpu_top_tb___024unit(Vtpu_top_tb__Syms* symsp, const char* v__name);
    ~Vtpu_top_tb___024unit();
    VL_UNCOPYABLE(Vtpu_top_tb___024unit);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
