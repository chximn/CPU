#include <iostream>

#include "ui.hh"

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
        "   mov cl, 100\n"
        "   loop:\n"
        "   sub cl, 1\n"
        "   push ecx\n"
        "   cmp cl, 0\n"
        "   jne loop\n"
        "   hlt\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   hlt\n";

    UserInterface ui(code, true, 500);
    ui.start();

    return 0;
}
