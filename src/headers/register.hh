#pragma once
#include <cstdint>
#include <memory>

enum register_code {
    // Memory Registers
    mar, mdr,

    // segment registers
    cs, ds, ss,

    // General Purpose Registers
    rax, rbx, rcx, rdx,
    eax, ebx, ecx, edx,
     ax,  bx,  cx,  dx,
     al,  bl,  cl,  dl,
     ah,  bh,  ch,  dh,

    rsp, esp, sp,
    rbp, ebp, bp,

    // TODO: add r8.. registers

    // Flags
    rflags, eflags, flags,

    // Floating Point Registers
    st0, st1, st2, st3,
    st4, st5, st6, st7,

    // ControlUnit Registers
    rip, ir, _immediate,

    // Vector Registers
    xmm0, xmm1, xmm2, xmm3,
    xmm4, xmm5, xmm6, xmm7,
    _temp_sse
};

class Register {
private:
    register_code code;
public:

    Register() = delete;
    Register(register_code);

    register_code get_code() const;
    void clear();

    virtual uint64_t get_value() const = 0;
    virtual void set_value(uint64_t) = 0;
    virtual void increment() = 0;

    virtual ~Register() = default;

    static std::string to_string(register_code);
    static uint8_t register_size(register_code);
};

using register_ptr = std::shared_ptr<Register>;

class FullRegister : public Register {
private:
    uint64_t value;
public:
    FullRegister() = delete;
    FullRegister(register_code);

    uint64_t get_value() const override;
    void set_value(uint64_t) override;
    void increment() override;
};

class PartRegister : public Register {
private:
    register_ptr reg;
    unsigned int size;
    unsigned int offset;
public:
    PartRegister() = delete;
    PartRegister(register_code, register_ptr, unsigned int, unsigned int);

    uint64_t get_value() const override;
    void set_value(uint64_t) override;
    void increment() override;
};
