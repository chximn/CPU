#pragma once
#include "parser.hpp"
#include "scanner.hh"
#include "program.hh"
#include "driver.hh"
#include <iostream>
#include <istream>
#include <ostream>
#include <cstring>

class Assembler {
private:
    Driver driver;
    Scanner scanner;
    yy::Parser parser;

public:
    Assembler() = delete;
    Assembler(std::istream &, std::ostream &);

    Program const & assemble();
};
