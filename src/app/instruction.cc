#include "instruction.hh"

Instruction::Instruction(instruction_code c, std::vector<Operand> const & ops):
    code(c), operands(ops) {}

instruction_code Instruction::get_code() const {
    return code;
}

std::vector<Operand> const & Instruction::get_operands() const {
    return operands;
}
