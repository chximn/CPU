#pragma once
#include <sstream>

#include "program.hh"
#include "symbol_table.hh"

class Driver {
private:
    Program program;

public:
    Driver() = default;

    SymbolTable symbol_table;

    Program const & get_program();
    void set_program(Program const &);
};
