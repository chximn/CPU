#include <catch.hpp>
#include "assembler.hh"


TEST_CASE("assembler") {

    auto code ="\n\n\n\n\n\n\n\n\n\n"
          "mov eax, 1000\n"
          "mov eax, [1000]\n"
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
          "shl eax, cx\n"
          "shl [eax], 2\n"
          "nop\n"
          "hlt\n\n";

    Assembler assembler(code);
    Program program = assembler.assemble();
}
