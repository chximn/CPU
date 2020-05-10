#include "cu.hh"

#include <iostream>

ControlUnit::ControlUnit(std::map<register_code, register_ptr> & r, ArithemeticLogicUnit & a, FloatingPointUnit & f, VectorUnit & s, RandomAccessMemory & mem):
    instruction_pointer_register(std::make_shared<FullRegister>(register_code::rip)),
    instruction_register(std::make_shared<FullRegister>(register_code::ir)),
    immediate_register(std::make_shared<FullRegister>(register_code::_immediate)),
    halt(false),
    registers(r),
    alu(a),
    fpu(f),
    sse(s),
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
        load_from_memory = true;
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
    load_from_memory = false;
    write_to_memory = false;
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

            auto pointer = std::dynamic_pointer_cast<MemoryOperand>(operands.at(0));
            if (pointer != nullptr) {
                load_from_memory = false;
            }

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

        case instruction_code::_div: {
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

        case instruction_code::neg: {
            alu.operation = alu_operation::neg;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_destination(operands.at(0));
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

        case instruction_code::cmp: {
            alu.operation = alu_operation::cmp;
            load_from_memory = false;
            write_to_memory = false;
            evaluate_destination(operands.at(0));
            evaluate_source(operands.at(1));
            alu.size = instruction.get_size();
            execute_alu = true;
            break;
        }

        case instruction_code::shl: {
            alu.operation = alu_operation::shl;
            load_from_memory = false;
            write_to_memory = false;

            evaluate_destination(operands.at(0));

            if (operands.size() == 2) evaluate_source(operands.at(1));
            else {
                alu.source = immediate_register;
                immediate_register->set_value(1);
            }

            alu.size = instruction.get_size();
            execute_alu = true;

            break;
        }

        case instruction_code::shr: {
            alu.operation = alu_operation::shl;
            load_from_memory = false;
            write_to_memory = false;

            evaluate_destination(operands.at(0));

            if (operands.size() == 2) evaluate_source(operands.at(1));
            else {
                alu.source = immediate_register;
                immediate_register->set_value(1);
            }

            alu.size = instruction.get_size();
            execute_alu = true;

            break;
        }

        case instruction_code::jmp: {
            load_from_memory = false;
            write_to_memory = false;

            auto immediate_operand_pointer = std::dynamic_pointer_cast<ImmediateOperand>(operands.at(0));
            if (immediate_operand_pointer != nullptr) {
                alu.operation = alu_operation::add;

            }

            else alu.operation = alu_operation::mov;

            evaluate_source(operands.at(0));
            alu.destination = instruction_pointer_register;
            alu.size = 64;
            execute_alu = true;
            break;
        }

        case instruction_code::je: {
            load_from_memory = false;
            write_to_memory = false;

            if (alu.flags[flag_code::zf]->get_value() != 1) {
                execute_alu = false;
                break;
            }

            auto immediate_operand_pointer = std::dynamic_pointer_cast<ImmediateOperand>(operands.at(0));
            if (immediate_operand_pointer != nullptr) {
                alu.operation = alu_operation::add;
            }

            else alu.operation = alu_operation::mov;

            evaluate_source(operands.at(0));
            alu.destination = instruction_pointer_register;
            alu.size = 64;
            execute_alu = true;
            break;
        }

        case instruction_code::jne: {
            load_from_memory = false;
            write_to_memory = false;

            if (alu.flags[flag_code::zf]->get_value() != 0) {
                execute_alu = false;
                break;
            }

            auto immediate_operand_pointer = std::dynamic_pointer_cast<ImmediateOperand>(operands.at(0));
            if (immediate_operand_pointer != nullptr) {
                alu.operation = alu_operation::add;
            }

            else alu.operation = alu_operation::mov;

            evaluate_source(operands.at(0));
            alu.destination = instruction_pointer_register;
            alu.size = 64;
            execute_alu = true;
            break;
        }

        case instruction_code::jg: {
            load_from_memory = false;
            write_to_memory = false;

            if (alu.flags[flag_code::sf]->get_value() != 0 && alu.flags[flag_code::zf]->get_value() != 0) {
                execute_alu = false;
                break;
            }

            auto immediate_operand_pointer = std::dynamic_pointer_cast<ImmediateOperand>(operands.at(0));
            if (immediate_operand_pointer != nullptr) {
                alu.operation = alu_operation::add;
            }

            else alu.operation = alu_operation::mov;

            evaluate_source(operands.at(0));
            alu.destination = instruction_pointer_register;
            alu.size = 64;
            execute_alu = true;
            break;
        }

        case instruction_code::jl: {
            load_from_memory = false;
            write_to_memory = false;

            if (alu.flags[flag_code::sf]->get_value() != 1 && alu.flags[flag_code::zf]->get_value() != 0) {
                execute_alu = false;
                break;
            }

            auto immediate_operand_pointer = std::dynamic_pointer_cast<ImmediateOperand>(operands.at(0));
            if (immediate_operand_pointer != nullptr) {
                alu.operation = alu_operation::add;
            }

            else alu.operation = alu_operation::mov;

            evaluate_source(operands.at(0));
            alu.destination = instruction_pointer_register;
            alu.size = 64;
            execute_alu = true;
            break;
        }

        case instruction_code::jge: {
            load_from_memory = false;
            write_to_memory = false;

            if (alu.flags[flag_code::zf]->get_value() == 1 || alu.flags[flag_code::sf]->get_value() == 0) {
                auto immediate_operand_pointer = std::dynamic_pointer_cast<ImmediateOperand>(operands.at(0));
                if (immediate_operand_pointer != nullptr) {
                    alu.operation = alu_operation::add;
                }

                else alu.operation = alu_operation::mov;

                evaluate_source(operands.at(0));
                alu.destination = instruction_pointer_register;
                alu.size = 64;
                execute_alu = true;
                break;
            }

            else {
                execute_alu = false;
            }

            break;
        }

        case instruction_code::jle: {
            load_from_memory = false;
            write_to_memory = false;

            if (alu.flags[flag_code::zf]->get_value() == 1 || alu.flags[flag_code::sf]->get_value() == 1) {
                auto immediate_operand_pointer = std::dynamic_pointer_cast<ImmediateOperand>(operands.at(0));
                if (immediate_operand_pointer != nullptr) {
                    alu.operation = alu_operation::add;
                }

                else alu.operation = alu_operation::mov;

                evaluate_source(operands.at(0));
                alu.destination = instruction_pointer_register;
                alu.size = 64;
                execute_alu = true;
            }

            else {
                execute_alu = false;
            }

            break;
        }

        case instruction_code::call: {
            load_from_memory = false;
            write_to_memory = false;

            auto immediate_operand_pointer = std::dynamic_pointer_cast<ImmediateOperand>(operands.at(0));
            if (immediate_operand_pointer != nullptr) {
                alu.operation = alu_operation::add;
            }

            else alu.operation = alu_operation::mov;

            evaluate_source(operands.at(0));
            alu.destination = instruction_pointer_register;
            alu.size = 64;
            ram.size = 64;
            ram.data_register->set_value(instruction_pointer_register->get_value());
            write_to_memory = true;
            execute_alu = true;
            break;
        }

        case instruction_code::ret: {
            load_from_memory = true;
            write_to_memory = false;
            ram.size = 64;

            alu.operation = alu_operation::mov;
            alu.destination = instruction_pointer_register;
            alu.source = ram.data_register;
            alu.size = 64;
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

        case instruction_code::fld:
            load_from_memory = true;
            write_to_memory = false;
            execute_fpu = true;

            ram.address_register->set_value(evaluate_address(*std::dynamic_pointer_cast<MemoryOperand>(operands.at(0))));
            fpu.operation = fpu_operation::fld;
            fpu.src_dest = ram.data_register;
            fpu.is_double = operands.at(0)->get_size() == 64;
            break;

        case instruction_code::fst:
            load_from_memory = true;
            write_to_memory = false;
            execute_fpu = true;
            fpu.perform_pop = false;

            ram.address_register->set_value(evaluate_address(*std::dynamic_pointer_cast<MemoryOperand>(operands.at(0))));
            fpu.operation = fpu_operation::fst;
            fpu.src_dest = ram.data_register;
            fpu.is_double = operands.at(0)->get_size() == 64;
            break;

        case instruction_code::fstp:
            load_from_memory = true;
            write_to_memory = false;
            execute_fpu = true;
            fpu.perform_pop = true;

            ram.address_register->set_value(evaluate_address(*std::dynamic_pointer_cast<MemoryOperand>(operands.at(0))));
            fpu.operation = fpu_operation::fst;
            fpu.src_dest = ram.data_register;
            fpu.is_double = operands.at(0)->get_size() == 64;
            break;

        case instruction_code::fldz:
            load_from_memory = true;
            write_to_memory = false;
            execute_fpu = true;

            immediate_register->set_value(0);
            fpu.operation = fpu_operation::fld;
            fpu.src_dest = immediate_register;
            fpu.is_double = true;
            break;

        case instruction_code::fld1:
            load_from_memory = true;
            write_to_memory = false;
            execute_fpu = true;

            immediate_register->set_value(0x3FF0000000000000);
            fpu.operation = fpu_operation::fld;
            fpu.src_dest = immediate_register;
            fpu.is_double = true;
            break;

        case instruction_code::_fadd:
            write_to_memory = false;
            execute_fpu = true;
            fpu.perform_pop = false;
            fpu.operation = fpu_operation::fadd;

            if (operands.size() == 1) {
                auto memory_operand_pointer = std::dynamic_pointer_cast<MemoryOperand>(operands.at(0));
                fpu.is_double = memory_operand_pointer->get_size() == 64;
                load_from_memory = true;
                ram.address_register->set_value(evaluate_address(*memory_operand_pointer));
                fpu.src_dest = ram.data_register;
                fpu.is_destination = false;
            }

            else if (operands.size() == 2) {
                auto op1 = std::dynamic_pointer_cast<RegisterOperand>(operands.at(0));
                auto op2 = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));

                if (op1->get_reg() == register_code::st0) {
                    fpu.is_destination = false;
                    fpu.src_dest = fpu.get_register(op2->get_reg());
                }

                else {
                    fpu.is_destination = true;
                    fpu.src_dest = fpu.get_register(op1->get_reg());
                }

                fpu.is_double = true;
                load_from_memory = false;
            }

            break;

        case instruction_code::faddp:
            write_to_memory = false;
            load_from_memory = false;
            execute_fpu = true;
            fpu.perform_pop = true;
            fpu.operation = fpu_operation::fadd;
            fpu.is_double = true;
            fpu.is_destination = true;

            if (operands.size() == 0) fpu.src_dest = fpu.stages[1];
            else if (operands.size() == 2) fpu.src_dest = fpu.get_register(std::dynamic_pointer_cast<RegisterOperand>(operands.at(0))->get_reg());
            break;

        case instruction_code::movdqu:
        case instruction_code::movdqa: {
            sse.operation = vector_operation::mov;
            load_from_memory = false;
            write_to_memory = false;
            sse.load_from_memory = false;
            sse.write_to_memory = false;

            auto op1mem = std::dynamic_pointer_cast<MemoryOperand>(operands.at(0));
            auto op1reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(0));
            if (op1mem != nullptr) {
                ram.address_register->set_value(evaluate_address(*op1mem));
                sse.destination = sse.temp_register;
                sse.write_to_memory = true;

                auto op2reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));
                sse.source = sse.get_register(op2reg->get_reg());
            }

            else {
                sse.destination = sse.get_register(op1reg->get_reg());

                auto op2mem = std::dynamic_pointer_cast<MemoryOperand>(operands.at(1));
                auto op2reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));

                if (op2mem != nullptr) {
                    sse.source = sse.temp_register;
                    sse.load_from_memory = true;
                    ram.address_register->set_value(evaluate_address(*op2mem));
                }

                else {
                    sse.operation = vector_operation::mov;
                    sse.source = sse.get_register(op2reg->get_reg());
                }

            }

            execute_sse = true;
            break;
        }

        case instruction_code::paddb: {
            sse.operation = vector_operation::paddb;
            sse.write_to_memory = false;

            auto op1reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(0));
            sse.destination = sse.get_register(op1reg->get_reg());
            sse.source      = sse.get_register(op1reg->get_reg());


            auto op2mem = std::dynamic_pointer_cast<MemoryOperand>(operands.at(1));
            auto op2reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));

            if (op2mem != nullptr) {
                sse.source2 = sse.temp_register;
                ram.address_register->set_value(evaluate_address(*op2mem));
                sse.load_from_memory = true;
            }

            else {
                sse.load_from_memory = false;
                sse.source2 = sse.get_register(op2reg->get_reg());
            }

            execute_sse = true;
            break;
        }

        case instruction_code::paddw: {
            sse.operation = vector_operation::paddw;
            sse.write_to_memory = false;

            auto op1reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(0));
            sse.destination = sse.get_register(op1reg->get_reg());
            sse.source      = sse.get_register(op1reg->get_reg());


            auto op2mem = std::dynamic_pointer_cast<MemoryOperand>(operands.at(1));
            auto op2reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));

            if (op2mem != nullptr) {
                sse.source2 = sse.temp_register;
                ram.address_register->set_value(evaluate_address(*op2mem));
                sse.load_from_memory = true;
            }

            else {
                sse.load_from_memory = false;
                sse.source2 = sse.get_register(op2reg->get_reg());
            }

            execute_sse = true;
            break;
        }

        case instruction_code::paddd: {
            sse.operation = vector_operation::paddd;
            sse.write_to_memory = false;

            auto op1reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(0));
            sse.destination = sse.get_register(op1reg->get_reg());
            sse.source      = sse.get_register(op1reg->get_reg());

            auto op2mem = std::dynamic_pointer_cast<MemoryOperand>(operands.at(1));
            auto op2reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));

            if (op2mem != nullptr) {
                sse.source2 = sse.temp_register;
                ram.address_register->set_value(evaluate_address(*op2mem));
                sse.load_from_memory = true;
            }

            else {
                sse.load_from_memory = false;
                sse.source2 = sse.get_register(op2reg->get_reg());
            }

            execute_sse = true;
            break;
        }

        case instruction_code::paddq: {
            sse.operation = vector_operation::paddq;
            sse.write_to_memory = false;

            auto op1reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(0));
            sse.destination = sse.get_register(op1reg->get_reg());
            sse.source      = sse.get_register(op1reg->get_reg());


            auto op2mem = std::dynamic_pointer_cast<MemoryOperand>(operands.at(1));
            auto op2reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));

            if (op2mem != nullptr) {
                sse.source2 = sse.temp_register;
                ram.address_register->set_value(evaluate_address(*op2mem));
                sse.load_from_memory = true;
            }

            else {
                sse.load_from_memory = false;
                sse.source2 = sse.get_register(op2reg->get_reg());
            }

            execute_sse = true;
            break;
        }

        case instruction_code::addps: {
            sse.operation = vector_operation::addps;
            sse.write_to_memory = false;

            auto op1reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(0));
            sse.destination = sse.get_register(op1reg->get_reg());
            sse.source      = sse.get_register(op1reg->get_reg());


            auto op2mem = std::dynamic_pointer_cast<MemoryOperand>(operands.at(1));
            auto op2reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));

            if (op2mem != nullptr) {
                sse.source2 = sse.temp_register;
                ram.address_register->set_value(evaluate_address(*op2mem));
                sse.load_from_memory = true;
            }

            else {
                sse.load_from_memory = false;
                sse.source2 = sse.get_register(op2reg->get_reg());
            }

            execute_sse = true;
            break;
        }

        case instruction_code::addpd: {
            sse.operation = vector_operation::addpd;
            sse.write_to_memory = false;

            auto op1reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(0));
            sse.destination = sse.get_register(op1reg->get_reg());
            sse.source      = sse.get_register(op1reg->get_reg());


            auto op2mem = std::dynamic_pointer_cast<MemoryOperand>(operands.at(1));
            auto op2reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));

            if (op2mem != nullptr) {
                sse.source2 = sse.temp_register;
                ram.address_register->set_value(evaluate_address(*op2mem));
                sse.load_from_memory = true;
            }

            else {
                sse.load_from_memory = false;
                sse.source2 = sse.get_register(op2reg->get_reg());
            }

            execute_sse = true;
            break;
        }

        case instruction_code::pand: {
            sse.operation = vector_operation::pand;
            sse.write_to_memory = false;

            auto op1reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(0));
            sse.destination = sse.get_register(op1reg->get_reg());
            sse.source      = sse.get_register(op1reg->get_reg());


            auto op2mem = std::dynamic_pointer_cast<MemoryOperand>(operands.at(1));
            auto op2reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));

            if (op2mem != nullptr) {
                sse.source2 = sse.temp_register;
                ram.address_register->set_value(evaluate_address(*op2mem));
                sse.load_from_memory = true;
            }

            else {
                sse.load_from_memory = false;
                sse.source2 = sse.get_register(op2reg->get_reg());
            }

            execute_sse = true;
            break;
        }

        case instruction_code::pshufd: {
            sse.operation = vector_operation::pshufd;
            sse.write_to_memory = false;

            auto op1reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(0));
            sse.destination = sse.get_register(op1reg->get_reg());

            auto op2mem = std::dynamic_pointer_cast<MemoryOperand>(operands.at(1));
            auto op2reg = std::dynamic_pointer_cast<RegisterOperand>(operands.at(1));

            if (op2mem != nullptr) {
                sse.source = sse.temp_register;
                ram.address_register->set_value(evaluate_address(*op2mem));
                sse.load_from_memory = true;
            }

            else {
                sse.load_from_memory = false;
                sse.source = sse.get_register(op2reg->get_reg());
            }

            auto value = std::dynamic_pointer_cast<ImmediateOperand>(operands.at(2))->get_value();
            sse.control_byte = *reinterpret_cast<uint8_t *>(&value);

            execute_sse = true;
            break;
        }

        default:
            throw "unknown or unimplemented instruction";
            break;
    }
}

