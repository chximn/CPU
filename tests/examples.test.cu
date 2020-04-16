#include <catch.hpp>
#include "cpu.hh"
#include "loader.hh"
#include "program.hh"
#include "instruction.hh"
#include "operand.hh"

TEST_CASE("example 01: mov, add, mov") {
    // mov al, [0x5]
    Instruction i1(instruction_code::mov, std::vector<operand_ptr> {
        std::make_shared<RegisterOperand>(register_code::al),
        std::make_shared<MemoryOperand>(5, 8)
    });


    // mov dl, [0x3]
    Instruction i2(instruction_code::mov, std::vector<operand_ptr> {
        std::make_shared<RegisterOperand>(register_code::dl),
        std::make_shared<MemoryOperand>(3, 8)
    });

    // add dl, al
    Instruction i3(instruction_code::add, std::vector<operand_ptr> {
        std::make_shared<RegisterOperand>(register_code::dl),
        std::make_shared<RegisterOperand>(register_code::al)
    });

    // add [0x8], al
    Instruction i4(instruction_code::mov, std::vector<operand_ptr> {
        std::make_shared<MemoryOperand>(8, 8),
        std::make_shared<RegisterOperand>(register_code::dl)
    });

    // add bh, [0x8]
    Instruction i5(instruction_code::mov, std::vector<operand_ptr> {
        std::make_shared<RegisterOperand>(register_code::bh),
        std::make_shared<MemoryOperand>(8, 8)
    });

    // hlt
    Instruction i6(instruction_code::hlt, std::vector<operand_ptr>{});

    Program program;
    program.add_instruction(i1);
    program.add_instruction(i2);
    program.add_instruction(i3);
    program.add_instruction(i4);
    program.add_instruction(i5);
    program.add_instruction(i6);
    program.add_data(std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1});


    RandomAccessMemory ram;
    CentralProcessingUnit cpu(ram);
    auto registers = cpu.get_registers();

    Loader loader(cpu, ram);
    loader.load(program);

    cpu.start();

    REQUIRE(registers[register_code::bh]->get_value() == 8);
}
