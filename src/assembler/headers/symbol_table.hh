#pragma once
#include <map>
#include "instruction.hh"

class SymbolTable {
public:
    std::map<std::string, instruction_ptr> labels;
    std::map<std::string, int> objects;
    SymbolTable() = default;

};
