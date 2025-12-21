// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtpu_top_tb__pch.h"
#include "verilated_vcd_c.h"

//============================================================
// Constructors

Vtpu_top_tb::Vtpu_top_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtpu_top_tb__Syms(contextp(), _vcname__, this)}
    , __PVT____024unit{vlSymsp->TOP.__PVT____024unit}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
    contextp()->traceBaseModelCbAdd(
        [this](VerilatedTraceBaseC* tfp, int levels, int options) { traceBaseModel(tfp, levels, options); });
}

Vtpu_top_tb::Vtpu_top_tb(const char* _vcname__)
    : Vtpu_top_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtpu_top_tb::~Vtpu_top_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtpu_top_tb___024root___eval_debug_assertions(Vtpu_top_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vtpu_top_tb___024root___eval_static(Vtpu_top_tb___024root* vlSelf);
void Vtpu_top_tb___024root___eval_initial(Vtpu_top_tb___024root* vlSelf);
void Vtpu_top_tb___024root___eval_settle(Vtpu_top_tb___024root* vlSelf);
void Vtpu_top_tb___024root___eval(Vtpu_top_tb___024root* vlSelf);

void Vtpu_top_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtpu_top_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtpu_top_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_activity = true;
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtpu_top_tb___024root___eval_static(&(vlSymsp->TOP));
        Vtpu_top_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vtpu_top_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtpu_top_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtpu_top_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vtpu_top_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtpu_top_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtpu_top_tb___024root___eval_final(Vtpu_top_tb___024root* vlSelf);

VL_ATTR_COLD void Vtpu_top_tb::final() {
    Vtpu_top_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtpu_top_tb::hierName() const { return vlSymsp->name(); }
const char* Vtpu_top_tb::modelName() const { return "Vtpu_top_tb"; }
unsigned Vtpu_top_tb::threads() const { return 1; }
void Vtpu_top_tb::prepareClone() const { contextp()->prepareClone(); }
void Vtpu_top_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
std::unique_ptr<VerilatedTraceConfig> Vtpu_top_tb::traceConfig() const {
    return std::unique_ptr<VerilatedTraceConfig>{new VerilatedTraceConfig{false, false, false}};
};

//============================================================
// Trace configuration

void Vtpu_top_tb___024root__trace_decl_types(VerilatedVcd* tracep);

void Vtpu_top_tb___024root__trace_init_top(Vtpu_top_tb___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD static void trace_init(void* voidSelf, VerilatedVcd* tracep, uint32_t code) {
    // Callback from tracep->open()
    Vtpu_top_tb___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtpu_top_tb___024root*>(voidSelf);
    Vtpu_top_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (!vlSymsp->_vm_contextp__->calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
            "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->pushPrefix(std::string{vlSymsp->name()}, VerilatedTracePrefixType::SCOPE_MODULE);
    Vtpu_top_tb___024root__trace_decl_types(tracep);
    Vtpu_top_tb___024root__trace_init_top(vlSelf, tracep);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vtpu_top_tb___024root__trace_register(Vtpu_top_tb___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD void Vtpu_top_tb::traceBaseModel(VerilatedTraceBaseC* tfp, int levels, int options) {
    (void)levels; (void)options;
    VerilatedVcdC* const stfp = dynamic_cast<VerilatedVcdC*>(tfp);
    if (VL_UNLIKELY(!stfp)) {
        vl_fatal(__FILE__, __LINE__, __FILE__,"'Vtpu_top_tb::trace()' called on non-VerilatedVcdC object;"
            " use --trace-fst with VerilatedFst object, and --trace-vcd with VerilatedVcd object");
    }
    stfp->spTrace()->addModel(this);
    stfp->spTrace()->addInitCb(&trace_init, &(vlSymsp->TOP));
    Vtpu_top_tb___024root__trace_register(&(vlSymsp->TOP), stfp->spTrace());
}
