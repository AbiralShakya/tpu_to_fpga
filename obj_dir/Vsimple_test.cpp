// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vsimple_test__pch.h"

//============================================================
// Constructors

Vsimple_test::Vsimple_test(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vsimple_test__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vsimple_test::Vsimple_test(const char* _vcname__)
    : Vsimple_test(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vsimple_test::~Vsimple_test() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vsimple_test___024root___eval_debug_assertions(Vsimple_test___024root* vlSelf);
#endif  // VL_DEBUG
void Vsimple_test___024root___eval_static(Vsimple_test___024root* vlSelf);
void Vsimple_test___024root___eval_initial(Vsimple_test___024root* vlSelf);
void Vsimple_test___024root___eval_settle(Vsimple_test___024root* vlSelf);
void Vsimple_test___024root___eval(Vsimple_test___024root* vlSelf);

void Vsimple_test::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vsimple_test::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vsimple_test___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vsimple_test___024root___eval_static(&(vlSymsp->TOP));
        Vsimple_test___024root___eval_initial(&(vlSymsp->TOP));
        Vsimple_test___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vsimple_test___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vsimple_test::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vsimple_test::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vsimple_test::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vsimple_test___024root___eval_final(Vsimple_test___024root* vlSelf);

VL_ATTR_COLD void Vsimple_test::final() {
    Vsimple_test___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vsimple_test::hierName() const { return vlSymsp->name(); }
const char* Vsimple_test::modelName() const { return "Vsimple_test"; }
unsigned Vsimple_test::threads() const { return 1; }
void Vsimple_test::prepareClone() const { contextp()->prepareClone(); }
void Vsimple_test::atClone() const {
    contextp()->threadPoolpOnClone();
}
