#pragma once
#include "operand.hh"
#include <vector>

enum class instruction_code {
    mov, lea, add, sub, fld, jmp, movdqua
};

class Instruction {
private:
    instruction_code code;
    std::vector<Operand> operands;

public:
    instruction_code get_code() const;
    std::vector<Operand> const & get_operands() const;
};
