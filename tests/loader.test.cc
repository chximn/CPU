#include <catch.hpp>
#include "loader.hh"
#include "program.hh"
#include "instruction.hh"
#include "ram.hh"
#include "cpu.hh"

TEST_CASE("loader") {
    Instruction i1(instruction_code::mov, std::vector<operand_ptr> {
        std::make_shared<RegisterOperand>(register_code::eax),
        std::make_shared<ImmediateOperand>(2647)
    });

    auto i1p = reinterpret_cast<uint64_t>(const_cast<void*>(reinterpret_cast<void const *>(&i1)));

    Instruction i2(instruction_code::mov, std::vector<operand_ptr> {
        std::make_shared<RegisterOperand>(register_code::ebx),
        std::make_shared<RegisterOperand>(register_code::eax)
    });

    auto i2p = reinterpret_cast<uint64_t>(const_cast<void*>(reinterpret_cast<void const *>(&i2)));

    Program program;
    program.add_instruction(i1);
    program.add_instruction(i2);
    program.add_data(std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7});


    RandomAccessMemory ram;
    CentralProcessingUnit cpu(ram);

    auto registers = cpu.get_registers();

    Loader loader(cpu, ram);

    ram.address_register->set_value(LOADER_DEFAULT_CODE_SEGMENT);
    ram.size = 64;
    ram.data_register->set_value(0);
    ram.write();

    loader.load(program);

    ram.address_register->set_value(LOADER_DEFAULT_CODE_SEGMENT);
    ram.size = 64;
    ram.load();

    REQUIRE(ram.data_register->get_value() == i1p);

    ram.address_register->set_value(LOADER_DEFAULT_CODE_SEGMENT + 8);
    ram.load();

    REQUIRE(ram.data_register->get_value() == i2p);

    ram.address_register->set_value(LOADER_DEFAULT_DATA_SEGMENT);
    ram.load();

    REQUIRE(ram.data_register->get_value() == 0x0706050403020100);

    REQUIRE(registers[register_code::cs]->get_value() == LOADER_DEFAULT_CODE_SEGMENT);
    REQUIRE(registers[register_code::ds]->get_value() == LOADER_DEFAULT_DATA_SEGMENT);
}
