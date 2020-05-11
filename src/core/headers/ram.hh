#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "register.hh"
#include <iostream>
#include "instruction.hh"
#include "helpers.hh"

#define MEMORY_SIZE 1048576 // 1 megabyte

class RandomAccessMemory {
private:
    uint8_t data[MEMORY_SIZE];
public:
    RandomAccessMemory();

    register_ptr address_register;
    register_ptr data_register;
    uint8_t size;

    void load() const;
    void write();

    std::vector<uint8_t> get_data(int from, int size);
    std::vector<std::pair<uint64_t, std::string>> get_instructions(int from, int n);
};
