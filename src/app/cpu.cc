#include "cpu.hh"

CentralProcessingUnit::CentralProcessingUnit(RandomAccessMemory & r):
    ram(r),
    flags(std::make_shared<FullRegister>(register_code::rflags)),
    arithmetic_logic_unit(flags),
    vector_unit(ram),
    control_unit(registers, arithmetic_logic_unit, ram) {


    registers[register_code::rax] = std::make_shared<FullRegister>(register_code::rax);
    registers[register_code::eax] = std::make_shared<PartRegister>(register_code::eax, registers[register_code::rax], 32, 0);
    registers[register_code::ax]  = std::make_shared<PartRegister>(register_code::ax,  registers[register_code::rax], 16, 0);
    registers[register_code::al]  = std::make_shared<PartRegister>(register_code::al,  registers[register_code::rax], 8,  0);
    registers[register_code::ah]  = std::make_shared<PartRegister>(register_code::ah,  registers[register_code::rax], 8,  8);

    registers[register_code::rbx] = std::make_shared<FullRegister>(register_code::rbx);
    registers[register_code::ebx] = std::make_shared<PartRegister>(register_code::ebx,  registers[register_code::rbx], 32, 0);
    registers[register_code::bx]  = std::make_shared<PartRegister>(register_code::bx,  registers[register_code::rbx], 16, 0);
    registers[register_code::bl]  = std::make_shared<PartRegister>(register_code::bl,  registers[register_code::rbx], 8,  0);
    registers[register_code::bh]  = std::make_shared<PartRegister>(register_code::bh,  registers[register_code::rbx], 8,  8);

    registers[register_code::rcx] = std::make_shared<FullRegister>(register_code::rax);
    registers[register_code::ecx] = std::make_shared<PartRegister>(register_code::ecx, registers[register_code::rcx], 32, 0);
    registers[register_code::cx]  = std::make_shared<PartRegister>(register_code::cx,  registers[register_code::rcx], 16, 0);
    registers[register_code::cl]  = std::make_shared<PartRegister>(register_code::cl,  registers[register_code::rcx], 8,  0);
    registers[register_code::ch]  = std::make_shared<PartRegister>(register_code::ch,  registers[register_code::rcx], 8,  8);

    registers[register_code::rdx] = std::make_shared<FullRegister>(register_code::rdx);
    registers[register_code::edx] = std::make_shared<PartRegister>(register_code::edx, registers[register_code::rdx], 32, 0);
    registers[register_code::dx]  = std::make_shared<PartRegister>(register_code::dx,  registers[register_code::rdx], 16, 0);
    registers[register_code::dl]  = std::make_shared<PartRegister>(register_code::dl,  registers[register_code::rdx], 8,  0);
    registers[register_code::dh]  = std::make_shared<PartRegister>(register_code::dh,  registers[register_code::rdx], 8,  8);

    registers[register_code::cs] = std::make_shared<FullRegister>(register_code::cs);
    registers[register_code::ds] = std::make_shared<FullRegister>(register_code::ds);

    registers[register_code::eflags] = std::make_shared<PartRegister>(register_code::eflags, registers[register_code::rflags], 32, 0);
    registers[register_code::flags]  = std::make_shared<PartRegister>(register_code::flags,  registers[register_code::rflags], 16, 0);
}

void CentralProcessingUnit::start() {
    control_unit.instruction_pointer_register->set_value(0);

    while (!control_unit.halt) {
        control_unit.fetch(registers[register_code::cs]);
        control_unit.decode();
        control_unit.load();
        control_unit.execute();
        control_unit.write();
    }
}

ArithemeticLogicUnit & CentralProcessingUnit::get_arithemetic_logic_unit() {
    return arithmetic_logic_unit;
}

ControlUnit & CentralProcessingUnit::get_control_unit() {
    return control_unit;
}

std::map<register_code, register_ptr> const & CentralProcessingUnit::get_registers() const {
    return registers;
}
