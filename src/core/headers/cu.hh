#pragma once
#include <map>

#include "alu.hh"
#include "fpu.hh"
#include "instruction.hh"
#include "operand.hh"
#include "ram.hh"
#include "register.hh"
#include "sse.hh"

class CentralProcessingUnit;
class UserInterface;

class ControlUnit {
private:
    register_ptr instruction_pointer_register;
    register_ptr instruction_register;
    register_ptr immediate_register;

    bool load_from_memory;
    bool write_to_memory;

    bool halt;
    bool execute_alu;
    bool execute_fpu;
    bool execute_sse;

    uint64_t pop_temp_address;

    std::map<register_code, register_ptr> & registers;
    ArithemeticLogicUnit & alu;
    FloatingPointUnit & fpu;
    VectorUnit & sse;
    RandomAccessMemory & ram;

    void evaluate_destination(operand_ptr);
    void evaluate_source(operand_ptr);

public:
    ControlUnit() = delete;
    ControlUnit(std::map<register_code, register_ptr> &,
                ArithemeticLogicUnit &,
                FloatingPointUnit &,
                VectorUnit &,
                RandomAccessMemory &);

    void fetch(register_ptr cs);
    void decode();
    void load();
    void execute();
    void write();

    uint64_t evaluate_address(MemoryOperand);

    register_ptr get_instruction_pointer_register() const;
    register_ptr get_instruction_register() const;
    bool get_load_from_memory() const;
    bool get_write_to_memory() const;

    friend CentralProcessingUnit;
    friend UserInterface;
};
