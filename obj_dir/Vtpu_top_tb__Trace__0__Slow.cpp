// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vtpu_top_tb__Syms.h"


VL_ATTR_COLD void Vtpu_top_tb___024root__trace_init_sub__TOP__0(Vtpu_top_tb___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root__trace_init_sub__TOP__0\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const int c = vlSymsp->__Vm_baseCode;
    tracep->pushPrefix("tpu_top_tb", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+13,0,"instr_data_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+365,0,"instr_addr_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+48,0,"dma_start_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+49,0,"dma_dir_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+50,0,"dma_ub_addr_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+51,0,"dma_length_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+52,0,"dma_elem_sz_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declArray(c+53,0,"dma_data_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+366,0,"dma_busy_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+367,0,"dma_done_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declArray(c+98,0,"dma_data_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+344,0,"tpu_busy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+345,0,"tpu_done",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+346,0,"pipeline_stage",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+344,0,"hazard_detected",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("instr_memory", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+14+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 31,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("weights_layer1", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBus(c+1+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("weights_layer2", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBus(c+5+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("activations", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBus(c+9+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("dut", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"uart_rx",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+106,0,"uart_tx",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+48,0,"dma_start_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+49,0,"dma_dir_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+50,0,"dma_ub_addr_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+51,0,"dma_length_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+52,0,"dma_elem_sz_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declArray(c+53,0,"dma_data_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+366,0,"dma_busy_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+367,0,"dma_done_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declArray(c+98,0,"dma_data_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+344,0,"tpu_busy",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+345,0,"tpu_done",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+346,0,"pipeline_stage",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+344,0,"hazard_detected",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+107,0,"uart_debug_state",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+108,0,"uart_debug_cmd",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+109,0,"uart_debug_byte_count",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+82,0,"sys_start",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+83,0,"sys_rows",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+84,0,"ub_rd_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+85,0,"wt_fifo_wr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+86,0,"vpu_start",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+87,0,"vpu_mode",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+351,0,"wt_buf_sel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+352,0,"acc_buf_sel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+110,0,"sys_busy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+111,0,"vpu_busy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"dma_busy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+88,0,"wt_fifo_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declArray(c+89,0,"ub_wr_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declArray(c+98,0,"ub_rd_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+112,0,"ub_wr_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+113,0,"ub_rd_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+114,0,"uart_ub_wr_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+115,0,"uart_ub_wr_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declArray(c+116,0,"uart_ub_wr_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+124,0,"uart_ub_rd_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+125,0,"uart_ub_rd_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+126,0,"uart_wt_wr_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+127,0,"uart_wt_wr_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declQuad(c+128,0,"uart_wt_wr_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBit(c+130,0,"uart_instr_wr_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+131,0,"uart_instr_wr_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+132,0,"uart_instr_wr_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+133,0,"uart_start_execution",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+353,0,"dma_start",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+354,0,"dma_dir",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+355,0,"dma_ub_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+356,0,"dma_length",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+357,0,"dma_elem_sz",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+344,0,"pipeline_stall",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+346,0,"current_stage",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+134,0,"use_uart_dma",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+133,0,"controller_start",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("controller", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+135,0,"instr_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+368,0,"instr_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+110,0,"sys_busy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+111,0,"vpu_busy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"dma_busy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+82,0,"sys_start",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+83,0,"sys_rows",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+84,0,"ub_rd_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+85,0,"wt_fifo_wr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+86,0,"vpu_start",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+87,0,"vpu_mode",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+351,0,"wt_buf_sel",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+352,0,"acc_buf_sel",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+353,0,"dma_start",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+354,0,"dma_dir",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+355,0,"dma_ub_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+356,0,"dma_length",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+357,0,"dma_elem_sz",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+344,0,"pipeline_stall",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+346,0,"current_stage",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+369,0,"OPCODE_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+370,0,"MATMUL_OP",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+371,0,"RD_WEIGHT_OP",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+372,0,"RELU_OP",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+373,0,"SYNC_OP",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+135,0,"pc_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+367,0,"pc_cnt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"pc_ld",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+136,0,"ir_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+367,0,"ir_ld",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+137,0,"opcode",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+138,0,"arg1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+139,0,"arg2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+140,0,"arg3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+141,0,"flags",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+374,0,"unit_sel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+347,0,"if_id_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+142,0,"if_id_pc",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+143,0,"if_id_opcode",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+144,0,"if_id_arg1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+145,0,"if_id_arg2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+146,0,"if_id_arg3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+147,0,"if_id_flags",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+148,0,"if_id_unit_sel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+344,0,"if_id_stall",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"if_id_flush",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+348,0,"exec_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+149,0,"exec_opcode",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+150,0,"exec_arg1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+151,0,"exec_arg2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+152,0,"exec_arg3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+153,0,"exec_flags",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+344,0,"hazard_detected",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->pushPrefix("datapath", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+82,0,"sys_start",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+83,0,"sys_rows",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+84,0,"ub_rd_addr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+85,0,"wt_fifo_wr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+86,0,"vpu_start",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+87,0,"vpu_mode",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+351,0,"wt_buf_sel",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+352,0,"acc_buf_sel",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+88,0,"wt_fifo_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declArray(c+89,0,"ub_wr_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declArray(c+98,0,"ub_rd_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+110,0,"sys_busy",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+154,0,"sys_done",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+111,0,"vpu_busy",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+155,0,"vpu_done",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"dma_busy",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+367,0,"dma_done",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+156,0,"weight_to_sys",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+157,0,"act_to_sys",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declArray(c+158,0,"psum_col0_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->declArray(c+161,0,"psum_col1_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->declArray(c+164,0,"psum_col2_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+168,0,"en_capture_col0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+169,0,"en_capture_col1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+170,0,"en_capture_col2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+171,0,"systolic_active",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+171,0,"acc_wr_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+375,0,"acc_wr_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declQuad(c+172,0,"acc_wr_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBit(c+86,0,"acc_rd_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+375,0,"acc_rd_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declQuad(c+174,0,"acc_rd_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declArray(c+176,0,"vpu_out_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+184,0,"vpu_out_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+82,0,"ub_port_a_rd_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+84,0,"ub_port_a_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+83,0,"ub_port_a_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+185,0,"ub_port_a_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+184,0,"ub_port_b_wr_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+375,0,"ub_port_b_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+376,0,"ub_port_b_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+186,0,"ub_port_b_ready",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+187,0,"ub_busy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+188,0,"ub_done",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+358,0,"wt_rd_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+359,0,"wt_rd_empty",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+360,0,"wt_wr_full",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+189,0,"wt_load_done",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("accumulators", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+377,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+378,0,"ARRAY_SIZE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+379,0,"BUFFER_DEPTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+352,0,"acc_buf_sel",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+171,0,"wr_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+375,0,"wr_addr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declArray(c+190,0,"wr_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->declBit(c+86,0,"rd_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+375,0,"rd_addr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declArray(c+193,0,"rd_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->declBit(c+366,0,"acc_busy",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declArray(c+193,0,"selected_rd_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->popPrefix();
    tracep->pushPrefix("systolic_array", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+380,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"ACC_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+378,0,"ARRAY_SIZE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+168,0,"en_capture_col0",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+169,0,"en_capture_col1",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+170,0,"en_capture_col2",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+156,0,"weight_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+157,0,"act_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declArray(c+158,0,"psum_col0_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->declArray(c+161,0,"psum_col1_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->declArray(c+164,0,"psum_col2_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->pushPrefix("pe_act_in", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->pushPrefix("[0]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+196,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+197,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+198,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->pushPrefix("[1]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+199,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+200,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+201,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->pushPrefix("[2]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+202,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+203,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+204,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("pe_act_out", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->pushPrefix("[0]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+205,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+206,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+207,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->pushPrefix("[1]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+208,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+209,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+210,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->pushPrefix("[2]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+211,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+212,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+213,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("pe_psum_in", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->pushPrefix("[0]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+64,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+65,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+66,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("[1]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+67,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+68,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+69,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("[2]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+70,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+71,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+72,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("pe_psum_out", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->pushPrefix("[0]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+214,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+215,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+216,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("[1]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+217,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+218,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+219,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("[2]", VerilatedTracePrefixType::ARRAY_UNPACKED);
    tracep->declBus(c+220,0,"[0]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+221,0,"[1]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+222,0,"[2]",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->declBus(c+223,0,"weight_col0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+224,0,"weight_col1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+225,0,"weight_col2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+226,0,"act_row1_delayed",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+227,0,"act_row2_delayed",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("pe_00", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+380,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"ACC_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+168,0,"en_weight_capture",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+223,0,"weight_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+228,0,"act_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+73,0,"psum_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+229,0,"act_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+230,0,"psum_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+231,0,"weight_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+232,0,"mult_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+230,0,"mac_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("pe_01", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+380,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"ACC_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+169,0,"en_weight_capture",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+224,0,"weight_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+233,0,"act_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+74,0,"psum_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+234,0,"act_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+235,0,"psum_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+236,0,"weight_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+237,0,"mult_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+235,0,"mac_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("pe_02", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+380,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"ACC_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+170,0,"en_weight_capture",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+225,0,"weight_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+238,0,"act_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+75,0,"psum_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+239,0,"act_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+240,0,"psum_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+241,0,"weight_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+242,0,"mult_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+240,0,"mac_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("pe_10", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+380,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"ACC_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+168,0,"en_weight_capture",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+223,0,"weight_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+243,0,"act_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+76,0,"psum_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+244,0,"act_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+245,0,"psum_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+246,0,"weight_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+247,0,"mult_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+245,0,"mac_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("pe_11", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+380,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"ACC_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+169,0,"en_weight_capture",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+224,0,"weight_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+248,0,"act_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+77,0,"psum_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+249,0,"act_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+250,0,"psum_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+251,0,"weight_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+252,0,"mult_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+250,0,"mac_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("pe_12", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+380,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"ACC_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+170,0,"en_weight_capture",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+225,0,"weight_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+253,0,"act_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+78,0,"psum_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+254,0,"act_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+255,0,"psum_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+256,0,"weight_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+257,0,"mult_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+255,0,"mac_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("pe_20", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+380,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"ACC_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+168,0,"en_weight_capture",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+223,0,"weight_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+258,0,"act_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+79,0,"psum_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+259,0,"act_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+260,0,"psum_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+261,0,"weight_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+262,0,"mult_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+260,0,"mac_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("pe_21", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+380,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"ACC_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+169,0,"en_weight_capture",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+224,0,"weight_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+263,0,"act_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+80,0,"psum_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+264,0,"act_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+265,0,"psum_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+266,0,"weight_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+267,0,"mult_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+265,0,"mac_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("pe_22", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+380,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"ACC_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+170,0,"en_weight_capture",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+225,0,"weight_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+268,0,"act_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+81,0,"psum_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+269,0,"act_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+270,0,"psum_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+271,0,"weight_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+272,0,"mult_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+270,0,"mac_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("systolic_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+82,0,"sys_start",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+83,0,"sys_rows",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+110,0,"sys_busy",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+154,0,"sys_done",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+167,0,"en_weight_pass",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+168,0,"en_capture_col0",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+169,0,"en_capture_col1",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+170,0,"en_capture_col2",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+171,0,"systolic_active",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+273,0,"current_state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+349,0,"next_state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+274,0,"weight_load_counter",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+275,0,"compute_counter",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+276,0,"total_rows",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->pushPrefix("ub", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+379,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+379,0,"DEPTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+380,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+82,0,"port_a_rd_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+84,0,"port_a_addr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+83,0,"port_a_count",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declArray(c+98,0,"port_a_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+185,0,"port_a_valid",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+184,0,"port_b_wr_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+375,0,"port_b_addr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+376,0,"port_b_count",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declArray(c+89,0,"port_b_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+186,0,"port_b_ready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+187,0,"ub_busy",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+188,0,"ub_done",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+277,0,"port_a_state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+278,0,"port_a_burst_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+279,0,"port_a_current_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+280,0,"port_b_state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+281,0,"port_b_burst_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+282,0,"port_b_current_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->pushPrefix("vpu_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+86,0,"vpu_start",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+87,0,"vpu_mode",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+375,0,"vpu_in_addr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+375,0,"vpu_out_addr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+381,0,"vpu_length",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declQuad(c+174,0,"vpu_in_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declArray(c+176,0,"vpu_out_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+184,0,"vpu_out_valid",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+111,0,"vpu_busy",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+155,0,"vpu_done",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+382,0,"MODE_RELU",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+383,0,"MODE_SIGMOID",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+384,0,"MODE_TANH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+385,0,"MODE_POOL",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+386,0,"MODE_BATCHNORM",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+283,0,"current_state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+284,0,"process_counter",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("data_buffer", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 8; ++i) {
        tracep->declBus(c+285+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 31,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+293,0,"relu_result0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+294,0,"relu_result1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("weight_fifo_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+387,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+388,0,"DEPTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+389,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+351,0,"wt_buf_sel",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+390,0,"wt_num_tiles",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+85,0,"wr_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+97,0,"wr_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBit(c+360,0,"wr_full",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+361,0,"wr_almost_full",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+358,0,"rd_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+156,0,"rd_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBit(c+359,0,"rd_empty",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+362,0,"rd_almost_empty",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+189,0,"wt_load_done",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+295,0,"buf0_wr_ptr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 11,0);
    tracep->declBus(c+296,0,"buf0_rd_ptr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 11,0);
    tracep->declBus(c+297,0,"buf0_wr_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 12,0);
    tracep->declBus(c+298,0,"buf0_rd_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 12,0);
    tracep->declBus(c+299,0,"buf1_wr_ptr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 11,0);
    tracep->declBus(c+300,0,"buf1_rd_ptr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 11,0);
    tracep->declBus(c+301,0,"buf1_wr_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 12,0);
    tracep->declBus(c+302,0,"buf1_rd_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 12,0);
    tracep->declBus(c+391,0,"selected_wr_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+156,0,"selected_rd_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBit(c+360,0,"selected_wr_full",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+361,0,"selected_wr_almost_full",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+359,0,"selected_rd_empty",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+362,0,"selected_rd_almost_empty",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("uart_dma", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+392,0,"CLOCK_FREQ",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+393,0,"BAUD_RATE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"uart_rx",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+106,0,"uart_tx",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+114,0,"ub_wr_en",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+115,0,"ub_wr_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declArray(c+116,0,"ub_wr_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+124,0,"ub_rd_en",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+125,0,"ub_rd_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declArray(c+98,0,"ub_rd_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBit(c+126,0,"wt_wr_en",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+127,0,"wt_wr_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declQuad(c+128,0,"wt_wr_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBit(c+130,0,"instr_wr_en",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+131,0,"instr_wr_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+132,0,"instr_wr_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+133,0,"start_execution",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+110,0,"sys_busy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+367,0,"sys_done",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+111,0,"vpu_busy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+367,0,"vpu_done",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"ub_busy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+367,0,"ub_done",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+107,0,"debug_state",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+108,0,"debug_cmd",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+109,0,"debug_byte_count",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+303,0,"rx_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+304,0,"rx_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+305,0,"rx_ready",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+306,0,"tx_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+307,0,"tx_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+308,0,"tx_ready",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+375,0,"IDLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+376,0,"READ_CMD",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+394,0,"READ_ADDR_HI",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+395,0,"READ_ADDR_LO",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+396,0,"READ_LENGTH_HI",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+397,0,"READ_LENGTH_LO",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+398,0,"WRITE_UB",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+399,0,"WRITE_WT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+381,0,"WRITE_INSTR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+400,0,"READ_UB",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+401,0,"SEND_STATUS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+402,0,"EXECUTE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+309,0,"state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+310,0,"command",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+311,0,"addr_hi",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+312,0,"addr_lo",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+313,0,"length",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+314,0,"byte_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+315,0,"byte_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declArray(c+316,0,"ub_buffer",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declQuad(c+324,0,"wt_buffer",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBus(c+326,0,"instr_buffer",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declArray(c+327,0,"read_buffer",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 255,0);
    tracep->declBus(c+335,0,"read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("uart_rx_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+392,0,"CLOCK_FREQ",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+393,0,"BAUD_RATE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"rx",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+303,0,"rx_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+304,0,"rx_valid",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+305,0,"rx_ready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+403,0,"CLKS_PER_BIT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+404,0,"HALF_BIT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+405,0,"IDLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+406,0,"START",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+407,0,"DATA",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+408,0,"STOP",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+336,0,"state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+337,0,"clk_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+338,0,"bit_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+339,0,"rx_byte",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+363,0,"rx_sync_1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+364,0,"rx_sync_2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->pushPrefix("uart_tx_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+392,0,"CLOCK_FREQ",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+393,0,"BAUD_RATE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+350,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+106,0,"tx",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+306,0,"tx_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+307,0,"tx_valid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+308,0,"tx_ready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+403,0,"CLKS_PER_BIT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+405,0,"IDLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+406,0,"START",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+407,0,"DATA",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+408,0,"STOP",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+340,0,"state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+341,0,"clk_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+342,0,"bit_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+343,0,"tx_byte",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+46,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+61,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+62,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+63,0,"cycle_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->popPrefix();
    tracep->popPrefix();
}

VL_ATTR_COLD void Vtpu_top_tb___024root__trace_init_top(Vtpu_top_tb___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root__trace_init_top\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtpu_top_tb___024root__trace_init_sub__TOP__0(vlSelf, tracep);
}

VL_ATTR_COLD void Vtpu_top_tb___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
VL_ATTR_COLD void Vtpu_top_tb___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vtpu_top_tb___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vtpu_top_tb___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/);

VL_ATTR_COLD void Vtpu_top_tb___024root__trace_register(Vtpu_top_tb___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root__trace_register\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    tracep->addConstCb(&Vtpu_top_tb___024root__trace_const_0, 0, vlSelf);
    tracep->addFullCb(&Vtpu_top_tb___024root__trace_full_0, 0, vlSelf);
    tracep->addChgCb(&Vtpu_top_tb___024root__trace_chg_0, 0, vlSelf);
    tracep->addCleanupCb(&Vtpu_top_tb___024root__trace_cleanup, vlSelf);
}

VL_ATTR_COLD void Vtpu_top_tb___024root__trace_const_0_sub_0(Vtpu_top_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vtpu_top_tb___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root__trace_const_0\n"); );
    // Body
    Vtpu_top_tb___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtpu_top_tb___024root*>(voidSelf);
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    Vtpu_top_tb___024root__trace_const_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vtpu_top_tb___024root__trace_const_0_sub_0(Vtpu_top_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root__trace_const_0_sub_0\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    bufp->fullCData(oldp+365,(vlSelfRef.tpu_top_tb__DOT__instr_addr_out),8);
    bufp->fullBit(oldp+366,(0U));
    bufp->fullBit(oldp+367,(1U));
    bufp->fullIData(oldp+368,(0U),32);
    bufp->fullIData(oldp+369,(6U),32);
    bufp->fullCData(oldp+370,(1U),6);
    bufp->fullCData(oldp+371,(2U),6);
    bufp->fullCData(oldp+372,(3U),6);
    bufp->fullCData(oldp+373,(4U),6);
    bufp->fullCData(oldp+374,(0U),4);
    bufp->fullCData(oldp+375,(0U),8);
    bufp->fullCData(oldp+376,(1U),8);
    bufp->fullIData(oldp+377,(0x00000020U),32);
    bufp->fullIData(oldp+378,(3U),32);
    bufp->fullIData(oldp+379,(0x00000100U),32);
    bufp->fullIData(oldp+380,(8U),32);
    bufp->fullCData(oldp+381,(8U),8);
    bufp->fullCData(oldp+382,(1U),4);
    bufp->fullCData(oldp+383,(2U),4);
    bufp->fullCData(oldp+384,(3U),4);
    bufp->fullCData(oldp+385,(4U),4);
    bufp->fullCData(oldp+386,(5U),4);
    bufp->fullIData(oldp+387,(0x00000018U),32);
    bufp->fullIData(oldp+388,(0x00001000U),32);
    bufp->fullIData(oldp+389,(0x0000000cU),32);
    bufp->fullCData(oldp+390,(0x10U),8);
    bufp->fullIData(oldp+391,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_wr_data),24);
    bufp->fullIData(oldp+392,(0x05f5e100U),32);
    bufp->fullIData(oldp+393,(0x0001c200U),32);
    bufp->fullCData(oldp+394,(2U),8);
    bufp->fullCData(oldp+395,(3U),8);
    bufp->fullCData(oldp+396,(4U),8);
    bufp->fullCData(oldp+397,(5U),8);
    bufp->fullCData(oldp+398,(6U),8);
    bufp->fullCData(oldp+399,(7U),8);
    bufp->fullCData(oldp+400,(9U),8);
    bufp->fullCData(oldp+401,(0x0aU),8);
    bufp->fullCData(oldp+402,(0x0bU),8);
    bufp->fullIData(oldp+403,(0x00000364U),32);
    bufp->fullIData(oldp+404,(0x000001b2U),32);
    bufp->fullCData(oldp+405,(0U),2);
    bufp->fullCData(oldp+406,(1U),2);
    bufp->fullCData(oldp+407,(2U),2);
    bufp->fullCData(oldp+408,(3U),2);
}

VL_ATTR_COLD void Vtpu_top_tb___024root__trace_full_0_sub_0(Vtpu_top_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vtpu_top_tb___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root__trace_full_0\n"); );
    // Body
    Vtpu_top_tb___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtpu_top_tb___024root*>(voidSelf);
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    Vtpu_top_tb___024root__trace_full_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vtpu_top_tb___024root__trace_full_0_sub_0(Vtpu_top_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtpu_top_tb___024root__trace_full_0_sub_0\n"); );
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlWide<3>/*95:0*/ __Vtemp_1;
    VlWide<3>/*95:0*/ __Vtemp_2;
    VlWide<3>/*95:0*/ __Vtemp_3;
    // Body
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    bufp->fullSData(oldp+1,(vlSelfRef.tpu_top_tb__DOT__weights_layer1[0]),16);
    bufp->fullSData(oldp+2,(vlSelfRef.tpu_top_tb__DOT__weights_layer1[1]),16);
    bufp->fullSData(oldp+3,(vlSelfRef.tpu_top_tb__DOT__weights_layer1[2]),16);
    bufp->fullSData(oldp+4,(vlSelfRef.tpu_top_tb__DOT__weights_layer1[3]),16);
    bufp->fullSData(oldp+5,(vlSelfRef.tpu_top_tb__DOT__weights_layer2[0]),16);
    bufp->fullSData(oldp+6,(vlSelfRef.tpu_top_tb__DOT__weights_layer2[1]),16);
    bufp->fullSData(oldp+7,(vlSelfRef.tpu_top_tb__DOT__weights_layer2[2]),16);
    bufp->fullSData(oldp+8,(vlSelfRef.tpu_top_tb__DOT__weights_layer2[3]),16);
    bufp->fullSData(oldp+9,(vlSelfRef.tpu_top_tb__DOT__activations[0]),16);
    bufp->fullSData(oldp+10,(vlSelfRef.tpu_top_tb__DOT__activations[1]),16);
    bufp->fullSData(oldp+11,(vlSelfRef.tpu_top_tb__DOT__activations[2]),16);
    bufp->fullSData(oldp+12,(vlSelfRef.tpu_top_tb__DOT__activations[3]),16);
    bufp->fullIData(oldp+13,(vlSelfRef.tpu_top_tb__DOT__instr_memory
                             [(0x0000001fU & (IData)(vlSelfRef.tpu_top_tb__DOT__instr_addr_out))]),32);
    bufp->fullIData(oldp+14,(vlSelfRef.tpu_top_tb__DOT__instr_memory[0]),32);
    bufp->fullIData(oldp+15,(vlSelfRef.tpu_top_tb__DOT__instr_memory[1]),32);
    bufp->fullIData(oldp+16,(vlSelfRef.tpu_top_tb__DOT__instr_memory[2]),32);
    bufp->fullIData(oldp+17,(vlSelfRef.tpu_top_tb__DOT__instr_memory[3]),32);
    bufp->fullIData(oldp+18,(vlSelfRef.tpu_top_tb__DOT__instr_memory[4]),32);
    bufp->fullIData(oldp+19,(vlSelfRef.tpu_top_tb__DOT__instr_memory[5]),32);
    bufp->fullIData(oldp+20,(vlSelfRef.tpu_top_tb__DOT__instr_memory[6]),32);
    bufp->fullIData(oldp+21,(vlSelfRef.tpu_top_tb__DOT__instr_memory[7]),32);
    bufp->fullIData(oldp+22,(vlSelfRef.tpu_top_tb__DOT__instr_memory[8]),32);
    bufp->fullIData(oldp+23,(vlSelfRef.tpu_top_tb__DOT__instr_memory[9]),32);
    bufp->fullIData(oldp+24,(vlSelfRef.tpu_top_tb__DOT__instr_memory[10]),32);
    bufp->fullIData(oldp+25,(vlSelfRef.tpu_top_tb__DOT__instr_memory[11]),32);
    bufp->fullIData(oldp+26,(vlSelfRef.tpu_top_tb__DOT__instr_memory[12]),32);
    bufp->fullIData(oldp+27,(vlSelfRef.tpu_top_tb__DOT__instr_memory[13]),32);
    bufp->fullIData(oldp+28,(vlSelfRef.tpu_top_tb__DOT__instr_memory[14]),32);
    bufp->fullIData(oldp+29,(vlSelfRef.tpu_top_tb__DOT__instr_memory[15]),32);
    bufp->fullIData(oldp+30,(vlSelfRef.tpu_top_tb__DOT__instr_memory[16]),32);
    bufp->fullIData(oldp+31,(vlSelfRef.tpu_top_tb__DOT__instr_memory[17]),32);
    bufp->fullIData(oldp+32,(vlSelfRef.tpu_top_tb__DOT__instr_memory[18]),32);
    bufp->fullIData(oldp+33,(vlSelfRef.tpu_top_tb__DOT__instr_memory[19]),32);
    bufp->fullIData(oldp+34,(vlSelfRef.tpu_top_tb__DOT__instr_memory[20]),32);
    bufp->fullIData(oldp+35,(vlSelfRef.tpu_top_tb__DOT__instr_memory[21]),32);
    bufp->fullIData(oldp+36,(vlSelfRef.tpu_top_tb__DOT__instr_memory[22]),32);
    bufp->fullIData(oldp+37,(vlSelfRef.tpu_top_tb__DOT__instr_memory[23]),32);
    bufp->fullIData(oldp+38,(vlSelfRef.tpu_top_tb__DOT__instr_memory[24]),32);
    bufp->fullIData(oldp+39,(vlSelfRef.tpu_top_tb__DOT__instr_memory[25]),32);
    bufp->fullIData(oldp+40,(vlSelfRef.tpu_top_tb__DOT__instr_memory[26]),32);
    bufp->fullIData(oldp+41,(vlSelfRef.tpu_top_tb__DOT__instr_memory[27]),32);
    bufp->fullIData(oldp+42,(vlSelfRef.tpu_top_tb__DOT__instr_memory[28]),32);
    bufp->fullIData(oldp+43,(vlSelfRef.tpu_top_tb__DOT__instr_memory[29]),32);
    bufp->fullIData(oldp+44,(vlSelfRef.tpu_top_tb__DOT__instr_memory[30]),32);
    bufp->fullIData(oldp+45,(vlSelfRef.tpu_top_tb__DOT__instr_memory[31]),32);
    bufp->fullIData(oldp+46,(vlSelfRef.tpu_top_tb__DOT__unnamedblk1__DOT__i),32);
    bufp->fullBit(oldp+47,(vlSelfRef.tpu_top_tb__DOT__rst_n));
    bufp->fullBit(oldp+48,(vlSelfRef.tpu_top_tb__DOT__dma_start_in));
    bufp->fullBit(oldp+49,(vlSelfRef.tpu_top_tb__DOT__dma_dir_in));
    bufp->fullCData(oldp+50,(vlSelfRef.tpu_top_tb__DOT__dma_ub_addr_in),8);
    bufp->fullSData(oldp+51,(vlSelfRef.tpu_top_tb__DOT__dma_length_in),16);
    bufp->fullCData(oldp+52,(vlSelfRef.tpu_top_tb__DOT__dma_elem_sz_in),2);
    bufp->fullWData(oldp+53,(vlSelfRef.tpu_top_tb__DOT__dma_data_in),256);
    bufp->fullIData(oldp+61,(vlSelfRef.tpu_top_tb__DOT__unnamedblk2__DOT__i),32);
    bufp->fullIData(oldp+62,(vlSelfRef.tpu_top_tb__DOT__unnamedblk3__DOT__i),32);
    bufp->fullSData(oldp+63,(vlSelfRef.tpu_top_tb__DOT__unnamedblk4__DOT__cycle_count),10);
    bufp->fullIData(oldp+64,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [0U][0U]),32);
    bufp->fullIData(oldp+65,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [0U][1U]),32);
    bufp->fullIData(oldp+66,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [0U][2U]),32);
    bufp->fullIData(oldp+67,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [1U][0U]),32);
    bufp->fullIData(oldp+68,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [1U][1U]),32);
    bufp->fullIData(oldp+69,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [1U][2U]),32);
    bufp->fullIData(oldp+70,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [2U][0U]),32);
    bufp->fullIData(oldp+71,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [2U][1U]),32);
    bufp->fullIData(oldp+72,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [2U][2U]),32);
    bufp->fullIData(oldp+73,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [0U][0U]),32);
    bufp->fullIData(oldp+74,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [0U][1U]),32);
    bufp->fullIData(oldp+75,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [0U][2U]),32);
    bufp->fullIData(oldp+76,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [1U][0U]),32);
    bufp->fullIData(oldp+77,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [1U][1U]),32);
    bufp->fullIData(oldp+78,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [1U][2U]),32);
    bufp->fullIData(oldp+79,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [2U][0U]),32);
    bufp->fullIData(oldp+80,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [2U][1U]),32);
    bufp->fullIData(oldp+81,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_in
                             [2U][2U]),32);
    bufp->fullBit(oldp+82,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_start));
    bufp->fullCData(oldp+83,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__sys_rows),8);
    bufp->fullCData(oldp+84,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_addr),8);
    bufp->fullBit(oldp+85,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_fifo_wr));
    bufp->fullBit(oldp+86,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_start));
    bufp->fullCData(oldp+87,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__vpu_mode),4);
    bufp->fullSData(oldp+88,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data),16);
    bufp->fullWData(oldp+89,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_data),256);
    bufp->fullIData(oldp+97,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__wt_fifo_data),24);
    bufp->fullWData(oldp+98,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data),256);
    bufp->fullBit(oldp+106,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_tx));
    bufp->fullCData(oldp+107,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_state),8);
    bufp->fullCData(oldp+108,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_cmd),8);
    bufp->fullSData(oldp+109,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_debug_byte_count),16);
    bufp->fullBit(oldp+110,((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))));
    bufp->fullBit(oldp+111,((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state))));
    bufp->fullBit(oldp+112,(((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__use_uart_dma) 
                             & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en))));
    bufp->fullBit(oldp+113,(((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__use_uart_dma) 
                             & (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en))));
    bufp->fullBit(oldp+114,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_en));
    bufp->fullCData(oldp+115,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_addr),8);
    bufp->fullWData(oldp+116,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_wr_data),256);
    bufp->fullBit(oldp+124,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_en));
    bufp->fullCData(oldp+125,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_ub_rd_addr),8);
    bufp->fullBit(oldp+126,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_en));
    bufp->fullSData(oldp+127,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_addr),10);
    bufp->fullQData(oldp+128,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_wt_wr_data),64);
    bufp->fullBit(oldp+130,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_en));
    bufp->fullCData(oldp+131,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_addr),5);
    bufp->fullIData(oldp+132,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_instr_wr_data),32);
    bufp->fullBit(oldp+133,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_start_execution));
    bufp->fullBit(oldp+134,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__use_uart_dma));
    bufp->fullCData(oldp+135,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__pc_reg),8);
    bufp->fullIData(oldp+136,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg),32);
    bufp->fullCData(oldp+137,((vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                               >> 0x0000001aU)),6);
    bufp->fullCData(oldp+138,((0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                                              >> 0x00000012U))),8);
    bufp->fullCData(oldp+139,((0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                                              >> 0x0000000aU))),8);
    bufp->fullCData(oldp+140,((0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg 
                                              >> 2U))),8);
    bufp->fullCData(oldp+141,((3U & vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__ir_reg)),2);
    bufp->fullCData(oldp+142,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_pc),8);
    bufp->fullCData(oldp+143,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_opcode),6);
    bufp->fullCData(oldp+144,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg1),8);
    bufp->fullCData(oldp+145,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg2),8);
    bufp->fullCData(oldp+146,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_arg3),8);
    bufp->fullCData(oldp+147,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_flags),2);
    bufp->fullCData(oldp+148,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_unit_sel),4);
    bufp->fullCData(oldp+149,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_opcode),6);
    bufp->fullCData(oldp+150,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg1),8);
    bufp->fullCData(oldp+151,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg2),8);
    bufp->fullCData(oldp+152,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_arg3),8);
    bufp->fullCData(oldp+153,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_flags),2);
    bufp->fullBit(oldp+154,((4U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))));
    bufp->fullBit(oldp+155,((3U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state))));
    bufp->fullIData(oldp+156,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data),24);
    bufp->fullIData(oldp+157,((0x0000ffffU & vlSelfRef.tpu_top_tb__DOT__dut__DOT__ub_rd_data[0U])),24);
    bufp->fullWData(oldp+158,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out),96);
    __Vtemp_1[0U] = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
        [0U][1U];
    __Vtemp_1[1U] = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                              [2U][1U])) 
                              << 0x00000020U) | (QData)((IData)(
                                                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                                [1U]
                                                                [1U]))));
    __Vtemp_1[2U] = (IData)(((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                               [2U]
                                               [1U])) 
                               << 0x00000020U) | (QData)((IData)(
                                                                 vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                                 [1U]
                                                                 [1U]))) 
                             >> 0x00000020U));
    bufp->fullWData(oldp+161,(__Vtemp_1),96);
    __Vtemp_2[0U] = vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
        [0U][2U];
    __Vtemp_2[1U] = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                              [2U][2U])) 
                              << 0x00000020U) | (QData)((IData)(
                                                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                                [1U]
                                                                [2U]))));
    __Vtemp_2[2U] = (IData)(((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                               [2U]
                                               [2U])) 
                               << 0x00000020U) | (QData)((IData)(
                                                                 vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                                                                 [1U]
                                                                 [2U]))) 
                             >> 0x00000020U));
    bufp->fullWData(oldp+164,(__Vtemp_2),96);
    bufp->fullBit(oldp+167,((1U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))));
    bufp->fullBit(oldp+168,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col0));
    bufp->fullBit(oldp+169,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col1));
    bufp->fullBit(oldp+170,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__en_capture_col2));
    bufp->fullBit(oldp+171,((2U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state))));
    bufp->fullQData(oldp+172,((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U])) 
                                << 0x00000020U) | (QData)((IData)(
                                                                  vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U])))),64);
    bufp->fullQData(oldp+174,((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[1U])) 
                                << 0x00000020U) | (QData)((IData)(
                                                                  vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data[0U])))),64);
    bufp->fullWData(oldp+176,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_data),256);
    bufp->fullBit(oldp+184,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_out_valid));
    bufp->fullBit(oldp+185,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_a_valid));
    bufp->fullBit(oldp+186,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub_port_b_ready));
    bufp->fullBit(oldp+187,(((0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state)) 
                             | (0U != (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state)))));
    bufp->fullBit(oldp+188,(((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state)) 
                             & ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state)) 
                                & ((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count)) 
                                   & (0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count)))))));
    bufp->fullBit(oldp+189,(((0x0010U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count)) 
                             | (0x0010U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count)))));
    __Vtemp_3[0U] = (IData)((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U])) 
                              << 0x00000020U) | (QData)((IData)(
                                                                vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U]))));
    __Vtemp_3[1U] = (IData)(((((QData)((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[1U])) 
                               << 0x00000020U) | (QData)((IData)(
                                                                 vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__psum_col0_out[0U]))) 
                             >> 0x00000020U));
    __Vtemp_3[2U] = 0U;
    bufp->fullWData(oldp+190,(__Vtemp_3),96);
    bufp->fullWData(oldp+193,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__accumulators__DOT__selected_rd_data),96);
    bufp->fullCData(oldp+196,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [0U][0U]),8);
    bufp->fullCData(oldp+197,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [0U][1U]),8);
    bufp->fullCData(oldp+198,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [0U][2U]),8);
    bufp->fullCData(oldp+199,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [1U][0U]),8);
    bufp->fullCData(oldp+200,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [1U][1U]),8);
    bufp->fullCData(oldp+201,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [1U][2U]),8);
    bufp->fullCData(oldp+202,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [2U][0U]),8);
    bufp->fullCData(oldp+203,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [2U][1U]),8);
    bufp->fullCData(oldp+204,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [2U][2U]),8);
    bufp->fullCData(oldp+205,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                              [0U][0U]),8);
    bufp->fullCData(oldp+206,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                              [0U][1U]),8);
    bufp->fullCData(oldp+207,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                              [0U][2U]),8);
    bufp->fullCData(oldp+208,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                              [1U][0U]),8);
    bufp->fullCData(oldp+209,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                              [1U][1U]),8);
    bufp->fullCData(oldp+210,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                              [1U][2U]),8);
    bufp->fullCData(oldp+211,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                              [2U][0U]),8);
    bufp->fullCData(oldp+212,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                              [2U][1U]),8);
    bufp->fullCData(oldp+213,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_out
                              [2U][2U]),8);
    bufp->fullIData(oldp+214,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                              [0U][0U]),32);
    bufp->fullIData(oldp+215,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                              [0U][1U]),32);
    bufp->fullIData(oldp+216,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                              [0U][2U]),32);
    bufp->fullIData(oldp+217,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                              [1U][0U]),32);
    bufp->fullIData(oldp+218,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                              [1U][1U]),32);
    bufp->fullIData(oldp+219,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                              [1U][2U]),32);
    bufp->fullIData(oldp+220,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                              [2U][0U]),32);
    bufp->fullIData(oldp+221,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                              [2U][1U]),32);
    bufp->fullIData(oldp+222,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_psum_out
                              [2U][2U]),32);
    bufp->fullCData(oldp+223,((0x000000ffU & vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data)),8);
    bufp->fullCData(oldp+224,((0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
                                              >> 8U))),8);
    bufp->fullCData(oldp+225,((0x000000ffU & (vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__selected_rd_data 
                                              >> 0x00000010U))),8);
    bufp->fullCData(oldp+226,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row1_delayed),8);
    bufp->fullCData(oldp+227,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__act_row2_delayed),8);
    bufp->fullCData(oldp+228,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [0U][0U]),8);
    bufp->fullCData(oldp+229,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_00__act_out),8);
    bufp->fullIData(oldp+230,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mac_result),32);
    bufp->fullCData(oldp+231,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__weight_reg),8);
    bufp->fullSData(oldp+232,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_00__DOT__mult_result),16);
    bufp->fullCData(oldp+233,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [0U][1U]),8);
    bufp->fullCData(oldp+234,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_01__act_out),8);
    bufp->fullIData(oldp+235,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mac_result),32);
    bufp->fullCData(oldp+236,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__weight_reg),8);
    bufp->fullSData(oldp+237,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_01__DOT__mult_result),16);
    bufp->fullCData(oldp+238,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [0U][2U]),8);
    bufp->fullCData(oldp+239,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_02__act_out),8);
    bufp->fullIData(oldp+240,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mac_result),32);
    bufp->fullCData(oldp+241,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__weight_reg),8);
    bufp->fullSData(oldp+242,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_02__DOT__mult_result),16);
    bufp->fullCData(oldp+243,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [1U][0U]),8);
    bufp->fullCData(oldp+244,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_10__act_out),8);
    bufp->fullIData(oldp+245,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mac_result),32);
    bufp->fullCData(oldp+246,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__weight_reg),8);
    bufp->fullSData(oldp+247,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_10__DOT__mult_result),16);
    bufp->fullCData(oldp+248,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [1U][1U]),8);
    bufp->fullCData(oldp+249,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_11__act_out),8);
    bufp->fullIData(oldp+250,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mac_result),32);
    bufp->fullCData(oldp+251,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__weight_reg),8);
    bufp->fullSData(oldp+252,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_11__DOT__mult_result),16);
    bufp->fullCData(oldp+253,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [1U][2U]),8);
    bufp->fullCData(oldp+254,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_12__act_out),8);
    bufp->fullIData(oldp+255,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mac_result),32);
    bufp->fullCData(oldp+256,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__weight_reg),8);
    bufp->fullSData(oldp+257,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_12__DOT__mult_result),16);
    bufp->fullCData(oldp+258,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [2U][0U]),8);
    bufp->fullCData(oldp+259,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_20__act_out),8);
    bufp->fullIData(oldp+260,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mac_result),32);
    bufp->fullCData(oldp+261,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__weight_reg),8);
    bufp->fullSData(oldp+262,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_20__DOT__mult_result),16);
    bufp->fullCData(oldp+263,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [2U][1U]),8);
    bufp->fullCData(oldp+264,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_21__act_out),8);
    bufp->fullIData(oldp+265,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mac_result),32);
    bufp->fullCData(oldp+266,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__weight_reg),8);
    bufp->fullSData(oldp+267,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_21__DOT__mult_result),16);
    bufp->fullCData(oldp+268,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_act_in
                              [2U][2U]),8);
    bufp->fullCData(oldp+269,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT____Vcellout__pe_22__act_out),8);
    bufp->fullIData(oldp+270,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mac_result),32);
    bufp->fullCData(oldp+271,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__weight_reg),8);
    bufp->fullSData(oldp+272,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_array__DOT__pe_22__DOT__mult_result),16);
    bufp->fullCData(oldp+273,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__current_state),3);
    bufp->fullCData(oldp+274,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__weight_load_counter),8);
    bufp->fullCData(oldp+275,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__compute_counter),8);
    bufp->fullCData(oldp+276,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__total_rows),8);
    bufp->fullCData(oldp+277,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_state),2);
    bufp->fullCData(oldp+278,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_burst_count),8);
    bufp->fullCData(oldp+279,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_a_current_addr),8);
    bufp->fullCData(oldp+280,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_state),2);
    bufp->fullCData(oldp+281,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_burst_count),8);
    bufp->fullCData(oldp+282,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__ub__DOT__port_b_current_addr),8);
    bufp->fullCData(oldp+283,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__current_state),2);
    bufp->fullCData(oldp+284,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__process_counter),8);
    bufp->fullIData(oldp+285,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[0]),32);
    bufp->fullIData(oldp+286,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[1]),32);
    bufp->fullIData(oldp+287,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[2]),32);
    bufp->fullIData(oldp+288,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[3]),32);
    bufp->fullIData(oldp+289,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[4]),32);
    bufp->fullIData(oldp+290,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[5]),32);
    bufp->fullIData(oldp+291,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[6]),32);
    bufp->fullIData(oldp+292,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__data_buffer[7]),32);
    bufp->fullIData(oldp+293,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result0),32);
    bufp->fullIData(oldp+294,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__vpu_inst__DOT__relu_result1),32);
    bufp->fullSData(oldp+295,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_ptr),12);
    bufp->fullSData(oldp+296,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_ptr),12);
    bufp->fullSData(oldp+297,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count),13);
    bufp->fullSData(oldp+298,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_count),13);
    bufp->fullSData(oldp+299,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_ptr),12);
    bufp->fullSData(oldp+300,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_ptr),12);
    bufp->fullSData(oldp+301,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count),13);
    bufp->fullSData(oldp+302,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_count),13);
    bufp->fullCData(oldp+303,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_data),8);
    bufp->fullBit(oldp+304,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__rx_valid));
    bufp->fullBit(oldp+305,((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state))));
    bufp->fullCData(oldp+306,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_data),8);
    bufp->fullBit(oldp+307,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__tx_valid));
    bufp->fullBit(oldp+308,((0U == (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state))));
    bufp->fullCData(oldp+309,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__state),8);
    bufp->fullCData(oldp+310,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__command),8);
    bufp->fullCData(oldp+311,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_hi),8);
    bufp->fullCData(oldp+312,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__addr_lo),8);
    bufp->fullSData(oldp+313,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__length),16);
    bufp->fullSData(oldp+314,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_count),16);
    bufp->fullCData(oldp+315,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__byte_index),5);
    bufp->fullWData(oldp+316,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__ub_buffer),256);
    bufp->fullQData(oldp+324,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__wt_buffer),64);
    bufp->fullIData(oldp+326,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__instr_buffer),32);
    bufp->fullWData(oldp+327,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_buffer),256);
    bufp->fullCData(oldp+335,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__read_index),8);
    bufp->fullCData(oldp+336,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__state),2);
    bufp->fullSData(oldp+337,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__clk_count),16);
    bufp->fullCData(oldp+338,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__bit_index),3);
    bufp->fullCData(oldp+339,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_byte),8);
    bufp->fullCData(oldp+340,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__state),2);
    bufp->fullSData(oldp+341,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__clk_count),16);
    bufp->fullCData(oldp+342,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__bit_index),3);
    bufp->fullCData(oldp+343,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_tx_inst__DOT__tx_byte),8);
    bufp->fullBit(oldp+344,(vlSelfRef.tpu_top_tb__DOT__tpu_busy));
    bufp->fullBit(oldp+345,((1U & (~ (IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)))));
    bufp->fullCData(oldp+346,(((IData)(vlSelfRef.tpu_top_tb__DOT__tpu_busy)
                                ? 0U : ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid)
                                         ? 2U : 1U))),2);
    bufp->fullBit(oldp+347,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__if_id_valid));
    bufp->fullBit(oldp+348,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__controller__DOT__exec_valid));
    bufp->fullCData(oldp+349,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__systolic_ctrl__DOT__next_state),3);
    bufp->fullBit(oldp+350,(vlSelfRef.tpu_top_tb__DOT__clk));
    bufp->fullBit(oldp+351,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel));
    bufp->fullBit(oldp+352,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__acc_buf_sel));
    bufp->fullBit(oldp+353,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_start));
    bufp->fullBit(oldp+354,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_dir));
    bufp->fullCData(oldp+355,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_ub_addr),8);
    bufp->fullSData(oldp+356,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_length),16);
    bufp->fullCData(oldp+357,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__dma_elem_sz),2);
    bufp->fullBit(oldp+358,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_en));
    bufp->fullBit(oldp+359,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__rd_empty));
    bufp->fullBit(oldp+360,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__wr_full));
    bufp->fullBit(oldp+361,(((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel)
                              ? (0x0ffcU <= (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count))
                              : (0x0ffcU <= (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count)))));
    bufp->fullBit(oldp+362,(((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__wt_buf_sel)
                              ? (4U >= ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_wr_count) 
                                        - (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf1_rd_count)))
                              : (4U >= ((IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_wr_count) 
                                        - (IData)(vlSelfRef.tpu_top_tb__DOT__dut__DOT__datapath__DOT__weight_fifo_inst__DOT__buf0_rd_count))))));
    bufp->fullBit(oldp+363,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_1));
    bufp->fullBit(oldp+364,(vlSelfRef.tpu_top_tb__DOT__dut__DOT__uart_dma__DOT__uart_rx_inst__DOT__rx_sync_2));
}
