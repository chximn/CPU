#include "instruction.hh"

Instruction::Instruction(instruction_code c, std::vector<operand_ptr> const & ops):
    code(c), operands(ops) {}

Instruction::Instruction(instruction_code c, std::vector<operand_ptr> const & ops, uint8_t s):
    code(c), operands(ops), size(s) {}

uint8_t Instruction::get_size() const {
    return size;
}

instruction_code Instruction::get_code() const {
    return code;
}

std::vector<operand_ptr> const & Instruction::get_operands() const {
    return operands;
}
