#include "register.hh"

std::string Register::to_string(register_code code) {
    std::string names[] = {
        "mar", "mdr", "cs", "ds", "ss",
        "rax", "rbx", "rcx", "rdx",
        "eax", "ebx", "ecx", "edx",
        "ax", "bx", "cx", "dx",
        "al", "bl", "cl", "dl",
        "ah", "bh", "ch", "dh",
        "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
        "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d",
        "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w",
        "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b",
        "rdi", "rsi",
        "edi", "esi",
        "di", "si",
        "rsp", "esp", "sp",
        "bsp", "ebp", "bp",
        "rflags", "eflags", "flags",
        "st0", "st1", "st2", "st3",
        "st4", "st5", "st6", "st7",
        "rip", "ir", "_immediate",
        "xmm0", "xmm1", "xmm2", "xmm3",
        "xmm4", "xmm5", "xmm6", "xmm7",
        "_temp_sse"
    };

    return names[code];
}

uint8_t Register::register_size(register_code code) {
    uint8_t sizes[] = {
        64, 64, 16, 16, 16,
        64, 64, 64, 64,
        32, 32, 32, 32,
        16, 16, 16, 16,
        8, 8, 8, 8,
        8, 8, 8, 8,
        64, 32, 16,
        64, 32, 16,

        64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64,

        128, 128, 128, 128, 128, 128, 128, 128
    };

    return sizes[code];
}

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
