// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VSIMPLE_TEST__SYMS_H_
#define VERILATED_VSIMPLE_TEST__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vsimple_test.h"

// INCLUDE MODULE CLASSES
#include "Vsimple_test___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vsimple_test__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vsimple_test* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vsimple_test___024root         TOP;

    // CONSTRUCTORS
    Vsimple_test__Syms(VerilatedContext* contextp, const char* namep, Vsimple_test* modelp);
    ~Vsimple_test__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
