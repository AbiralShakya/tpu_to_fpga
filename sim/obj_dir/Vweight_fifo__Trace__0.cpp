// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vweight_fifo__Syms.h"


void Vweight_fifo___024root__trace_chg_0_sub_0(Vweight_fifo___024root* vlSelf, VerilatedVcd::Buffer* bufp);

void Vweight_fifo___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root__trace_chg_0\n"); );
    // Body
    Vweight_fifo___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vweight_fifo___024root*>(voidSelf);
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    Vweight_fifo___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vweight_fifo___024root__trace_chg_0_sub_0(Vweight_fifo___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root__trace_chg_0_sub_0\n"); );
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 1);
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[1U]))) {
        bufp->chgSData(oldp+0,(vlSelfRef.weight_fifo__DOT__buf0_rd_ptr),12);
        bufp->chgSData(oldp+1,(vlSelfRef.weight_fifo__DOT__buf0_wr_count),13);
        bufp->chgSData(oldp+2,(vlSelfRef.weight_fifo__DOT__buf0_rd_count),13);
        bufp->chgSData(oldp+3,(vlSelfRef.weight_fifo__DOT__buf1_rd_ptr),12);
        bufp->chgSData(oldp+4,(vlSelfRef.weight_fifo__DOT__buf1_wr_count),13);
        bufp->chgSData(oldp+5,(vlSelfRef.weight_fifo__DOT__buf1_rd_count),13);
        bufp->chgIData(oldp+6,(vlSelfRef.weight_fifo__DOT__selected_rd_data),24);
    }
    bufp->chgBit(oldp+7,(vlSelfRef.clk));
    bufp->chgBit(oldp+8,(vlSelfRef.rst_n));
    bufp->chgBit(oldp+9,(vlSelfRef.wt_buf_sel));
    bufp->chgCData(oldp+10,(vlSelfRef.wt_num_tiles),8);
    bufp->chgBit(oldp+11,(vlSelfRef.wr_en));
    bufp->chgIData(oldp+12,(vlSelfRef.wr_data),24);
    bufp->chgBit(oldp+13,(vlSelfRef.wr_full));
    bufp->chgBit(oldp+14,(vlSelfRef.wr_almost_full));
    bufp->chgBit(oldp+15,(vlSelfRef.rd_en));
    bufp->chgIData(oldp+16,(vlSelfRef.rd_data),24);
    bufp->chgBit(oldp+17,(vlSelfRef.rd_empty));
    bufp->chgBit(oldp+18,(vlSelfRef.rd_almost_empty));
    bufp->chgBit(oldp+19,(vlSelfRef.wt_load_done));
    bufp->chgSData(oldp+20,(vlSelfRef.weight_fifo__DOT__buf0_wr_ptr),12);
    bufp->chgSData(oldp+21,(vlSelfRef.weight_fifo__DOT__buf1_wr_ptr),12);
}

void Vweight_fifo___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vweight_fifo___024root__trace_cleanup\n"); );
    // Body
    Vweight_fifo___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vweight_fifo___024root*>(voidSelf);
    Vweight_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
}
