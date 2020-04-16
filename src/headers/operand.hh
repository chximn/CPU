#pragma once
#include "register.hh"
#include <cstdint>
#include <memory>

// abstract class
class Operand {
public:
    virtual ~Operand() = default;
};

using operand_ptr = std::shared_ptr<Operand>;

class ImmediateOperand : public Operand {
private:
    uint64_t value;
public:
    ImmediateOperand() = delete;
    ImmediateOperand(uint64_t);

    uint64_t get_value() const;
};

class MemoryOperand : public Operand {
private:
    register_code base;
    register_code index;
    uint8_t scale;
    uint64_t displacement;

    bool use_base;
    bool use_index;
    uint8_t size;

public:
    MemoryOperand() = delete;
    MemoryOperand(register_code, register_code, uint8_t, uint64_t, uint8_t);
    MemoryOperand(register_code, uint8_t, uint64_t, uint8_t);
    MemoryOperand(register_code, uint64_t, uint8_t);
    MemoryOperand(uint64_t, uint8_t);


    register_code get_base() const;
    register_code get_index() const;
    uint8_t get_scale() const;
    uint64_t get_displacement() const;

    bool get_use_base() const;
    bool get_use_index() const;
    uint8_t get_size() const;
};

class RegisterOperand : public Operand {
private:
    register_code reg;

public:
    RegisterOperand() = delete;
    RegisterOperand(register_code);

    register_code get_reg() const;
};
