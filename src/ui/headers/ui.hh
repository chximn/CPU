#pragma once
#include <string>
#include <locale>
#include <codecvt>
#include <thread>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

#include "assembler.hh"
#include "ram.hh"
#include "cpu.hh"
#include "loader.hh"
#include "helpers.hh"

class UserInterface {
private:
    std::string code;
    bool interactive;
    int speed;

    Assembler assembler;
    RandomAccessMemory ram;
    CentralProcessingUnit cpu;
    Loader loader;

    Program program;

    std::string reset_position;

public:
    UserInterface() = delete;
    UserInterface(std::string const & code, bool interactive, int speed);

    void start();
    void render();
};
