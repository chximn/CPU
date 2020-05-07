#pragma once
#include "ram.hh"
#include "vector_register.hh"
#include <emmintrin.h>

enum class vector_operation {
    mov,
    paddb,
    paddw,
    paddd,
    paddq,
    addps,
    addpd
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
    vector_register_ptr temp_register;

    vector_register_ptr registers[8];
    vector_operation operation;

    bool load_from_memory;
    bool write_to_memory;

    vector_register_ptr get_register(register_code);
    void execute();
};
