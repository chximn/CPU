#include "loader.hh"

#include <iostream>

Loader::Loader(CentralProcessingUnit & c, RandomAccessMemory & r):
    cpu(c), ram(r) {}

void Loader::load(Program const & program) {
    uint64_t address;

    // load code
    ram.size = 64;
    address = LOADER_DEFAULT_CODE_SEGMENT;
    for (auto const & i : program.get_instructions()) {
        uint64_t pointer = reinterpret_cast<uint64_t>(const_cast<void*>(reinterpret_cast<void const *>(i.get())));
        ram.data_register->set_value(pointer);
        ram.address_register->set_value(address);
        ram.write();
        address += 8;
    }

    // load data
    ram.size = 8;
    address = LOADER_DEFAULT_DATA_SEGMENT;
    for (auto const & b : program.get_data()) {

        ram.data_register->set_value(b);
        ram.address_register->set_value(address);
        ram.write();

        address += 1;
    }

    // set cpu segment registers
    cpu.registers[register_code::cs]->set_value(LOADER_DEFAULT_CODE_SEGMENT);
    cpu.registers[register_code::ds]->set_value(LOADER_DEFAULT_DATA_SEGMENT);
    cpu.registers[register_code::ss]->set_value(LOADER_DEFAULT_STACK_SEGMENT);
    cpu.registers[register_code::rsp]->set_value(LOADER_DEFAULT_STACK_SIZE);
    cpu.get_control_unit().get_instruction_pointer_register()->set_value(0);

}
