#pragma once
#include <map>
#include "register.hh"
#include "alu.hh"
#include "cu.hh"
#include "ram.hh"

class CentralProcessingUnit {
private:
    RandomAccessMemory & ram;

    std::map<register_code, register_ptr> registers;
    ArithemeticLogicUnit arithmetic_logic_unit;
    // FloatingPointerUnit floating_point_unit;
    // VectorUnit vector_unit;
    ControlUnit control_unit;

public:
    CentralProcessingUnit() = delete;
    CentralProcessingUnit(RandomAccessMemory &);

    void start();

    ControlUnit & get_control_unit();
    ArithemeticLogicUnit & get_arithemetic_logic_unit();
    std::map<register_code, register_ptr> const & get_registers() const;
    // friend Loader;
};
