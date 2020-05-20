#pragma once
#include <cstdint>
#include <vector>

#include "instruction.hh"

class Program {
private:
    std::vector<instruction_ptr> instructions;
    std::vector<uint8_t> data;

public:
    Program() = default;

    void add_instruction(instruction_ptr);
    uint64_t add_data(std::vector<uint8_t> const &);
    std::vector<instruction_ptr> const & get_instructions() const;
    std::vector<uint8_t> const & get_data() const;

    void print(std::ostream &) const;
};
