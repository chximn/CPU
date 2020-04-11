#pragma once
#include "operand.hh"
#include "instruction.hh"
#include "register.hh"
#include "ram.hh"
#include <map>
#include "alu.hh"

class ControlUnit {
private:
    register_ptr instruction_pointer_register;
    register_ptr instruction_register;
    register_ptr immediate_register;

    bool load_from_memory;
    bool write_to_memory;

    std::map<register_code, register_ptr> & registers;
    ArithemeticLogicUnit & alu;
    RandomAccessMemory & ram;

    void evaluate_destination(operand_ptr);
    void evaluate_source(operand_ptr);
public:
    ControlUnit() = delete;
    ControlUnit(std::map<register_code, register_ptr> &, ArithemeticLogicUnit &, RandomAccessMemory &);

    void fetch();
    void decode();
    void load();
    void execute();
    void write();

    uint64_t evaluate_address(MemoryOperand);

    register_ptr get_instruction_pointer_register() const;
    register_ptr get_instruction_register() const;
    bool get_load_from_memory() const;
    bool get_write_to_memory() const;
};
