#include "instruction.hh"

std::string Instruction::to_string(instruction_code code) {
    std::string names[] = {
        "mov", "lea", "push", "pop",
        "add", "sub", "mul", "div",
        "neg", "_and", "_or", "_xor", "_not", "shl", "shr",
        "cmp", "jmp", "je", "jne", "jl", "jg", "jle", "jge",
        "call", "ret",
        "nop", "hlt",

        "fld", "fldz", "fld1", "fst", "fstp", "fadd", "faddp",

        "movdqu", "movdqa", "paddb", "paddw", "paddd", "paddd", "addps", "addpd", "pand", "pshufd"
    };

    return names[code];
}

Instruction::Instruction(instruction_code c, std::vector<operand_ptr> const & ops):
    code(c), operands(ops) {}

Instruction::Instruction(instruction_code c, std::vector<operand_ptr> const & ops, uint8_t s):
    code(c), operands(ops), size(s) {}

Instruction::Instruction(Instruction const & i): code(i.get_code()), size(i.get_size()) {
    for (auto const & op : i.get_operands()) {
        operands.push_back(op);
    }
}

uint8_t Instruction::get_size() const {
    return size;
}

instruction_code Instruction::get_code() const {
    return code;
}

std::vector<operand_ptr> const & Instruction::get_operands() const {
    return operands;
}

void Instruction::set_operands(std::vector<operand_ptr> const & ops) {
    operands = ops;
}

std::string Instruction::to_string() const {
    auto str = Instruction::to_string(code);

    if (operands.size() >= 1) str += " " + operands.at(0)->to_string();
    if (operands.size() >= 2) str += ", " + operands.at(1)->to_string();
    if (operands.size() == 3) str += ", " + operands.at(2)->to_string();

    return str;
}
