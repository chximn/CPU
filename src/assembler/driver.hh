#pragma once
#include "symbol_table.hh"
#include <sstream>
#include "program.hh"

class Driver {
private:
    Program program;

public:
    Driver() = default;

    SymbolTable symbol_table;

    Program const & get_program();
    void set_program(Program const &);
};
