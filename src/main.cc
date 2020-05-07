#include <iostream>

#include "assembler.hh"
#include "ram.hh"
#include "cpu.hh"
#include "loader.hh"
#include "helpers.hh"

int main() {
    std::string code =
        "section .data\n"
        "   x dd 1, 2, 3, 4\n"
        "section .text\n"
        "main:\n"
        "   movdqu xmm0, [x]\n"
        "   hlt\n";

    Assembler assembler(code);
    Program program = assembler.assemble();

    program.print(std::cout);

    RandomAccessMemory ram;
    CentralProcessingUnit cpu(ram);
    Loader loader(cpu, ram);
    loader.load(program);

    try {
        cpu.start();
    }

    catch (const char * s) {
        std::cout << std::string(s) << "\n";
    }

    std::cout << helpers::to_hex(cpu.get_vector_unit().get_register(register_code::xmm0)->get_low()) << "\n";
    std::cout << helpers::to_hex(cpu.get_vector_unit().get_register(register_code::xmm0)->get_high()) << "\n";
    return 0;
}
