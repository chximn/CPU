#include "vector_register.hh"

VectorRegister::VectorRegister(register_code c):
    code(c),
    value(aligned_alloc(16, 16)) {}


register_code VectorRegister::get_code() const {
    return code;
}

uint64_t VectorRegister::get_low() {
    return *reinterpret_cast<uint64_t *>(value);
}

void VectorRegister::set_low(uint64_t v) {
    *reinterpret_cast<uint64_t *>(value) = v;
}

uint64_t VectorRegister::get_high() {
    return (reinterpret_cast<uint64_t *>(value))[1];
}

void VectorRegister::set_high(uint64_t v) {
    (reinterpret_cast<uint64_t *>(value))[1] = v;
}

float * VectorRegister::value_float() {
    return reinterpret_cast<float *>(value);
}
