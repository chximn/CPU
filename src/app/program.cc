#include "program.hh"

void Program::add_instruction(Instruction const & instruction) {
    instructions.push_back(&instruction);
}

uint64_t Program::add_data(std::vector<uint8_t> const & new_data) {
    uint64_t address = data.size();
    data.insert(data.end(), new_data.begin(), new_data.end());
    return address;
}

std::vector<const Instruction *> const & Program::get_instructions() const {
    return instructions;
}

std::vector<uint8_t> const & Program::get_data() const {
    return data;
}
