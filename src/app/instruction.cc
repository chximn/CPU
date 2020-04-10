#include "instruction.hh"

instruction_code Instruction::get_code() const {
    return code;
}

std::vector<Operand> const & Instruction::get_operands() const {
    return operands;
}
