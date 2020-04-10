#pragma once
#include "register.hh"

enum class arithemetic_logic_operation {
    mov, add, sub, div, _and, _or
};

class ArithemeticLogicUnit {
public:
    ArithemeticLogicUnit() = default;

    register_ptr source;
    register_ptr destination;
    register_ptr extra;

    arithemetic_logic_operation operation;

    void execute() const;
};
