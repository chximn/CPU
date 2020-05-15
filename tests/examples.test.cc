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

    auto halt = std::make_shared<Instruction>(instruction_code::hlt, std::vector<operand_ptr>{});

    SECTION("example 01: mov, add, mov") {
        // mov al, [0x5]
        auto i1 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::al),
            std::make_shared<MemoryOperand>(5, 8, register_code::ds)
        }, 8);


        // mov dl, [0x3]
        auto i2 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::dl),
            std::make_shared<MemoryOperand>(3, 8, register_code::ds)
        }, 8);

        // add dl, al
        auto i3 = std::make_shared<Instruction>(instruction_code::add, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::dl),
            std::make_shared<RegisterOperand>(register_code::al)
        }, 8);

        // add [0x8], dl
        auto i4 = std::make_shared<Instruction>(instruction_code::add, std::vector<operand_ptr> {
            std::make_shared<MemoryOperand>(8, 8, register_code::ds),
            std::make_shared<RegisterOperand>(register_code::dl)
        }, 8);

        // mov bh, [0x8]
        auto i5 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
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
        auto i1 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::bx),
            std::make_shared<ImmediateOperand>(1)
        }, 16);


        // lea ax, [bx * 4 + 1]
        auto i2 = std::make_shared<Instruction>(instruction_code::lea, std::vector<operand_ptr>{
            std::make_shared<RegisterOperand>(register_code::ax),
            std::make_shared<MemoryOperand>(register_code::bx, 4, 1, 16)
        }, 16);

        // mov cl, byte [ax]
        auto i3 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
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
        auto i1 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::bx),
            std::make_shared<ImmediateOperand>(0x2647)
        }, 16);

        auto i2 = std::make_shared<Instruction>(instruction_code::push, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::bx)
        }, 16);

        auto i3 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
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

        REQUIRE(registers[register_code::rsp]->get_value() == LOADER_DEFAULT_STACK_SIZE - 2);
        REQUIRE(registers[register_code::cx]->get_value() == 0x2647);
        REQUIRE(registers[register_code::bx]->get_value() == 0x2647);
    }

    SECTION("example 04: pop") {

        auto i1 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::rbx),
            std::make_shared<ImmediateOperand>(0x2647)
        }, 64);

        auto i2 = std::make_shared<Instruction>(instruction_code::push, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::rbx)
        }, 64);

        auto i3 = std::make_shared<Instruction>(instruction_code::pop, std::vector<operand_ptr> {
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
        REQUIRE(registers[register_code::rsp]->get_value() == LOADER_DEFAULT_STACK_SIZE);
    }

    SECTION("example 05: jmp") {
        auto i1 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::ebx),
            std::make_shared<ImmediateOperand>(2647)
        }, 32);

        auto i2 = std::make_shared<Instruction>(instruction_code::jmp, std::vector<operand_ptr> {
            std::make_shared<ImmediateOperand>(8)
        });

        auto i3 = std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
            std::make_shared<RegisterOperand>(register_code::ebx),
            std::make_shared<ImmediateOperand>(1000)
        }, 32);

        Program program;
        program.add_instruction(i1);
        program.add_instruction(i2);
        program.add_instruction(i3);
        program.add_instruction(halt);

        loader.load(program);

        cpu.start();

        REQUIRE(registers[register_code::ebx]->get_value() == 2647);
    }

    SECTION("example 06: je, jne, jg, gl, jge, jle") {


        std::vector<instruction_ptr> instructions {
            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(2647)
            }, 32),

            std::make_shared<Instruction>(instruction_code::cmp, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(2647)
            }, 32),

            std::make_shared<Instruction>(instruction_code::je, std::vector<operand_ptr> {
                std::make_shared<ImmediateOperand>(8)
            }),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(1000)
            }, 32),

            std::make_shared<Instruction>(instruction_code::cmp, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(1000)
            }, 32),

            std::make_shared<Instruction>(instruction_code::jne, std::vector<operand_ptr> {
                std::make_shared<ImmediateOperand>(8)
            }),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(1000)
            }, 32),

            std::make_shared<Instruction>(instruction_code::cmp, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(2000)
            }, 32),

            std::make_shared<Instruction>(instruction_code::jg, std::vector<operand_ptr> {
                std::make_shared<ImmediateOperand>(8)
            }),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(1000)
            }, 32),

            std::make_shared<Instruction>(instruction_code::cmp, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(3000)
            }, 32),

            std::make_shared<Instruction>(instruction_code::jl, std::vector<operand_ptr> {
                std::make_shared<ImmediateOperand>(8)
            }),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(1000)
            }, 32),

            std::make_shared<Instruction>(instruction_code::cmp, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(2647)
            }, 32),

            std::make_shared<Instruction>(instruction_code::jle, std::vector<operand_ptr> {
                std::make_shared<ImmediateOperand>(8)
            }),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(1000)
            }, 32),

            std::make_shared<Instruction>(instruction_code::cmp, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(2647)
            }, 32),

            std::make_shared<Instruction>(instruction_code::jge, std::vector<operand_ptr> {
                std::make_shared<ImmediateOperand>(8)
            }),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ebx),
                std::make_shared<ImmediateOperand>(1000)
            }, 32)
        };

        Program program;
        for (auto const & i : instructions) {
            program.add_instruction(i);
        }

        program.add_instruction(halt);

        loader.load(program);
        cpu.start();

        REQUIRE(registers[register_code::ebx]->get_value() == 2647);
    }

    SECTION("example 07: factorial of 5") {
        std::vector<instruction_ptr> instructions {

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::eax),
                std::make_shared<ImmediateOperand>(1)
            }, 32),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ecx),
                std::make_shared<ImmediateOperand>(1)
            }, 32),

            std::make_shared<Instruction>(instruction_code::mul, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ecx)
            }, 32),

            std::make_shared<Instruction>(instruction_code::add, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ecx),
                std::make_shared<ImmediateOperand>(1)
            }, 32),

            std::make_shared<Instruction>(instruction_code::cmp, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::ecx),
                std::make_shared<ImmediateOperand>(6)
            }, 32),

            std::make_shared<Instruction>(instruction_code::jne, std::vector<operand_ptr> {
                std::make_shared<ImmediateOperand>(-32)
            }, 32),
        };

        Program program;
        for (auto const & i : instructions) {
            program.add_instruction(i);
        }

        program.add_instruction(halt);

        loader.load(program);
        cpu.start();

        REQUIRE(registers[register_code::eax]->get_value() == 120);
    }

    SECTION("example 08: call") {
        std::vector<instruction_ptr> instructions {
            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::rax),
                std::make_shared<ImmediateOperand>(0)
            }, 64),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::eax),
                std::make_shared<ImmediateOperand>(2647)
            }, 32),

            std::make_shared<Instruction>(instruction_code::call, std::vector<operand_ptr> {
                std::make_shared<ImmediateOperand>(8)
            }, 64),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::eax),
                std::make_shared<ImmediateOperand>(1000)
            }, 32),

            std::make_shared<Instruction>(instruction_code::pop, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::rbx)
            }, 64)
        };

        Program program;
        for (auto const & i : instructions) {
            program.add_instruction(i);
        }

        program.add_instruction(halt);

        loader.load(program);
        cpu.start();

        REQUIRE(registers[register_code::eax]->get_value() == 2647);
        REQUIRE(registers[register_code::rbx]->get_value() == 3 * 8);
    }

    SECTION("example 08: call + ret") {
        std::vector<instruction_ptr> instructions {
            std::make_shared<Instruction>(instruction_code::jmp, std::vector<operand_ptr> {
                std::make_shared<ImmediateOperand>(16)
            }, 64),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::eax),
                std::make_shared<ImmediateOperand>(2647)
            }, 32),

            std::make_shared<Instruction>(instruction_code::ret, std::vector<operand_ptr> {}, 64),

            std::make_shared<Instruction>(instruction_code::mov, std::vector<operand_ptr> {
                std::make_shared<RegisterOperand>(register_code::eax),
                std::make_shared<ImmediateOperand>(1000)
            }, 32),

            std::make_shared<Instruction>(instruction_code::call, std::vector<operand_ptr> {
                std::make_shared<ImmediateOperand>(-32),
            }, 64)
        };

        Program program;
        for (auto const & i : instructions) {
            program.add_instruction(i);
        }

        program.add_instruction(halt);

        loader.load(program);
        cpu.start();

        REQUIRE(registers[register_code::eax]->get_value() == 2647);
    }
}
