#pragma once
#include "ram.hh"
#include "vector_register.hh"
#include <emmintrin.h>

enum class vector_operation {
    load,
    write,
    add
};

enum class vector_operation_type {
    _float,
    _double,
    byte,
    word,
    dword,
    qword
};

class VectorUnit {
private:
    RandomAccessMemory & ram;

public:
    VectorUnit() = delete;
    VectorUnit(RandomAccessMemory &);

    vector_register_ptr source;
    vector_register_ptr source2;
    vector_register_ptr destination;

    vector_register_ptr registers[8];
    vector_operation operation;
    vector_operation_type operation_type;

    void execute();
};
