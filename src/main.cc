#include <iostream>
#include "cpu.hh"
#include "loader.hh"
#include "program.hh"
#include "instruction.hh"
#include "operand.hh"

int main() {

    Instruction i1(instruction_code::mov, std::vector<operand_ptr> {
        std::make_shared<RegisterOperand>(register_code::eax),
        std::make_shared<ImmediateOperand>(2647)
    });


    Instruction i2(instruction_code::mov, std::vector<operand_ptr> {
        std::make_shared<RegisterOperand>(register_code::ebx),
        std::make_shared<RegisterOperand>(register_code::eax)
    });

    Instruction i3(instruction_code::hlt, std::vector<operand_ptr>{});

    Program program;
    program.add_instruction(i1);
    program.add_instruction(i2);
    program.add_instruction(i3);
    program.add_data(std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7});


    RandomAccessMemory ram;
    CentralProcessingUnit cpu(ram);
    auto registers = cpu.get_registers();

    Loader loader(cpu, ram);
    loader.load(program);

    cpu.start();

    std::cout << "ebx = " << registers[register_code::ebx]->get_value() << "\n";

    return 0;
}
