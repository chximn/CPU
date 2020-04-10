#include "instruction.hh"

Instruction::Instruction(instruction_code c, std::vector<operand_ptr> const & ops):
    code(c), operands(ops) {}

instruction_code Instruction::get_code() const {
    return code;
}

std::vector<operand_ptr> const & Instruction::get_operands() const {
    return operands;
}
