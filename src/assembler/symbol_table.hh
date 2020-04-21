#pragma once
#include <map>

class Driver;

class SymbolTable {
private:
    std::map<std::string, int> labels;
    std::map<std::string, int> objects;

public:
    SymbolTable() = default;
    friend Driver;
};
