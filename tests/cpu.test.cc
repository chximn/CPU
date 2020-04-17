#include <catch.hpp>
#include "instruction.hh"
#include "cpu.hh"
#include "ram.hh"
#include "cu.hh"

TEST_CASE("cpu") {
    RandomAccessMemory ram;
    CentralProcessingUnit cpu(ram);


    auto registers = cpu.get_registers();
    auto cs = registers[register_code::cs];
    cs->set_value(0);
    ArithemeticLogicUnit & alu = cpu.get_arithemetic_logic_unit();

    std::vector<operand_ptr> operands {
        std::make_shared<RegisterOperand>(register_code::eax),
        std::make_shared<ImmediateOperand>(2647)
    };

    Instruction * i = new Instruction(instruction_code::mov, operands);

    ram.data_register->set_value((uint64_t) i);
    ram.address_register->set_value(800);
    ram.size = 64;
    ram.write();
    auto cu = cpu.get_control_unit();
    auto ip = cu.get_instruction_pointer_register();
    auto ir = cu.get_instruction_register();

    ip->set_value(800);

    SECTION("fetch instruction") {
        cu.fetch(cs);
        REQUIRE(ip->get_value() == 808);

        uint64_t instruction_pointer = (ir->get_value());
        Instruction * loaded_instruction = reinterpret_cast<Instruction *>(instruction_pointer);

        REQUIRE(loaded_instruction == i);
    }

    SECTION("evaluate address") {
        registers[register_code::ds]->set_value(7);
        registers[register_code::eax]->set_value(2200);
        registers[register_code::ecx]->set_value(100);
        MemoryOperand operand(register_code::eax, register_code::ecx, 4, 40, 64, register_code::ds);

        REQUIRE(cu.evaluate_address(operand) == 2647);
    }

    SECTION("decode") {
        cu.fetch(cs);
        cu.decode();

        REQUIRE(cu.get_load_from_memory() == false);
        REQUIRE(cu.get_write_to_memory() == false);

        REQUIRE(alu.destination == registers[register_code::eax]);
    }

    SECTION("load") {
        cu.fetch(cs);
        cu.decode();

        ram.data_register->set_value(2647);
        cu.load();
        REQUIRE(ram.data_register->get_value() == 2647);
    }

    SECTION("execute") {
        cu.fetch(cs);
        cu.decode();
        cu.load();
        cu.execute();

        REQUIRE(registers[register_code::eax]->get_value() == 2647);
    }

    SECTION("write") {
        cu.fetch(cs);
        cu.decode();
        cu.load();
        cu.execute();

        ram.data_register->set_value(2647);
        cu.write();
        REQUIRE(ram.data_register->get_value() == 2647);
    }
}
