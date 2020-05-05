#include <catch.hpp>
#include <sstream>
#include "assembler.hh"

TEST_CASE("assembler") {

    SECTION("sections") {

        auto code =
        "section .data\n"
        "; comment\n"
        "x dw 1, 2, 3\n"
        "y dw 4, 5, 6\n"
        "section .text\n"
        "mov eax, 1000\n"
        "mov eax, [y]\n"
        "hlt\n";

        Assembler assembler(code);
        Program program = assembler.assemble();

        std::string output =
        "data:\n"
        "01 00 02 00 03 00 04 00 \n05 00 06 00 \n"
        "instructions:\n"
        "mov eax, 0x3e8\n"
        "mov eax, dword ds:[0x06]\n"
        "hlt\n";

        std::stringstream sstream;
        program.print(sstream);

        REQUIRE(sstream.str() == output);
    }


    SECTION("instructions") {
        auto code =
        "section .data\n"
        "x dw 1\n"
        "section .text\n"
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
        "; comment\n"
        "mov eax, dword ss:[ecx * 4 + 2]\n"
        "lea eax, [ecx * 4]\n"
        "push eax\n"
        "push dword [eax]\n"
        "push dword 100\n"
        "shl eax\n"
        ".lbl: \n"
        "shl eax, 2\n"
        "shl eax, cl\n"
        "shl dword [eax], 2\n"
        "jmp .lbl \n"
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

}
