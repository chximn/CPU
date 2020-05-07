#pragma once
#include "register.hh"
#include <stdlib.h>
#include <memory>
#include <emmintrin.h>

class VectorRegister {
private:
    register_code code;

public:
    VectorRegister() = delete;
    VectorRegister(register_code);

    void * value;
    register_code get_code() const;

    uint64_t get_low();
    void set_low(uint64_t);

    uint64_t get_high();
    void set_high(uint64_t);

    int * value_int();
    float * value_float();
    double * value_double();
    __m128i_u * value_dq();
};

using vector_register_ptr = std::shared_ptr<VectorRegister>;
