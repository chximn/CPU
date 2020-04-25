#pragma once
#include "symbol_table.hh"
#include <sstream>
#include "program.hh"

class Driver {
private:
    SymbolTable symbol_table;
    Program program;
public:
    Driver() = default;

    Program const & get_program();
};
