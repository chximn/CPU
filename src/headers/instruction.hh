#pragma once
#include "operand.hh"
#include <vector>
#include <memory>

enum instruction_code {
    mov, lea,
    push, pop,


    add, sub, mul, _div, neg,

    _and, _or, _xor, _not,

    shl, shr,

    cmp,

    jmp, je, jne, jl, jg, jle, jge, call, ret,

    nop, hlt,

    fld, fldz, fld1, fst, fstp, _fadd, faddp,

    movdqu, movdqa, paddb, paddw, paddd, paddq, addps, addpd, pand
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

    Instruction(Instruction const &);

    instruction_code get_code() const;
    uint8_t get_size() const;
    std::vector<operand_ptr> const & get_operands() const;
    void set_operands(std::vector<operand_ptr> const &);

    std::string to_string() const;


    static std::string to_string(instruction_code);
};

using instruction_ptr = std::shared_ptr<Instruction>;
