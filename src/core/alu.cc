#include "alu.hh"

ArithemeticLogicUnit::ArithemeticLogicUnit(register_ptr flags_reg): flags_register(flags_reg) {
    flags[flag_code::cf] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 0);
    flags[flag_code::pf] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 2);
    flags[flag_code::zf] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 6);
    flags[flag_code::sf] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 7);
    flags[flag_code::of] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 11);
}

void ArithemeticLogicUnit::execute() {
    flags_register->set_value(0);
    bool update_flags = true;

    switch (operation) {
        case alu_operation::mov: {
            destination->set_value(source->get_value());
            break;
        }

        case alu_operation::add: {

            uint64_t result = 0;
            auto source_value = source->get_value();
            auto destination_value = destination->get_value();

            if (size == 8) {
                auto s = *reinterpret_cast<int8_t *>(&source_value);
                auto d = *reinterpret_cast<int8_t *>(&destination_value);
                auto r = s + d;
                result += r;
            }

            else if (size == 16) {
                auto s = *reinterpret_cast<int16_t *>(&source_value);
                auto d = *reinterpret_cast<int16_t *>(&destination_value);
                auto r = s + d;
                result += r;
            }

            else if (size == 32) {
                auto s = *reinterpret_cast<int32_t *>(&source_value);
                auto d = *reinterpret_cast<int32_t *>(&destination_value);
                auto r = s + d;
                result += r;
            }

            else if (size == 64) {
                auto s = *reinterpret_cast<int64_t *>(&source_value);
                auto d = *reinterpret_cast<int64_t *>(&destination_value);
                auto r = s + d;
                result += r;
            }

            else throw "invalid alu size modifier";

            destination->set_value(result);
            break;
        }

        case alu_operation::cmp: {
            uint64_t result = 0;
            auto source_value = source->get_value();
            auto destination_value = destination->get_value();

            if (size == 8) {
                auto s = *reinterpret_cast<int8_t *>(&source_value);
                auto d = *reinterpret_cast<int8_t *>(&destination_value);
                auto r = d - s;
                result += r;
            }

            else if (size == 16) {
                auto s = *reinterpret_cast<int16_t *>(&source_value);
                auto d = *reinterpret_cast<int16_t *>(&destination_value);
                auto r = d - s;
                result += r;
            }

            else if (size == 32) {
                auto s = *reinterpret_cast<int32_t *>(&source_value);
                auto d = *reinterpret_cast<int32_t *>(&destination_value);
                auto r = d - s;
                result += r;
            }

            else if (size == 64) {
                auto s = *reinterpret_cast<int64_t *>(&source_value);
                auto d = *reinterpret_cast<int64_t *>(&destination_value);
                auto r = d - s;
                result += r;
            }

            else throw "invalid alu size modifier";

            update_flags = false;
            if (result == 0) flags[flag_code::zf]->set_value(1);
            if ((result >> (size - 1)) & 1) flags[flag_code::sf]->set_value(1);
            if ((result & 1) == 0) flags[flag_code::pf]->set_value(1);
            break;
        }

        case alu_operation::sub: {
            uint64_t result = 0;
            auto source_value = source->get_value();
            auto destination_value = destination->get_value();

            if (size == 8) {
                auto s = *reinterpret_cast<int8_t *>(&source_value);
                auto d = *reinterpret_cast<int8_t *>(&destination_value);
                auto r = d - s;
                result += r;
            }

            else if (size == 16) {
                auto s = *reinterpret_cast<int16_t *>(&source_value);
                auto d = *reinterpret_cast<int16_t *>(&destination_value);
                auto r = d - s;
                result += r;
            }

            else if (size == 32) {
                auto s = *reinterpret_cast<int32_t *>(&source_value);
                auto d = *reinterpret_cast<int32_t *>(&destination_value);
                auto r = d - s;
                result += r;
            }

            else if (size == 64) {
                auto s = *reinterpret_cast<int64_t *>(&source_value);
                auto d = *reinterpret_cast<int64_t *>(&destination_value);
                auto r = d - s;
                result += r;
            }

            else throw "invalid alu size modifier";

            destination->set_value(result);
            break;
        }

        case alu_operation::mul: {
            uint64_t r, c;
            auto a = source->get_value();
            auto b = destination->get_value();
            helpers::multiply(a, b, r, c, size);

            destination->set_value(r);
            extra->set_value(c);

            break;
        }

        case alu_operation::div: {
            uint64_t q, r;
            helpers::multiply(destination->get_value(), source->get_value(), q, r, size);
            destination->set_value(q);
            extra->set_value(r);
            break;
        }

        case alu_operation::neg: {
            uint64_t value = destination->get_value();
            uint64_t mask = (~static_cast<uint64_t>(0) >> (64 - size));
            destination->set_value((~value & mask) + 1);
            break;
        }

        case alu_operation::_and: {
            destination->set_value(source->get_value() & destination->get_value());
            break;
        }

        case alu_operation::_or: {
            destination->set_value(source->get_value() | destination->get_value());
            break;
        }

        case alu_operation::_xor: {
            destination->set_value(source->get_value() ^ destination->get_value());
            break;
        }

        case alu_operation::_not: {
            destination->set_value( ~(destination->get_value()) );
            break;
        }

        case alu_operation::shl: {
            destination->set_value( (destination->get_value()) << (source->get_value()) );
            break;
        }

        case alu_operation::shr: {
            destination->set_value( (destination->get_value()) >> (source->get_value()) );
            break;
        }

        default:

            break;
    }

    if (update_flags) {
        if (destination->get_value() == 0) flags[flag_code::zf]->set_value(1);
        if ((destination->get_value() >> (size - 1)) & 1) flags[flag_code::sf]->set_value(1);
        if ((destination->get_value() & 1) == 0) flags[flag_code::pf]->set_value(1);
    }
}
