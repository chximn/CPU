#pragma once
#include "helpers.hh"
#include "register.hh"
#include <map>

enum class alu_operation {
    mov, add, sub, div, mul, neg,
    _and, _or, _xor, _not,
    shl, shr,

    cmp
};

enum class flag_code {
    pf, zf, sf, of, cf
};


class ArithemeticLogicUnit {
private:
    register_ptr flags_register;
public:
    ArithemeticLogicUnit() = delete;
    ArithemeticLogicUnit(register_ptr);

    std::map<flag_code, register_ptr> flags;
    uint8_t size;
    register_ptr source;
    register_ptr destination;
    register_ptr extra;

    alu_operation operation;

    void execute();
};
