#pragma once
#include "cpu.hh"
#include "ram.hh"
#include "program.hh"
#include <cstdint>

#define LOADER_DEFAULT_CODE_SEGMENT  0x1000
#define LOADER_DEFAULT_DATA_SEGMENT  0x8000
#define LOADER_DEFAULT_STACK_SEGMENT 0x15000
#define LOADER_DEFAULT_STACK_SIZE    0x1000

class Loader {
private:
    CentralProcessingUnit & cpu;
    RandomAccessMemory & ram;
public:
    Loader() = delete;
    Loader(CentralProcessingUnit &, RandomAccessMemory &);
    void load(Program const &);
};
