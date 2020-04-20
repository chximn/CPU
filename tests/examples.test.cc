#include <catch.hpp>
#include "cpu.hh"
#include "loader.hh"
#include "program.hh"
#include "instruction.hh"
#include "operand.hh"

TEST_CASE("examples") {
    RandomAccessMemory ram;
    CentralProcessingUnit cpu(ram);
    auto registers = cpu.get_registers();

    Loader loader(cpu, ram);

    Instruction halt(instruction_code::hlt, std::vector<operand_ptr>{});

    SECTION("example 01: mov, add, mov") {
        // mov al, [0x5]
        Instruction i1(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::al),
            std::make_shared<MemoryOperand>(5, 8, register_code::ds)
        }, 8);


        // mov dl, [0x3]
        Instruction i2(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::dl),
            std::make_shared<MemoryOperand>(3, 8, register_code::ds)
        }, 8);

        // add dl, al
        Instruction i3(instruction_code::add, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::dl),
            std::make_shared<RegisterOperand>(register_code::al)
        }, 8);

        // add [0x8], dl
        Instruction i4(instruction_code::add, std::vector<operand_ptr> {
            std::make_shared<MemoryOperand>(8, 8, register_code::ds),
            std::make_shared<RegisterOperand>(register_code::dl)
        }, 8);

        // mov bh, [0x8]
        Instruction i5(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::bh),
            std::make_shared<MemoryOperand>(8, 8, register_code::ds)
        }, 8);


        Program program;
        program.add_instruction(i1);
        program.add_instruction(i2);
        program.add_instruction(i3);
        program.add_instruction(i4);
        program.add_instruction(i5);
        program.add_instruction(halt);
        program.add_data(std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1});


        loader.load(program);

        registers[register_code::ebx]->set_value(0);
        cpu.start();

        REQUIRE(registers[register_code::bh]->get_value() == 9);
    }


    SECTION("example 02: lea") {

        // mov bx, 1
        Instruction i1(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::bx),
            std::make_shared<ImmediateOperand>(1)
        }, 16);


        // lea ax, [bx * 4 + 1]
        Instruction i2(instruction_code::lea, std::vector<operand_ptr>{
            std::make_shared<RegisterOperand>(register_code::ax),
            std::make_shared<MemoryOperand>(register_code::bx, 4, 1, 16)
        }, 16);

        // mov cl, byte [ax]
        Instruction i3(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::cl),
            std::make_shared<MemoryOperand>(register_code::ax, 0, 8, register_code::ds)
        }, 8);

        Program program;
        program.add_instruction(i1);
        program.add_instruction(i2);
        program.add_instruction(i3);
        program.add_instruction(halt);

        program.add_data(std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1});
        loader.load(program);

        cpu.start();

        REQUIRE(registers[register_code::cl]->get_value() == 5);
    }

    SECTION("example 03: push") {
        Instruction i1(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::bx),
            std::make_shared<ImmediateOperand>(0x2647)
        }, 16);

        Instruction i2(instruction_code::push, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::bx)
        }, 16);

        Instruction i3(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::cx),
            std::make_shared<MemoryOperand>(register_code::rsp, 0, 16, register_code::ss)
        }, 16);

        Program program;
        program.add_instruction(i1);
        program.add_instruction(i2);
        program.add_instruction(i3);
        program.add_instruction(halt);

        loader.load(program);

        cpu.start();

        REQUIRE(registers[register_code::rsp]->get_value() == LOADER_DEFAULT_STACK_SEGMENT + LOADER_DEFAULT_STACK_SIZE - 2);
        REQUIRE(registers[register_code::cx]->get_value() == 0x2647);
        REQUIRE(registers[register_code::bx]->get_value() == 0x2647);
    }

    SECTION("example 04: pop") {

        Instruction i1(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::rbx),
            std::make_shared<ImmediateOperand>(0x2647)
        }, 64);

        Instruction i2(instruction_code::push, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::rbx)
        }, 64);

        Instruction i3(instruction_code::pop, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::rax)
        }, 64);

        Program program;
        program.add_instruction(i1);
        program.add_instruction(i2);
        program.add_instruction(i3);
        program.add_instruction(halt);

        loader.load(program);

        registers[register_code::rax]->set_value(0);
        cpu.start();

        REQUIRE(registers[register_code::rbx]->get_value() == 0x2647);
        REQUIRE(registers[register_code::rax]->get_value() == 0x2647);
        REQUIRE(registers[register_code::rsp]->get_value() == LOADER_DEFAULT_STACK_SEGMENT + LOADER_DEFAULT_STACK_SIZE);
    }
}
