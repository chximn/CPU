#pragma once
#include "register.hh"
#include <memory>

enum class fpu_operation {
    fld, fst,
    fadd, fsub, fmul, fsqrt
};

class FloatingPointUnit {
public:
    register_ptr src_dest;
    bool is_destination;
    bool is_double;

    register_ptr stages[8];
    fpu_operation operation;
    bool perform_pop;

    FloatingPointUnit();

    double convert_float(uint64_t);
    double convert_double(uint64_t);
    uint64_t convert_uint64(double);
    uint64_t convert_uint64(float);

    register_ptr get_register(register_code);

    void push(uint64_t);
    void pop();
    void execute();
};
