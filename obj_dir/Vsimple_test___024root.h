// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vsimple_test.h for the primary calling header

#ifndef VERILATED_VSIMPLE_TEST___024ROOT_H_
#define VERILATED_VSIMPLE_TEST___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vsimple_test__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vsimple_test___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ simple_test__DOT__clk;
    CData/*0:0*/ simple_test__DOT__rst_n;
    CData/*7:0*/ simple_test__DOT__act_in;
    CData/*7:0*/ simple_test__DOT__weight_in;
    CData/*0:0*/ simple_test__DOT__en_weight_pass;
    CData/*0:0*/ simple_test__DOT__en_weight_capture;
    CData/*7:0*/ simple_test__DOT__act_out;
    CData/*7:0*/ simple_test__DOT__pe_inst__DOT__weight_reg;
    CData/*0:0*/ __Vtrigprevexpr___TOP__simple_test__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__simple_test__DOT__rst_n__0;
    IData/*31:0*/ simple_test__DOT__psum_in;
    IData/*31:0*/ simple_test__DOT__psum_out;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;

    // INTERNAL VARIABLES
    Vsimple_test__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vsimple_test___024root(Vsimple_test__Syms* symsp, const char* v__name);
    ~Vsimple_test___024root();
    VL_UNCOPYABLE(Vsimple_test___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
