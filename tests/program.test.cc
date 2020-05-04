#include <catch.hpp>
#include <memory>
#include <cstdint>
#include "program.hh"
#include "instruction.hh"
#include <iostream>

TEST_CASE("program") {
    Program program;

    SECTION("add and get instructions") {
        auto i1 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr>{
            std::make_shared<RegisterOperand>(register_code::eax),
            std::make_shared<ImmediateOperand>(2647)
        });

        auto i2 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr>{
            std::make_shared<RegisterOperand>(register_code::ebx),
            std::make_shared<RegisterOperand>(register_code::eax)
        });

        program.add_instruction(i1);
        program.add_instruction(i2);

        auto instructions = program.get_instructions();
        REQUIRE(instructions.size() == 2);
    }

    SECTION("add and get data") {
        std::vector<uint8_t> d1 = {1, 2, 3, 4, 5};
        std::vector<uint8_t> d2 = {6, 7, 8, 9, 10};
        std::vector<uint8_t> d3 = {11, 12, 13, 14, 15};

        REQUIRE(program.add_data(d1) == 0);
        REQUIRE(program.add_data(d2) == 5);
        REQUIRE(program.add_data(d3) == 10);

        auto data = program.get_data();
        REQUIRE(data.size() == 15);

        uint8_t i = 0;
        for (auto const & v : data) {
            i++;
            REQUIRE(i == v);
        }
    }
}
