#include "cu.hh"

ControlUnit::ControlUnit(std::map<register_code, register_ptr> & r, ArithemeticLogicUnit & a, RandomAccessMemory & mem):
    instruction_pointer_register(std::make_shared<FullRegister>(register_code::rip)),
    instruction_register(std::make_shared<FullRegister>(register_code::ir)),
    immediate_register(std::make_shared<FullRegister>(register_code::_immediate)),
    halt(false),
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
    uint64_t address = operand.get_displacement();

    if (operand.get_use_base()) {
        address += registers[operand.get_base()]->get_value();;
    }

    if (operand.get_use_index()) {
        uint64_t index = registers[operand.get_index()]->get_value();
        uint8_t  scale = operand.get_scale();

        address += index * scale;
    }

    if (operand.get_use_segment()) {
        address += registers[operand.get_segment()]->get_value();
    }

    return address;
}

void ControlUnit::fetch(register_ptr cs) {
    ram.address_register->set_value(instruction_pointer_register->get_value() + cs->get_value());
    ram.load();
    instruction_register->set_value(ram.data_register->get_value());
    instruction_pointer_register->set_value(instruction_pointer_register->get_value() + 8);
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
        ram.size = memory_operand_pointer->get_size();
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
        ram.size = memory_operand_pointer->get_size();
        alu.source = ram.data_register;
        load_from_memory = true;
    }

    else if (immediate_operand_pointer != nullptr) {
        immediate_register->set_value(immediate_operand_pointer->get_value());
        alu.source = immediate_register;
    }
}

void ControlUnit::decode() {
    execute_alu = false;
    execute_sse = false;
    execute_fpu = false;
    halt = false;

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
            execute_alu = true;
            alu.size = instruction.get_size();
            break;
        }

        case instruction_code::lea: {
            alu.operation = alu_operation::mov;
            load_from_memory = false;
            write_to_memory = false;
            ram.address_register->set_value(evaluate_address(*std::dynamic_pointer_cast<MemoryOperand>(operands.at(1))));
            alu.source = ram.address_register;
            alu.destination = registers[std::dynamic_pointer_cast<RegisterOperand>(operands.at(0))->get_reg()];
            execute_alu = true;
            break;
        }

        case instruction_code::push: {
            alu.operation = alu_operation::mov;
            load_from_memory = false;
            evaluate_source(operands.at(0));
            alu.destination = ram.data_register;
            write_to_memory = true;
            execute_alu = true;
            alu.size = instruction.get_size();
            ram.size = instruction.get_size();
            break;
        }

        case instruction_code::pop: {
            alu.operation = alu_operation::mov;
            write_to_memory = false;
            evaluate_destination(operands.at(0));
            alu.source = ram.data_register;
            load_from_memory = true;
            execute_alu = true;
            alu.size = instruction.get_size();
            ram.size = instruction.get_size();
            break;
        }

        case instruction_code::add: {
            alu.operation = alu_operation::add;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_destination(operands.at(0));
            evaluate_source(operands.at(1));
            alu.size = instruction.get_size();
            execute_alu = true;
            break;
        }

        case instruction_code::sub: {
            alu.operation = alu_operation::sub;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_destination(operands.at(0));
            evaluate_source(operands.at(1));
            alu.size = instruction.get_size();
            execute_alu = true;
            break;
        }

        case instruction_code::mul: {
            alu.operation = alu_operation::mul;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_source(operands.at(0));
            alu.destination = registers[register_code::rax];
            alu.extra = registers[register_code::rdx];
            alu.size = instruction.get_size();
            execute_alu = true;
            break;
        }

        case instruction_code::div: {
            alu.operation = alu_operation::div;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_source(operands.at(0));

            if (instruction.get_size() == 8) {
                alu.destination = registers[register_code::al];
                alu.extra = registers[register_code::ah];
            }

            else {
                alu.destination = registers[register_code::rax];
                alu.extra = registers[register_code::rdx];
            }

            alu.size = instruction.get_size();
            execute_alu = true;
            break;
        }

        case instruction_code::_and: {
            alu.operation = alu_operation::_and;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_destination(operands.at(0));
            evaluate_source(operands.at(1));
            alu.size = instruction.get_size();
            execute_alu = true;
            break;
        }

        case instruction_code::_or: {
            alu.operation = alu_operation::_or;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_destination(operands.at(0));
            evaluate_source(operands.at(1));
            alu.size = instruction.get_size();
            execute_alu = true;
            break;
        }

        case instruction_code::_xor: {
            alu.operation = alu_operation::_xor;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_destination(operands.at(0));
            evaluate_source(operands.at(1));
            alu.size = instruction.get_size();
            execute_alu = true;
            break;
        }

        case instruction_code::_not: {
            alu.operation = alu_operation::_not;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_destination(operands.at(0));
            evaluate_source(operands.at(0));
            alu.size = instruction.get_size();
            execute_alu = true;
            break;
        }

        case instruction_code::nop:
            load_from_memory = false;
            write_to_memory = false;
            execute_alu = false;
            break;

        case instruction_code::hlt:
            halt = true;
            load_from_memory = false;
            write_to_memory = false;
            break;

        default:
            throw "unknown or unimplemented instruction";
            break;
    }
}

void ControlUnit::load() {
    uint64_t instruction_pointer = (instruction_register->get_value());
    Instruction instruction = *reinterpret_cast<Instruction *>(instruction_pointer);

    if (instruction.get_code() == instruction_code::pop) {
        pop_temp_address = ram.address_register->get_value();
        ram.address_register->set_value(registers[register_code::rsp]->get_value() + registers[register_code::ss]->get_value());
    }

    else if (instruction.get_code() == instruction_code::push) registers[register_code::rsp]->set_value(registers[register_code::rsp]->get_value() - instruction.get_size() / 8);

    if (load_from_memory) ram.load();

    if (instruction.get_code() == instruction_code::pop) {
        registers[register_code::rsp]->set_value(registers[register_code::rsp]->get_value() + instruction.get_size() / 8);
        ram.address_register->set_value(pop_temp_address);
    }

    if (instruction.get_code() == instruction_code::push) ram.address_register->set_value(registers[register_code::rsp]->get_value() + registers[register_code::ss]->get_value());
}

void ControlUnit::execute() {
    if (halt) return;
    if (execute_alu) alu.execute();
}

void ControlUnit::write() {
    if (write_to_memory) ram.write();
}
