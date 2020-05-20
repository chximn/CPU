#include "program.hh"

void Program::add_instruction(instruction_ptr instruction) {
    instructions.push_back(instruction);
}

uint64_t Program::add_data(std::vector<uint8_t> const & new_data) {
    uint64_t address = data.size();
    data.insert(data.end(), new_data.begin(), new_data.end());
    return address;
}

std::vector<instruction_ptr> const & Program::get_instructions() const {
    return instructions;
}

std::vector<uint8_t> const & Program::get_data() const {
    return data;
}

void Program::print(std::ostream & os) const {
    os << "data:\n";

    int i = 0;
    for (auto const & byte : data) {
        if (i && (i % 8 == 0))
            os << "\n";

        os << helpers::to_hex(byte, "") << " ";
        i++;
    }

    os << "\n";

    os << "instructions:\n";
    for (auto const & instruction : instructions) {
        os << instruction->to_string() << "\n";
    }
}
