#include "operand.hh"

ImmediateOperand::ImmediateOperand(uint64_t v): value(v) {}

uint64_t ImmediateOperand::get_value() const {
    return value;
}

MemoryOperand::MemoryOperand(register_code b, register_code i, uint8_t s, uint64_t d):
    base(b), index(i), scale(s), displacement(d) {}

register_code MemoryOperand::get_base() const {
    return base;
}

register_code MemoryOperand::get_index() const {
    return index;
}

uint8_t MemoryOperand::get_scale() const {
    return scale;
}

uint64_t MemoryOperand::get_displacement() const {
    return displacement;
}

RegisterOperand::RegisterOperand(register_code r): reg(r) {}

register_code RegisterOperand::get_reg() const {
    return reg;
}
