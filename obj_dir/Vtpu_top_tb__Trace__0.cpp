// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vtpu_top_tb__Syms.h"


void Vtpu_top_tb___024root__trace_chg_0_sub_0(Vtpu_top_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp);

void Vtpu_top_tb___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root__trace_chg_0\n"); );
    // Body
    Vtpu_top_tb___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtpu_top_tb___024root*>(voidSelf);
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    Vtpu_top_tb___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vtpu_top_tb___024root__trace_chg_0_sub_0(Vtpu_top_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root__trace_chg_0_sub_0\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlWide<3>/*95:0*/ __Vtemp_1;
    VlWide<3>/*95:0*/ __Vtemp_2;
    VlWide<3>/*95:0*/ __Vtemp_3;
    // Body
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 1);
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[0U]))) {
        bufp->chgSData(oldp+0,(vlSelfRef.tpu_top_tb__DOT__weights_layer1[0]),16);
        bufp->chgSData(oldp+1,(vlSelfRef.tpu_top_tb__DOT__weights_layer1[1]),16);
        bufp->chgSData(oldp+2,(vlSelfRef.tpu_top_tb__DOT__weights_layer1[2]),16);
        bufp->chgSData(oldp+3,(vlSelfRef.tpu_top_tb__DOT__weights_layer1[3]),16);
        bufp->chgSData(oldp+4,(vlSelfRef.tpu_top_tb__DOT__weights_layer2[0]),16);
        bufp->chgSData(oldp+5,(vlSelfRef.tpu_top_tb__DOT__weights_layer2[1]),16);
        bufp->chgSData(oldp+6,(vlSelfRef.tpu_top_tb__DOT__weights_layer2[2]),16);
        bufp->chgSData(oldp+7,(vlSelfRef.tpu_top_tb__DOT__weights_layer2[3]),16);
        bufp->chgSData(oldp+8,(vlSelfRef.tpu_top_tb__DOT__activations[0]),16);
        bufp->chgSData(oldp+9,(vlSelfRef.tpu_top_tb__DOT__activations[1]),16);
        bufp->chgSData(oldp+10,(vlSelfRef.tpu_top_tb__DOT__activations[2]),16);
        bufp->chgSData(oldp+11,(vlSelfRef.tpu_top_tb__DOT__activations[3]),16);
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[1U]))) {
        bufp->chgIData(oldp+12,(vlSelfRef.tpu_top_tb__DOT__instr_memory
                                [(0x0000001fU & (IData)(vlSelfRef.tpu_top_tb__DOT__instr_addr_out))]),32);
        bufp->chgIData(oldp+13,(vlSelfRef.tpu_top_tb__DOT__instr_memory[0]),32);
        bufp->chgIData(oldp+14,(vlSelfRef.tpu_top_tb__DOT__instr_memory[1]),32);
        bufp->chgIData(oldp+15,(vlSelfRef.tpu_top_tb__DOT__instr_memory[2]),32);
        bufp->chgIData(oldp+16,(vlSelfRef.tpu_top_tb__DOT__instr_memory[3]),32);
        bufp->chgIData(oldp+17,(vlSelfRef.tpu_top_tb__DOT__instr_memory[4]),32);
        bufp->chgIData(oldp+18,(vlSelfRef.tpu_top_tb__DOT__instr_memory[5]),32);
        bufp->chgIData(oldp+19,(vlSelfRef.tpu_top_tb__DOT__instr_memory[6]),32);
        bufp->chgIData(oldp+20,(vlSelfRef.tpu_top_tb__DOT__instr_memory[7]),32);
        bufp->chgIData(oldp+21,(vlSelfRef.tpu_top_tb__DOT__instr_memory[8]),32);
        bufp->chgIData(oldp+22,(vlSelfRef.tpu_top_tb__DOT__instr_memory[9]),32);
        bufp->chgIData(oldp+23,(vlSelfRef.tpu_top_tb__DOT__instr_memory[10]),32);
        bufp->chgIData(oldp+24,(vlSelfRef.tpu_top_tb__DOT__instr_memory[11]),32);
        bufp->chgIData(oldp+25,(vlSelfRef.tpu_top_tb__DOT__instr_memory[12]),32);
        bufp->chgIData(oldp+26,(vlSelfRef.tpu_top_tb__DOT__instr_memory[13]),32);
        bufp->chgIData(oldp+27,(vlSelfRef.tpu_top_tb__DOT__instr_memory[14]),32);
        bufp->chgIData(oldp+28,(vlSelfRef.tpu_top_tb__DOT__instr_memory[15]),32);
        bufp->chgIData(oldp+29,(vlSelfRef.tpu_top_tb__DOT__instr_memory[16]),32);
        bufp->chgIData(oldp+30,(vlSelfRef.tpu_top_tb__DOT__instr_memory[17]),32);
        bufp->chgIData(oldp+31,(vlSelfRef.tpu_top_tb__DOT__instr_memory[18]),32);
        bufp->chgIData(oldp+32,(vlSelfRef.tpu_top_tb__DOT__instr_memory[19]),32);
        bufp->chgIData(oldp+33,(vlSelfRef.tpu_top_tb__DOT__instr_memory[20]),32);
        bufp->chgIData(oldp+34,(vlSelfRef.tpu_top_tb__DOT__instr_memory[21]),32);
        bufp->chgIData(oldp+35,(vlSelfRef.tpu_top_tb__DOT__instr_memory[22]),32);
        bufp->chgIData(oldp+36,(vlSelfRef.tpu_top_tb__DOT__instr_memory[23]),32);
        bufp->chgIData(oldp+37,(vlSelfRef.tpu_top_tb__DOT__instr_memory[24]),32);
        bufp->chgIData(oldp+38,(vlSelfRef.tpu_top_tb__DOT__instr_memory[25]),32);
        bufp->chgIData(oldp+39,(vlSelfRef.tpu_top_tb__DOT__instr_memory[26]),32);
        bufp->chgIData(oldp+40,(vlSelfRef.tpu_top_tb__DOT__instr_memory[27]),32);
        bufp->chgIData(oldp+41,(vlSelfRef.tpu_top_tb__DOT__instr_memory[28]),32);
        bufp->chgIData(oldp+42,(vlSelfRef.tpu_top_tb__DOT__instr_memory[29]),32);
        bufp->chgIData(oldp+43,(vlSelfRef.tpu_top_tb__DOT__instr_memory[30]),32);
        bufp->chgIData(oldp+44,(vlSelfRef.tpu_top_tb__DOT__instr_memory[31]),32);
        bufp->chgIData(oldp+45,(vlSelfRef.tpu_top_tb__DOT__unnamedblk1__DOT__i),32);
    }
    if (VL_UNLIKELY(((vlSelfRef.__Vm_traceActivity[1U] 
                      | vlSelfRef.__Vm_traceActivity
                      [2U])))) {
        bufp->chgBit(oldp+46,(vlSelfRef.tpu_top_tb__DOT__rst_n));
        bufp->chgBit(oldp+47,(vlSelfRef.tpu_top_tb__DOT__dma_start_in));
        bufp->chgBit(oldp+48,(vlSelfRef.tpu_top_tb__DOT__dma_dir_in));
        bufp->chgCData(oldp+49,(vlSelfRef.tpu_top_tb__DOT__dma_ub_addr_in),8);
        bufp->chgSData(oldp+50,(vlSelfRef.tpu_top_tb__DOT__dma_length_in),16);
        bufp->chgCData(oldp+51,(vlSelfRef.tpu_top_tb__DOT__dma_elem_sz_in),2);
        bufp->chgWData(oldp+52,(vlSelfRef.tpu_top_tb__DOT__dma_data_in),256);
        bufp->chgIData(oldp+60,(vlSelfRef.tpu_top_tb__DOT__unnamedblk2__DOT__i),32);
        bufp->chgIData(oldp+61,(vlSelfRef.tpu_top_tb__DOT__unnamedblk3__DOT__i),32);
        bufp->chgSData(oldp+62,(vlSelfRef.tpu_top_tb__DOT__unnamedblk4__DOT__cycle_count),10);
    }
    if (VL_UNLIKELY(((vlSelfRef.__Vm_traceActivity[1U] 
                      | vlSelfRef.__Vm_traceActivity
                      [4U])))) {
        bufp->chgIData(oldp+63,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [0U][0U]),32);
        bufp->chgIData(oldp+64,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [0U][1U]),32);
        bufp->chgIData(oldp+65,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [0U][2U]),32);
        bufp->chgIData(oldp+66,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [1U][0U]),32);
        bufp->chgIData(oldp+67,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [1U][1U]),32);
        bufp->chgIData(oldp+68,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [1U][2U]),32);
        bufp->chgIData(oldp+69,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [2U][0U]),32);
        bufp->chgIData(oldp+70,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [2U][1U]),32);
        bufp->chgIData(oldp+71,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [2U][2U]),32);
        bufp->chgIData(oldp+72,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [0U][0U]),32);
        bufp->chgIData(oldp+73,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [0U][1U]),32);
        bufp->chgIData(oldp+74,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [0U][2U]),32);
        bufp->chgIData(oldp+75,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [1U][0U]),32);
        bufp->chgIData(oldp+76,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [1U][1U]),32);
        bufp->chgIData(oldp+77,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [1U][2U]),32);
        bufp->chgIData(oldp+78,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [2U][0U]),32);
        bufp->chgIData(oldp+79,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [2U][1U]),32);
        bufp->chgIData(oldp+80,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                                [2U][2U]),32);
    }
    if (VL_UNLIKELY(((vlSelfRef.__Vm_traceActivity[1U] 
                      | vlSelfRef.__Vm_traceActivity
                      [6U])))) {
        bufp->chgBit(oldp+81,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_start));
        bufp->chgCData(oldp+82,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_rows),8);
        bufp->chgCData(oldp+83,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_addr),8);
        bufp->chgBit(oldp+84,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_fifo_wr));
        bufp->chgBit(oldp+85,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_start));
        bufp->chgCData(oldp+86,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_mode),4);
    }
    if (VL_UNLIKELY(((vlSelfRef.__Vm_traceActivity[3U] 
                      | vlSelfRef.__Vm_traceActivity
                      [5U])))) {
        bufp->chgSData(oldp+87,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data),16);
        bufp->chgWData(oldp+88,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data),256);
        bufp->chgIData(oldp+96,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data),24);
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[4U]))) {
        bufp->chgWData(oldp+97,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data),256);
        bufp->chgBit(oldp+105,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_tx));
        bufp->chgCData(oldp+106,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_state),8);
        bufp->chgCData(oldp+107,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_cmd),8);
        bufp->chgSData(oldp+108,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_byte_count),16);
        bufp->chgBit(oldp+109,((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))));
        bufp->chgBit(oldp+110,((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state))));
        bufp->chgBit(oldp+111,(((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__use_uart_dma) 
                                & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en))));
        bufp->chgBit(oldp+112,(((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__use_uart_dma) 
                                & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en))));
        bufp->chgBit(oldp+113,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en));
        bufp->chgCData(oldp+114,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_addr),8);
        bufp->chgWData(oldp+115,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data),256);
        bufp->chgBit(oldp+123,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en));
        bufp->chgCData(oldp+124,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_addr),8);
        bufp->chgBit(oldp+125,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_en));
        bufp->chgSData(oldp+126,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_addr),10);
        bufp->chgQData(oldp+127,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_data),64);
        bufp->chgBit(oldp+129,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_en));
        bufp->chgCData(oldp+130,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_addr),5);
        bufp->chgIData(oldp+131,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_data),32);
        bufp->chgBit(oldp+132,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_start_execution));
        bufp->chgBit(oldp+133,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__use_uart_dma));
        bufp->chgCData(oldp+134,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg),8);
        bufp->chgIData(oldp+135,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg),32);
        bufp->chgCData(oldp+136,((vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                                  >> 0x0000001aU)),6);
        bufp->chgCData(oldp+137,((0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                                                 >> 0x00000012U))),8);
        bufp->chgCData(oldp+138,((0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                                                 >> 0x0000000aU))),8);
        bufp->chgCData(oldp+139,((0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                                                 >> 2U))),8);
        bufp->chgCData(oldp+140,((3U & vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg)),2);
        bufp->chgCData(oldp+141,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_pc),8);
        bufp->chgCData(oldp+142,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_opcode),6);
        bufp->chgCData(oldp+143,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg1),8);
        bufp->chgCData(oldp+144,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg2),8);
        bufp->chgCData(oldp+145,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg3),8);
        bufp->chgCData(oldp+146,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_flags),2);
        bufp->chgCData(oldp+147,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_unit_sel),4);
        bufp->chgCData(oldp+148,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode),6);
        bufp->chgCData(oldp+149,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg1),8);
        bufp->chgCData(oldp+150,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg2),8);
        bufp->chgCData(oldp+151,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg3),8);
        bufp->chgCData(oldp+152,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_flags),2);
        bufp->chgBit(oldp+153,((4U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))));
        bufp->chgBit(oldp+154,((3U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state))));
        bufp->chgIData(oldp+155,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data),24);
        bufp->chgIData(oldp+156,((0x0000ffffU & vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[0U])),24);
        bufp->chgWData(oldp+157,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out),96);
        __Vtemp_1[0U] = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
            [0U][1U];
        __Vtemp_1[1U] = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                  [2U]
                                                  [1U])) 
                                  << 0x00000020U) | (QData)((IData)(
                                                                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                                    [1U]
                                                                    [1U]))));
        __Vtemp_1[2U] = (IData)(((((QData)((IData)(
                                                   vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                   [2U]
                                                   [1U])) 
                                   << 0x00000020U) 
                                  | (QData)((IData)(
                                                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                    [1U]
                                                    [1U]))) 
                                 >> 0x00000020U));
        bufp->chgWData(oldp+160,(__Vtemp_1),96);
        __Vtemp_2[0U] = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
            [0U][2U];
        __Vtemp_2[1U] = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                  [2U]
                                                  [2U])) 
                                  << 0x00000020U) | (QData)((IData)(
                                                                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                                    [1U]
                                                                    [2U]))));
        __Vtemp_2[2U] = (IData)(((((QData)((IData)(
                                                   vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                   [2U]
                                                   [2U])) 
                                   << 0x00000020U) 
                                  | (QData)((IData)(
                                                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                    [1U]
                                                    [2U]))) 
                                 >> 0x00000020U));
        bufp->chgWData(oldp+163,(__Vtemp_2),96);
        bufp->chgBit(oldp+166,((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))));
        bufp->chgBit(oldp+167,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col0));
        bufp->chgBit(oldp+168,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col1));
        bufp->chgBit(oldp+169,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col2));
        bufp->chgBit(oldp+170,((2U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))));
        bufp->chgQData(oldp+171,((((QData)((IData)(
                                                   vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U])) 
                                   << 0x00000020U) 
                                  | (QData)((IData)(
                                                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U])))),64);
        bufp->chgQData(oldp+173,((((QData)((IData)(
                                                   vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[1U])) 
                                   << 0x00000020U) 
                                  | (QData)((IData)(
                                                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[0U])))),64);
        bufp->chgWData(oldp+175,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data),256);
        bufp->chgBit(oldp+183,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_valid));
        bufp->chgBit(oldp+184,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_a_valid));
        bufp->chgBit(oldp+185,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_b_ready));
        bufp->chgBit(oldp+186,(((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state)) 
                                | (0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state)))));
        bufp->chgBit(oldp+187,(((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state)) 
                                & ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state)) 
                                   & ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count)) 
                                      & (0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count)))))));
        bufp->chgBit(oldp+188,(((0x0010U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count)) 
                                | (0x0010U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count)))));
        __Vtemp_3[0U] = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U])) 
                                  << 0x00000020U) | (QData)((IData)(
                                                                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U]))));
        __Vtemp_3[1U] = (IData)(((((QData)((IData)(
                                                   vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U])) 
                                   << 0x00000020U) 
                                  | (QData)((IData)(
                                                    vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U]))) 
                                 >> 0x00000020U));
        __Vtemp_3[2U] = 0U;
        bufp->chgWData(oldp+189,(__Vtemp_3),96);
        bufp->chgWData(oldp+192,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data),96);
        bufp->chgCData(oldp+195,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [0U][0U]),8);
        bufp->chgCData(oldp+196,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [0U][1U]),8);
        bufp->chgCData(oldp+197,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [0U][2U]),8);
        bufp->chgCData(oldp+198,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [1U][0U]),8);
        bufp->chgCData(oldp+199,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [1U][1U]),8);
        bufp->chgCData(oldp+200,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [1U][2U]),8);
        bufp->chgCData(oldp+201,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [2U][0U]),8);
        bufp->chgCData(oldp+202,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [2U][1U]),8);
        bufp->chgCData(oldp+203,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [2U][2U]),8);
        bufp->chgCData(oldp+204,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                                 [0U][0U]),8);
        bufp->chgCData(oldp+205,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                                 [0U][1U]),8);
        bufp->chgCData(oldp+206,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                                 [0U][2U]),8);
        bufp->chgCData(oldp+207,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                                 [1U][0U]),8);
        bufp->chgCData(oldp+208,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                                 [1U][1U]),8);
        bufp->chgCData(oldp+209,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                                 [1U][2U]),8);
        bufp->chgCData(oldp+210,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                                 [2U][0U]),8);
        bufp->chgCData(oldp+211,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                                 [2U][1U]),8);
        bufp->chgCData(oldp+212,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                                 [2U][2U]),8);
        bufp->chgIData(oldp+213,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                 [0U][0U]),32);
        bufp->chgIData(oldp+214,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                 [0U][1U]),32);
        bufp->chgIData(oldp+215,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                 [0U][2U]),32);
        bufp->chgIData(oldp+216,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                 [1U][0U]),32);
        bufp->chgIData(oldp+217,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                 [1U][1U]),32);
        bufp->chgIData(oldp+218,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                 [1U][2U]),32);
        bufp->chgIData(oldp+219,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                 [2U][0U]),32);
        bufp->chgIData(oldp+220,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                 [2U][1U]),32);
        bufp->chgIData(oldp+221,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                 [2U][2U]),32);
        bufp->chgCData(oldp+222,((0x000000ffU & vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data)),8);
        bufp->chgCData(oldp+223,((0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
                                                 >> 8U))),8);
        bufp->chgCData(oldp+224,((0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
                                                 >> 0x00000010U))),8);
        bufp->chgCData(oldp+225,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row1_delayed),8);
        bufp->chgCData(oldp+226,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row2_delayed),8);
        bufp->chgCData(oldp+227,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [0U][0U]),8);
        bufp->chgCData(oldp+228,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_00__act_out),8);
        bufp->chgIData(oldp+229,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result),32);
        bufp->chgCData(oldp+230,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__weight_reg),8);
        bufp->chgSData(oldp+231,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mult_result),16);
        bufp->chgCData(oldp+232,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [0U][1U]),8);
        bufp->chgCData(oldp+233,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_01__act_out),8);
        bufp->chgIData(oldp+234,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result),32);
        bufp->chgCData(oldp+235,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__weight_reg),8);
        bufp->chgSData(oldp+236,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mult_result),16);
        bufp->chgCData(oldp+237,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [0U][2U]),8);
        bufp->chgCData(oldp+238,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_02__act_out),8);
        bufp->chgIData(oldp+239,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result),32);
        bufp->chgCData(oldp+240,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__weight_reg),8);
        bufp->chgSData(oldp+241,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mult_result),16);
        bufp->chgCData(oldp+242,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [1U][0U]),8);
        bufp->chgCData(oldp+243,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_10__act_out),8);
        bufp->chgIData(oldp+244,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result),32);
        bufp->chgCData(oldp+245,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__weight_reg),8);
        bufp->chgSData(oldp+246,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mult_result),16);
        bufp->chgCData(oldp+247,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [1U][1U]),8);
        bufp->chgCData(oldp+248,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_11__act_out),8);
        bufp->chgIData(oldp+249,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result),32);
        bufp->chgCData(oldp+250,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__weight_reg),8);
        bufp->chgSData(oldp+251,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mult_result),16);
        bufp->chgCData(oldp+252,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [1U][2U]),8);
        bufp->chgCData(oldp+253,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_12__act_out),8);
        bufp->chgIData(oldp+254,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result),32);
        bufp->chgCData(oldp+255,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__weight_reg),8);
        bufp->chgSData(oldp+256,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mult_result),16);
        bufp->chgCData(oldp+257,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [2U][0U]),8);
        bufp->chgCData(oldp+258,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_20__act_out),8);
        bufp->chgIData(oldp+259,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result),32);
        bufp->chgCData(oldp+260,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__weight_reg),8);
        bufp->chgSData(oldp+261,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mult_result),16);
        bufp->chgCData(oldp+262,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [2U][1U]),8);
        bufp->chgCData(oldp+263,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_21__act_out),8);
        bufp->chgIData(oldp+264,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result),32);
        bufp->chgCData(oldp+265,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__weight_reg),8);
        bufp->chgSData(oldp+266,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mult_result),16);
        bufp->chgCData(oldp+267,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                                 [2U][2U]),8);
        bufp->chgCData(oldp+268,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_22__act_out),8);
        bufp->chgIData(oldp+269,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result),32);
        bufp->chgCData(oldp+270,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__weight_reg),8);
        bufp->chgSData(oldp+271,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mult_result),16);
        bufp->chgCData(oldp+272,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state),3);
        bufp->chgCData(oldp+273,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__weight_load_counter),8);
        bufp->chgCData(oldp+274,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__compute_counter),8);
        bufp->chgCData(oldp+275,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__total_rows),8);
        bufp->chgCData(oldp+276,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state),2);
        bufp->chgCData(oldp+277,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count),8);
        bufp->chgCData(oldp+278,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr),8);
        bufp->chgCData(oldp+279,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state),2);
        bufp->chgCData(oldp+280,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count),8);
        bufp->chgCData(oldp+281,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr),8);
        bufp->chgCData(oldp+282,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state),2);
        bufp->chgCData(oldp+283,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter),8);
        bufp->chgIData(oldp+284,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[0]),32);
        bufp->chgIData(oldp+285,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[1]),32);
        bufp->chgIData(oldp+286,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[2]),32);
        bufp->chgIData(oldp+287,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[3]),32);
        bufp->chgIData(oldp+288,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[4]),32);
        bufp->chgIData(oldp+289,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[5]),32);
        bufp->chgIData(oldp+290,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[6]),32);
        bufp->chgIData(oldp+291,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[7]),32);
        bufp->chgIData(oldp+292,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result0),32);
        bufp->chgIData(oldp+293,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result1),32);
        bufp->chgSData(oldp+294,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_ptr),12);
        bufp->chgSData(oldp+295,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr),12);
        bufp->chgSData(oldp+296,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count),13);
        bufp->chgSData(oldp+297,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_count),13);
        bufp->chgSData(oldp+298,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_ptr),12);
        bufp->chgSData(oldp+299,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr),12);
        bufp->chgSData(oldp+300,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count),13);
        bufp->chgSData(oldp+301,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_count),13);
        bufp->chgCData(oldp+302,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data),8);
        bufp->chgBit(oldp+303,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid));
        bufp->chgBit(oldp+304,((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state))));
        bufp->chgCData(oldp+305,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_data),8);
        bufp->chgBit(oldp+306,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_valid));
        bufp->chgBit(oldp+307,((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state))));
        bufp->chgCData(oldp+308,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state),8);
        bufp->chgCData(oldp+309,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command),8);
        bufp->chgCData(oldp+310,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi),8);
        bufp->chgCData(oldp+311,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo),8);
        bufp->chgSData(oldp+312,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length),16);
        bufp->chgSData(oldp+313,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count),16);
        bufp->chgCData(oldp+314,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index),5);
        bufp->chgWData(oldp+315,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer),256);
        bufp->chgQData(oldp+323,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer),64);
        bufp->chgIData(oldp+325,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer),32);
        bufp->chgWData(oldp+326,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer),256);
        bufp->chgCData(oldp+334,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index),8);
        bufp->chgCData(oldp+335,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state),2);
        bufp->chgSData(oldp+336,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count),16);
        bufp->chgCData(oldp+337,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index),3);
        bufp->chgCData(oldp+338,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_byte),8);
        bufp->chgCData(oldp+339,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state),2);
        bufp->chgSData(oldp+340,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count),16);
        bufp->chgCData(oldp+341,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index),3);
        bufp->chgCData(oldp+342,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__tx_byte),8);
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[6U]))) {
        bufp->chgBit(oldp+343,(vlSelfRef.tpu_top_tb__DOT__tpu_busy));
        bufp->chgBit(oldp+344,((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)))));
        bufp->chgCData(oldp+345,(((IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)
                                   ? 0U : ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid)
                                            ? 2U : 1U))),2);
        bufp->chgBit(oldp+346,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_valid));
        bufp->chgBit(oldp+347,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid));
        bufp->chgCData(oldp+348,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state),3);
    }
    bufp->chgBit(oldp+349,(vlSelfRef.tpu_top_tb__DOT__clk));
    bufp->chgBit(oldp+350,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel));
    bufp->chgBit(oldp+351,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel));
    bufp->chgBit(oldp+352,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_start));
    bufp->chgBit(oldp+353,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_dir));
    bufp->chgCData(oldp+354,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_ub_addr),8);
    bufp->chgSData(oldp+355,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_length),16);
    bufp->chgCData(oldp+356,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_elem_sz),2);
    bufp->chgBit(oldp+357,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_en));
    bufp->chgBit(oldp+358,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_empty));
    bufp->chgBit(oldp+359,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full));
    bufp->chgBit(oldp+360,(((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel)
                             ? (0x0ffcU <= (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count))
                             : (0x0ffcU <= (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count)))));
    bufp->chgBit(oldp+361,(((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel)
                             ? (4U >= ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count) 
                                       - (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_count)))
                             : (4U >= ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count) 
                                       - (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_count))))));
    bufp->chgBit(oldp+362,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_1));
    bufp->chgBit(oldp+363,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_2));
}

void Vtpu_top_tb___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root__trace_cleanup\n"); );
    // Body
    Vtpu_top_tb___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtpu_top_tb___024root*>(voidSelf);
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[2U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[3U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[4U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[5U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[6U] = 0U;
}
