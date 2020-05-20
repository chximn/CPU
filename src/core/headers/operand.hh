#pragma once
#include <cstdint>
#include <iostream>
#include <memory>

#include "helpers.hh"
#include "register.hh"

// abstract class
class Operand {
public:
    virtual ~Operand() = default;
    virtual uint8_t get_size() const = 0;
    virtual std::string to_string() const { return "hello"; }
};

using operand_ptr = std::shared_ptr<Operand>;

class ImmediateOperand : public Operand {
private:
    uint64_t value;
    uint8_t size;

public:
    ImmediateOperand() = delete;
    ImmediateOperand(uint64_t);

    uint64_t get_value() const;

    uint8_t get_size() const override;
    void set_size(uint8_t);

    std::string to_string() const override;
};

class MemoryOperand : public Operand {
private:
    register_code base;
    register_code index;
    uint8_t scale;
    uint64_t displacement;
    register_code segment;

    bool use_base;
    bool use_index;
    bool use_segment;

    operand_ptr object_displacement;
    bool use_object_displacement;

    uint8_t size;

public:
    MemoryOperand() = delete;
    MemoryOperand(register_code,
                  register_code,
                  uint8_t,
                  uint64_t,
                  uint8_t,
                  register_code);
    MemoryOperand(register_code, uint8_t, uint64_t, uint8_t, register_code);
    MemoryOperand(register_code, uint64_t, uint8_t, register_code);
    MemoryOperand(uint64_t, uint8_t, register_code);
    MemoryOperand(register_code, register_code, uint8_t, uint64_t, uint8_t);
    MemoryOperand(register_code, uint8_t, uint64_t, uint8_t);
    MemoryOperand(register_code, uint64_t, uint8_t);
    MemoryOperand(uint64_t, uint8_t);

    register_code get_segment() const;
    register_code get_base() const;
    register_code get_index() const;
    uint8_t get_scale() const;
    uint64_t get_displacement() const;

    bool get_use_segment() const;
    bool get_use_base() const;
    bool get_use_index() const;

    uint8_t get_size() const override;

    void set_size(uint8_t);
    void set_segment(register_code);

    void set_object_displacement(operand_ptr);
    void set_displacement(uint64_t);
    operand_ptr get_object_displacement();
    bool get_use_object_displacement();

    std::string to_string() const override;
};

class RegisterOperand : public Operand {
private:
    register_code reg;

public:
    RegisterOperand() = delete;
    RegisterOperand(register_code);

    register_code get_reg() const;
    uint8_t get_size() const override;
    std::string to_string() const override;
};

class SymbolOperand : public Operand {
private:
    std::string name;
    int line;

public:
    SymbolOperand() = delete;
    SymbolOperand(std::string const &, int);

    std::string get_name() const;
    int get_line() const;
    uint8_t get_size() const override;
    std::string to_string() const override;
};

class LabelOperand : public SymbolOperand {
public:
    LabelOperand() = delete;
    LabelOperand(std::string const &, int);
};

class ObjectOperand : public SymbolOperand {
public:
    ObjectOperand() = delete;
    ObjectOperand(std::string const &, int);
};
