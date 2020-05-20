#include "cpu.hh"

CentralProcessingUnit::CentralProcessingUnit(RandomAccessMemory & r):
    ram(r),
    flags(std::make_shared<FullRegister>(register_code::rflags)),
    arithmetic_logic_unit(flags),
    vector_unit(ram),
    control_unit(registers,
                 arithmetic_logic_unit,
                 floating_point_unit,
                 vector_unit,
                 ram) {
    // clang-format off
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

    registers[register_code::rcx] = std::make_shared<FullRegister>(register_code::rcx);
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
    registers[register_code::ss] = std::make_shared<FullRegister>(register_code::ss);

    registers[register_code::r8]  = std::make_shared<FullRegister>(register_code::r8);
    registers[register_code::r8d] = std::make_shared<PartRegister>(register_code::r8d, registers[register_code::r8], 32, 0);
    registers[register_code::r8w] = std::make_shared<PartRegister>(register_code::r8w, registers[register_code::r8], 16, 0);
    registers[register_code::r8b] = std::make_shared<PartRegister>(register_code::r8b, registers[register_code::r8], 8,  0);

    registers[register_code::r9]  = std::make_shared<FullRegister>(register_code::r9);
    registers[register_code::r9d] = std::make_shared<PartRegister>(register_code::r9d, registers[register_code::r9], 32, 0);
    registers[register_code::r9w] = std::make_shared<PartRegister>(register_code::r9w, registers[register_code::r9], 16, 0);
    registers[register_code::r9b] = std::make_shared<PartRegister>(register_code::r9b, registers[register_code::r9], 8,  0);

    registers[register_code::r10]  = std::make_shared<FullRegister>(register_code::r10);
    registers[register_code::r10d] = std::make_shared<PartRegister>(register_code::r10d, registers[register_code::r10], 32, 0);
    registers[register_code::r10w] = std::make_shared<PartRegister>(register_code::r10w, registers[register_code::r10], 16, 0);
    registers[register_code::r10b] = std::make_shared<PartRegister>(register_code::r10b, registers[register_code::r10], 8,  0);

    registers[register_code::r11]  = std::make_shared<FullRegister>(register_code::r11);
    registers[register_code::r11d] = std::make_shared<PartRegister>(register_code::r11d, registers[register_code::r11], 32, 0);
    registers[register_code::r11w] = std::make_shared<PartRegister>(register_code::r11w, registers[register_code::r11], 16, 0);
    registers[register_code::r11b] = std::make_shared<PartRegister>(register_code::r11b, registers[register_code::r11], 8,  0);

    registers[register_code::r12]  = std::make_shared<FullRegister>(register_code::r12);
    registers[register_code::r12d] = std::make_shared<PartRegister>(register_code::r12d, registers[register_code::r12], 32, 0);
    registers[register_code::r12w] = std::make_shared<PartRegister>(register_code::r12w, registers[register_code::r12], 16, 0);
    registers[register_code::r12b] = std::make_shared<PartRegister>(register_code::r12b, registers[register_code::r12], 8,  0);

    registers[register_code::r13]  = std::make_shared<FullRegister>(register_code::r13);
    registers[register_code::r13d] = std::make_shared<PartRegister>(register_code::r13d, registers[register_code::r13], 32, 0);
    registers[register_code::r13w] = std::make_shared<PartRegister>(register_code::r13w, registers[register_code::r13], 16, 0);
    registers[register_code::r13b] = std::make_shared<PartRegister>(register_code::r13b, registers[register_code::r13], 8,  0);

    registers[register_code::r14]  = std::make_shared<FullRegister>(register_code::r14);
    registers[register_code::r14d] = std::make_shared<PartRegister>(register_code::r14d, registers[register_code::r14], 32, 0);
    registers[register_code::r14w] = std::make_shared<PartRegister>(register_code::r14w, registers[register_code::r14], 16, 0);
    registers[register_code::r14b] = std::make_shared<PartRegister>(register_code::r14b, registers[register_code::r14], 8,  0);

    registers[register_code::r15]  = std::make_shared<FullRegister>(register_code::r15);
    registers[register_code::r15d] = std::make_shared<PartRegister>(register_code::r15d, registers[register_code::r15], 32, 0);
    registers[register_code::r15w] = std::make_shared<PartRegister>(register_code::r15w, registers[register_code::r15], 16, 0);
    registers[register_code::r15b] = std::make_shared<PartRegister>(register_code::r15b, registers[register_code::r15], 8,  0);

    registers[register_code::rdi] = std::make_shared<FullRegister>(register_code::rdi);
    registers[register_code::edi] = std::make_shared<PartRegister>(register_code::edi, registers[register_code::rdi], 32, 0);
    registers[register_code::di]  = std::make_shared<PartRegister>(register_code::di,  registers[register_code::rdi], 16, 0);

    registers[register_code::rsi] = std::make_shared<FullRegister>(register_code::rsi);
    registers[register_code::esi] = std::make_shared<PartRegister>(register_code::esi, registers[register_code::rsi], 32, 0);
    registers[register_code::si]  = std::make_shared<PartRegister>(register_code::si,  registers[register_code::rsi], 16, 0);

    registers[register_code::rsp] = std::make_shared<FullRegister>(register_code::rsp);
    registers[register_code::esp] = std::make_shared<PartRegister>(register_code::esp, registers[register_code::rsp], 32, 0);
    registers[register_code::sp]  = std::make_shared<PartRegister>(register_code::sp,  registers[register_code::rsp], 16, 0);

    registers[register_code::rbp] = std::make_shared<FullRegister>(register_code::rbp);
    registers[register_code::ebp] = std::make_shared<PartRegister>(register_code::ebp, registers[register_code::rbp], 32, 0);
    registers[register_code::bp]  = std::make_shared<PartRegister>(register_code::bp,  registers[register_code::rbp], 16, 0);

    registers[register_code::rflags] = flags;
    registers[register_code::eflags] = std::make_shared<PartRegister>(register_code::eflags, registers[register_code::rflags], 32, 0);
    registers[register_code::flags]  = std::make_shared<PartRegister>(register_code::flags,  registers[register_code::rflags], 16, 0);
    // clang-format on
}

void CentralProcessingUnit::start() {
    control_unit.instruction_pointer_register->set_value(0);

    while (!control_unit.halt) {
        step();
    }
}

void CentralProcessingUnit::step() {
    control_unit.fetch(registers[register_code::cs]);
    control_unit.decode();
    control_unit.load();
    control_unit.execute();
    control_unit.write();
}

ArithemeticLogicUnit & CentralProcessingUnit::get_arithemetic_logic_unit() {
    return arithmetic_logic_unit;
}

ControlUnit & CentralProcessingUnit::get_control_unit() {
    return control_unit;
}

FloatingPointUnit & CentralProcessingUnit::get_floating_point_unit() {
    return floating_point_unit;
}

VectorUnit & CentralProcessingUnit::get_vector_unit() {
    return vector_unit;
}

std::map<register_code, register_ptr> const &
CentralProcessingUnit::get_registers() const {
    return registers;
}
