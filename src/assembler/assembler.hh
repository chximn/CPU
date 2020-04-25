#pragma once
#include "parser.hpp"
#include "scanner.hh"
#include "program.hh"
#include "driver.hh"
#include <iostream>
#include <istream>
#include <ostream>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include "logger.hh"

class Assembler {
private:
    std::stringstream in;
    std::stringstream out;

    std::string code;

    Driver driver;
    Scanner scanner;
    yy::Parser parser;

public:
    Assembler() = delete;
    Assembler(std::string const &);

    Program const & assemble();
};
