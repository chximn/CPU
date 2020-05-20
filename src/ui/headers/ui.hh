#pragma once

#include <vector>
#include <list>
#include <thread>
#include <algorithm>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

#include "assembler.hh"
#include "ram.hh"
#include "cpu.hh"
#include "loader.hh"
#include "helpers.hh"
#include "padding.hh"

#define SCREEN_HEIGHT 30
#define STACK_HEIGHT 30
#define STACK_BYTES_PER_LINE 4
#define MEMORY_BYTES_PER_LINE 18
#define REGISTERS_PER_LINE 5
#define ADDRESS_WIDTH 8
#define INSTRUCTIONS_RANGE 15

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
    int n_lines;

    ftxui::Element render_stack();
    ftxui::Element render_data_segment();
    ftxui::Element render_registers();
    ftxui::Element render_instructions();
    ftxui::Element render_fpu();
    ftxui::Element render_sse();

    void render();
    void print(std::string const &);
    std::string getline();
    void cleanup();

public:
    UserInterface() = delete;
    UserInterface(std::string const & code, bool interactive, int speed);

    void start();
};
