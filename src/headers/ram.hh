#pragma once
#include <cstdint>
#include <vector>
#include "register.hh"

#define MEMORY_SIZE 1024

class RandomAccessMemory {
private:
    uint8_t data[MEMORY_SIZE];
public:
    RandomAccessMemory();

    register_ptr address_register;
    register_ptr data_register;

    void load() const;
    void write();
};
