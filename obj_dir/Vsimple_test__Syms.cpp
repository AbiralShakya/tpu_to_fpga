// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vsimple_test__pch.h"
#include "Vsimple_test.h"
#include "Vsimple_test___024root.h"

// FUNCTIONS
Vsimple_test__Syms::~Vsimple_test__Syms()
{
}

Vsimple_test__Syms::Vsimple_test__Syms(VerilatedContext* contextp, const char* namep, Vsimple_test* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(152);
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-9);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
}
