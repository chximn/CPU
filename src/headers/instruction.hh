#pragma once
#include "operand.hh"
#include <vector>

enum class instruction_code {
    mov, lea, add, sub, fld, jmp, movdqua, hlt
};

class Instruction {
private:
    instruction_code code;
    std::vector<operand_ptr> operands;

public:
    Instruction() = delete;
    Instruction(instruction_code, std::vector<operand_ptr> const & ops);

    instruction_code get_code() const;
    std::vector<operand_ptr> const & get_operands() const;
};
