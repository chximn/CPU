#include <iostream>

#include "assembler.hh"
#include "ram.hh"
#include "cpu.hh"
#include "loader.hh"
#include "helpers.hh"

int main() {
    std::string code =
        "section .data\n"
        "   x db 1, 2, 3, 4\n"
        "   y db \"abde\", 10, 0\n"
        "   z dd 0, 0, 0, 0\n"
        "   f dd 1.75\n"
        "section .text\n"
        "main:\n"
        "   mov eax, x\n"
        "   mov ebx, y\n"
        "   mov esp, z\n"
        "   mov ecx, 4\n"
        "   loop:\n"
        "       sub ecx, 1\n"
        "       mov dl, [eax]\n"
        "       add dl, [ebx]\n"
        "       mov [esp], dl\n"
        "       add eax, 1\n"
        "       add ebx, 1\n"
        "       add esp, 1\n"
        "       cmp ecx, 0\n"
        "       jne loop\n"
        "       hlt\n";

    Assembler assembler(code);
    Program program = assembler.assemble();

    program.print(std::cout);

    RandomAccessMemory ram;
    CentralProcessingUnit cpu(ram);
    Loader loader(cpu, ram);
    loader.load(program);
    cpu.start();

    ram.address_register->set_value(0x7 + LOADER_DEFAULT_DATA_SEGMENT);
    ram.size = 8;
    ram.load();

    auto registers = cpu.get_registers();

    std::cout << helpers::to_hex(ram.data_register->get_value()) << "\n";
    std::cout << helpers::to_hex(registers[register_code::edx]->get_value()) << "\n";
    return 0;
}
