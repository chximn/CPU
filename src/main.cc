#include <iostream>
#include <cxxopts.hpp>
#include "ui.hh"

int main(int argc, char * argv[]) {
    // std::string code =
    //     "section .data\n"
    //     "   x db 0,  1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15\n"
    //     "   y db 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31\n"
    //     "   m db FFh, 0, FFh,  0, FFh,  0,  FFh,  0, FFh,  0, FFh,  0, FFh,  0, FFh,  0\n"
    //     "   a dd 1, 2, 3, 4\n"
    //     "section .text\n"
    //     "main:\n"
    //     "   movdqu xmm0, [x]\n"
    //     "   movdqa xmm1, [y]\n"
    //     "   paddb xmm0, xmm1\n"
    //     "   pand  xmm0, [m]\n"
    //     "   movdqa xmm3, [a]\n"
    //     "   pshufd xmm2, xmm3, 00110111b\n"
    //     "   mov cl, 100\n"
    //     "   loop:\n"
    //     "   sub cl, 1\n"
    //     "   push ecx\n"
    //     "   cmp cl, 0\n"
    //     "   jne loop\n"
    //     "   hlt\n"
    //     "   nop\n"
    //     "   nop\n"
    //     "   nop\n"
    //     "   nop\n"
    //     "   nop\n"
    //     "   nop\n"
    //     "   nop\n"
    //     "   nop\n"
    //     "   hlt\n";

    // UserInterface ui(code, false, 30);
    // ui.start();

    using namespace cxxopts;

    Options options(argv[0], "Intel's x86-64 Architecture Simulation");
    options.allow_unrecognised_options()
           .add_options()
                ("f,filename",    "Input filename", cxxopts::value<std::string>())
                ("i,interactive", "Enable interactive execution")
                ("s,speed",       "Execution speed (instruction per minute)", cxxopts::value<int>()->default_value("100"))
                ("h,help",        "Print usage");

    options.positional_help("filename")
           .show_positional_help();

    std::string filename;
    bool interactive;
    int speed;

    try {
        options.parse_positional({"filename", "positional"});
        auto result = options.parse(argc, argv);

        if (result.count("help") || !result.count("filename")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        filename = result["filename"].as<std::string>();
        speed = result["speed"].as<int>();
        interactive = result.count("interactive") > 0;
    }

    catch (const cxxopts::OptionException& e)
    {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }

    std::cout << "speed: " << speed << "\n";
    std::cout << "filename: " << filename << "\n";
    std::cout << "interactive: " << interactive << "\n";

    return 0;
}
