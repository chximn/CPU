#include "register.hh"

Register::Register(register_code c) : code(c) {}

register_code Register::get_code() const {
    return code;
}

void Register::clear() {
    set_value(0);
}

FullRegister::FullRegister(register_code c) : Register(c) {}

uint64_t FullRegister::get_value() const {
    return value;
}

void FullRegister::set_value(uint64_t v) {
    value = v;
}

void FullRegister::increment() {
    value++;
}

PartRegister::PartRegister(register_code c, register_ptr r, unsigned int s, unsigned int o):
        Register(c), reg(r), size(s), offset(o) {}

uint64_t PartRegister::get_value() const {
    auto value = reg->get_value();
    uint64_t mask = 0;
    mask = ~mask;
    mask = (mask >> (64 - size)) << offset;
    return (value & mask) >> offset;
}

void PartRegister::set_value(uint64_t v) {
    auto old_value = reg->get_value();

    // make sure we only have `size` bits of data in parameter
    uint64_t mask = 0;
    mask = ~mask;
    mask = (mask >> (64 - size));
    v = (v & mask) << offset;

    uint64_t old_mask = ~(mask << offset);
    uint64_t new_value = (old_value & old_mask) | v;

    reg->set_value(new_value);
}

void PartRegister::increment() {
    uint64_t value = get_value();
    value++;
    set_value(value);
}
