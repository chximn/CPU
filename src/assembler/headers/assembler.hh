#pragma once
#include <cstring>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "driver.hh"
#include "logger.hh"
#include "parser.hpp"
#include "program.hh"
#include "scanner.hh"

class Assembler {
private:
    std::stringstream in;
    std::stringstream out;

    std::string code;

    Logger logger;

    Scanner scanner;
    yy::Parser parser;

public:
    Driver driver;
    Assembler() = delete;
    Assembler(std::string const &);

    Program const & assemble();
};
