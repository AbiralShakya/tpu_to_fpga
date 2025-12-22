// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vweight_fifo.h for the primary calling header

#ifndef VERILATED_VWEIGHT_FIFO___024ROOT_H_
#define VERILATED_VWEIGHT_FIFO___024ROOT_H_  // guard

#include "verilated.h"


class Vweight_fifo__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vweight_fifo___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(rst_n,0,0);
    VL_IN8(wt_buf_sel,0,0);
    VL_IN8(wt_num_tiles,7,0);
    VL_IN8(wr_en,0,0);
    VL_OUT8(wr_full,0,0);
    VL_OUT8(wr_almost_full,0,0);
    VL_IN8(rd_en,0,0);
    VL_OUT8(rd_empty,0,0);
    VL_OUT8(rd_almost_empty,0,0);
    VL_OUT8(wt_load_done,0,0);
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__rst_n__0;
    SData/*11:0*/ weight_fifo__DOT__buf0_wr_ptr;
    SData/*11:0*/ weight_fifo__DOT__buf0_rd_ptr;
    SData/*12:0*/ weight_fifo__DOT__buf0_wr_count;
    SData/*12:0*/ weight_fifo__DOT__buf0_rd_count;
    SData/*11:0*/ weight_fifo__DOT__buf1_wr_ptr;
    SData/*11:0*/ weight_fifo__DOT__buf1_rd_ptr;
    SData/*12:0*/ weight_fifo__DOT__buf1_wr_count;
    SData/*12:0*/ weight_fifo__DOT__buf1_rd_count;
    SData/*11:0*/ __Vdly__weight_fifo__DOT__buf0_wr_ptr;
    SData/*11:0*/ __Vdly__weight_fifo__DOT__buf1_wr_ptr;
    VL_IN(wr_data,23,0);
    VL_OUT(rd_data,23,0);
    IData/*23:0*/ weight_fifo__DOT__selected_rd_data;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<IData/*23:0*/, 4096> weight_fifo__DOT__buffer0;
    VlUnpacked<IData/*23:0*/, 4096> weight_fifo__DOT__buffer1;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VicoTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vweight_fifo__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vweight_fifo___024root(Vweight_fifo__Syms* symsp, const char* v__name);
    ~Vweight_fifo___024root();
    VL_UNCOPYABLE(Vweight_fifo___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
