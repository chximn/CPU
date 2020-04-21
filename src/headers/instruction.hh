#pragma once
#include "operand.hh"
#include <vector>

enum class instruction_code {
    mov, lea,
    push, pop,


    add, sub, mul, div, neg,

    _and, _or, _xor, _not,

    shl, shr,

    cmp,

    jmp, je, jne, jl, jg, jle, jge,

    nop, hlt
};

class Instruction {
private:
    instruction_code code;
    std::vector<operand_ptr> operands;
    uint8_t size;

public:
    Instruction() = delete;
    Instruction(instruction_code, std::vector<operand_ptr> const & ops);
    Instruction(instruction_code, std::vector<operand_ptr> const & ops, uint8_t s);

    instruction_code get_code() const;
    uint8_t get_size() const;
    std::vector<operand_ptr> const & get_operands() const;
};
