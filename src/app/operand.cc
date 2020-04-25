#include "operand.hh"

ImmediateOperand::ImmediateOperand(uint64_t v): value(v), size(0) {}

uint64_t ImmediateOperand::get_value() const {
    return value;
}

uint8_t ImmediateOperand::get_size() const {
    if (size) return size;

    if (value >> 8 == 0)  return 8;
    if (value >> 16 == 0) return 16;
    if (value >> 32 == 0) return 32;
    return 64;
}

void ImmediateOperand::set_size(uint8_t s) {
    auto old_size = size;
    size = 0;
    if (get_size() > s) size = old_size;
    else size = s;
}


std::string ImmediateOperand::to_string() const {
    return std::to_string(value);
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

void MemoryOperand::set_size(uint8_t s) {
    size = s;
}

void MemoryOperand::set_segment(register_code c) {
    segment = c;
    use_segment = true;
}

uint8_t MemoryOperand::get_size() const {
    return size;
}

std::string MemoryOperand::to_string() const {

    std::string str;

    if (size == 8)  str += "byte";
    if (size == 16) str += "word";
    if (size == 32) str += "dword";
    if (size == 64) str += "qword";


    str += " ";

    if (use_segment) str += Register::to_string(segment) + ":";
    str += "[";
    if (use_base) str += Register::to_string(base);
    if (use_index) {
        if (use_base) str += " + ";
        str += Register::to_string(index) + " * " + std::to_string(scale);
    }

    if (displacement != 0) {
        if (use_index || use_base) str += " + ";
        str += std::to_string(displacement);
    }

    str += "]";

    return str;
}

RegisterOperand::RegisterOperand(register_code r): reg(r) {}

register_code RegisterOperand::get_reg() const {
    return reg;
}

uint8_t RegisterOperand::get_size() const {
    return Register::register_size(reg);
}

std::string RegisterOperand::to_string() const {
    return Register::to_string(reg);
}
