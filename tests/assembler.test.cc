#include <catch.hpp>
#include "assembler.hh"


TEST_CASE("assembler") {

    auto code = ""
          "mov eax, 1000\n"
          "mov eax, [1000]\n"
          ".loop:\n"
          "mov eax, [eax]\n"
          "mov eax, [eax + 1000]\n"
          "mov eax, [eax + ecx]\n"
          "mov eax, [eax + ecx * 1]\n"
          "mov eax, [eax + ecx * 8 + 123]\n"
          "mov eax, [ecx * 2 + 123]\n"
          "mov eax, dword [ecx * 4 + 2]\n"
          "mov eax, dword ss:[ecx * 4 + 2]\n"
          "lea eax, [ecx * 4]\n"
          "push eax\n"
          "push dword [eax]\n"
          "push dword 100\n"
          "shl eax\n"
          "shl eax, 2\n"
          "shl eax, cl\n"
          "shl dword [eax], 2\n"
          "jmp\n"
          "jne 8\n"
          "je 8\n"
          "jg 8\n"
          "jl 8\n"
          "jle 8\n"
          "jge 8\n"
          "call 8\n"
          "ret\n"
          "nop\n"
          "mov eax, x\n"
          "mov eax, [x]\n"
          "hlt\n\n";

    Assembler assembler(code);
    Program program = assembler.assemble();
}
