#pragma once
#include <map>
#include "register.hh"
#include "alu.hh"
#include "fpu.hh"
#include "sse.hh"
#include "cu.hh"
#include "ram.hh"

class Loader;

class CentralProcessingUnit {
private:
    RandomAccessMemory & ram;

    std::map<register_code, register_ptr> registers;
    register_ptr flags;

    ArithemeticLogicUnit arithmetic_logic_unit;
    FloatingPointUnit floating_point_unit;
    VectorUnit vector_unit;
    ControlUnit control_unit;

public:
    CentralProcessingUnit() = delete;
    CentralProcessingUnit(RandomAccessMemory &);

    void start();
    void step();

    ControlUnit & get_control_unit();
    ArithemeticLogicUnit & get_arithemetic_logic_unit();
    FloatingPointUnit & get_floating_point_unit();
    VectorUnit & get_vector_unit();
    std::map<register_code, register_ptr> const & get_registers() const;

    friend Loader;
};