void ControlUnit::load() {
    uint64_t instruction_pointer = (instruction_register->get_value());
    Instruction instruction = *reinterpret_cast<Instruction *>(instruction_pointer);

    if (instruction.get_code() == instruction_code::pop || instruction.get_code() == instruction_code::ret) {
        pop_temp_address = ram.address_register->get_value();
        ram.address_register->set_value(registers[register_code::rsp]->get_value() + registers[register_code::ss]->get_value());
    }

    else if (instruction.get_code() == instruction_code::push || instruction.get_code() == instruction_code::call)
        registers[register_code::rsp]->set_value(registers[register_code::rsp]->get_value() - instruction.get_size() / 8);

    if (load_from_memory) ram.load();

    if (instruction.get_code() == instruction_code::pop || instruction.get_code() == instruction_code::call) {
        registers[register_code::rsp]->set_value(registers[register_code::rsp]->get_value() + instruction.get_size() / 8);
        ram.address_register->set_value(pop_temp_address);
    }

    if (instruction.get_code() == instruction_code::push || instruction.get_code() == instruction_code::call)
        ram.address_register->set_value(registers[register_code::rsp]->get_value() + registers[register_code::ss]->get_value());
}

void ControlUnit::execute() {
    if (halt) return;
    if (execute_alu) alu.execute();
    else if (execute_fpu) fpu.execute();
    else if (execute_sse) sse.execute();
}

void ControlUnit::write() {
    if (write_to_memory) ram.write();
}
