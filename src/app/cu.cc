#include "cu.hh"

ControlUnit::ControlUnit(std::map<register_code, register_ptr> & r, ArithemeticLogicUnit & a, RandomAccessMemory & mem):
    instruction_pointer_register(std::make_shared<FullRegister>(register_code::rip)),
    instruction_register(std::make_shared<FullRegister>(register_code::ir)),
    immediate_register(std::make_shared<FullRegister>(register_code::_immediate)),
    registers(r),
    alu(a),
    ram(mem) {}

register_ptr ControlUnit::get_instruction_pointer_register() const {
    return instruction_pointer_register;
}

register_ptr ControlUnit::get_instruction_register() const {
    return instruction_register;
}

bool ControlUnit::get_load_from_memory() const {
    return load_from_memory;
}

bool ControlUnit::get_write_to_memory() const {
    return write_to_memory;
}


uint64_t ControlUnit::evaluate_address(MemoryOperand operand)  {
    uint64_t base  = registers[operand.get_base()]->get_value();
    uint64_t index = registers[operand.get_index()]->get_value();
    uint8_t  scale = operand.get_scale();
    uint64_t displacement = operand.get_displacement();

    return base + index * scale + displacement;
}

void ControlUnit::fetch() {
    ram.address_register->set_value(instruction_pointer_register->get_value());
    ram.load();
    instruction_register->set_value(ram.data_register->get_value());
    instruction_pointer_register->set_value(instruction_pointer_register->get_value() + 4);
}

void ControlUnit::evaluate_destination(operand_ptr operand) {
    auto register_operand_pointer  = std::dynamic_pointer_cast<RegisterOperand> (operand);
    auto memory_operand_pointer    = std::dynamic_pointer_cast<MemoryOperand>   (operand);
    auto immediate_operand_pointer = std::dynamic_pointer_cast<ImmediateOperand>(operand);

    if (register_operand_pointer != nullptr) {
        alu.destination = registers[register_operand_pointer->get_reg()];
    }

    else if (memory_operand_pointer != nullptr) {
        ram.address_register->set_value(evaluate_address(*memory_operand_pointer));
        alu.destination = ram.data_register;
        write_to_memory = true;
    }

    else if (immediate_operand_pointer != nullptr) {
        // TODO: throw exception
    }
}

void ControlUnit::evaluate_source(operand_ptr operand) {
    auto register_operand_pointer  = std::dynamic_pointer_cast<RegisterOperand> (operand);
    auto memory_operand_pointer    = std::dynamic_pointer_cast<MemoryOperand>   (operand);
    auto immediate_operand_pointer = std::dynamic_pointer_cast<ImmediateOperand>(operand);

    if (register_operand_pointer != nullptr) {
        alu.source = registers[register_operand_pointer->get_reg()];
    }

    else if (memory_operand_pointer != nullptr) {
        ram.address_register->set_value(evaluate_address(*memory_operand_pointer));
        alu.source = ram.data_register;
        load_from_memory = true;
    }

    else if (immediate_operand_pointer != nullptr) {
        immediate_register->set_value(immediate_operand_pointer->get_value());
        alu.source = immediate_register;
    }
}

void ControlUnit::decode() {
    uint64_t instruction_pointer = (instruction_register->get_value());
    Instruction instruction = *reinterpret_cast<Instruction *>(instruction_pointer);

    auto code = instruction.get_code();
    auto operands = instruction.get_operands();

    switch (code) {
        case instruction_code::mov: {
            alu.operation = alu_operation::mov;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_destination(operands.at(0));
            evaluate_source(operands.at(1));
            break;
        }

        // case instruction_code::add:
        //
        //     break;

        default:
            // TODO: throw exception
            break;
    }
}

void ControlUnit::load() {
    if (load_from_memory) ram.load();
}

void ControlUnit::execute() {
    alu.execute();
}

void ControlUnit::write() {
    if (write_to_memory) ram.write();
}
