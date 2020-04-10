#pragma once
#include "instruction.hh"
#include <vector>
#include <cstdint>

class Program {
private:
    std::vector<Instruction> instructions;
    std::vector<uint8_t> data;

public:
    Program() = default;

    void add_instruction(Instruction const &);
    uint64_t add_data(std::vector<uint8_t> const &);
    std::vector<Instruction> const & get_instructions() const;
    std::vector<uint8_t> const & get_data() const;
};
