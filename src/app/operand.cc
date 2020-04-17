#include "operand.hh"

ImmediateOperand::ImmediateOperand(uint64_t v): value(v) {}

uint64_t ImmediateOperand::get_value() const {
    return value;
}

MemoryOperand::MemoryOperand(register_code b, register_code i, uint8_t s, uint64_t d, uint8_t si, register_code seg):
    base(b), index(i), scale(s), displacement(d), segment(seg),
    use_base(true), use_index(true), use_segment(true),
    size(si) {}

MemoryOperand::MemoryOperand(register_code i, uint8_t s, uint64_t d, uint8_t si, register_code seg):
    index(i), scale(s), displacement(d), segment(seg),
    use_base(false), use_index(true), use_segment(true),
    size(si) {}

MemoryOperand::MemoryOperand(register_code b, uint64_t d, uint8_t si, register_code seg):
    base(b), displacement(d), segment(seg),
    use_base(true), use_index(false), use_segment(true),
    size(si) {}

MemoryOperand::MemoryOperand(uint64_t d, uint8_t si, register_code seg):
    displacement(d), segment(seg),
    use_base(false), use_index(false), use_segment(true),
    size(si) {}

MemoryOperand::MemoryOperand(register_code b, register_code i, uint8_t s, uint64_t d, uint8_t si):
    base(b), index(i), scale(s), displacement(d),
    use_base(true), use_index(true), use_segment(false),
    size(si) {}

MemoryOperand::MemoryOperand(register_code i, uint8_t s, uint64_t d, uint8_t si):
    index(i), scale(s), displacement(d),
    use_base(false), use_index(true), use_segment(false),
    size(si) {}

MemoryOperand::MemoryOperand(register_code b, uint64_t d, uint8_t si):
    base(b), displacement(d),
    use_base(true), use_index(false), use_segment(false),
    size(si) {}

MemoryOperand::MemoryOperand(uint64_t d, uint8_t si):
    displacement(d),
    use_base(false), use_index(false), use_segment(false),
    size(si) {}

register_code MemoryOperand::get_segment() const {
    return segment;
}

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

bool MemoryOperand::get_use_segment() const {
    return use_segment;
}

bool MemoryOperand::get_use_base() const {
    return use_base;
}

bool MemoryOperand::get_use_index() const {
    return use_index;
}

uint8_t MemoryOperand::get_size() const {
    return size;
}

RegisterOperand::RegisterOperand(register_code r): reg(r) {}

register_code RegisterOperand::get_reg() const {
    return reg;
}
