#pragma once
#include "register.hh"
#include <map>

enum class alu_operation {
    mov, add, sub, div, _and, _or, _xor, _not
};

enum class flag_code {
    cf, pf, af, zf, sf, df, of
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
