#include <iostream>

#include "assembler.hh"
#include "ram.hh"
#include "cpu.hh"
#include "loader.hh"
#include "helpers.hh"

int main() {
    std::string code =
        "section .data\n"
        "   x db 0,  1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15\n"
        "   y db 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31\n"
        "   m db FFh, 0, FFh,  0, FFh,  0,  FFh,  0, FFh,  0, FFh,  0, FFh,  0, FFh,  0\n"
        "   a dd 1, 2, 3, 4\n"
        "section .text\n"
        "main:\n"
        "   movdqu xmm0, [x]\n"
        "   movdqa xmm1, [y]\n"
        "   paddb xmm0, xmm1\n"
        "   pand  xmm0, [m]\n"
        "   movdqa xmm3, [a]\n"
        "   pshufd xmm2, xmm3, 00110111b\n"
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

    std::cout << helpers::to_hex(cpu.get_vector_unit().get_register(register_code::xmm2)->get_low()) << "\n";
    std::cout << helpers::to_hex(cpu.get_vector_unit().get_register(register_code::xmm2)->get_high()) << "\n";
    return 0;
}
