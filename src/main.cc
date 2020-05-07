#include <iostream>

#include "assembler.hh"
#include "ram.hh"
#include "cpu.hh"
#include "loader.hh"
#include "helpers.hh"

int main() {
    std::string code =
        "section .data\n"
        "   x dd 1.5\n"
        "   y dd 0.25\n"
        "   z dd 0.0\n"
        "section .text\n"
        "main:\n"
        "   fld dword [x]\n"
        "   fld dword [y]\n"
        "   faddp\n"
        "   fst dword [z]\n"
        "   mov eax, [ebp + 8]\n"
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

    ram.address_register->set_value(0x8 + LOADER_DEFAULT_DATA_SEGMENT);
    ram.size = 32;
    ram.load();
    std::cout << helpers::to_hex(ram.data_register->get_value()) << "\n";
    std::cout << helpers::to_hex(cpu.get_floating_point_unit().stages[0]->get_value()) << "\n";
    return 0;
}
