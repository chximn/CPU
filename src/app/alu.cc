#include "alu.hh"

ArithemeticLogicUnit::ArithemeticLogicUnit(register_ptr flags_reg): flags_register(flags_reg) {
    flags[flag_code::cf] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 0);
    flags[flag_code::pf] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 2);
    flags[flag_code::af] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 4);
    flags[flag_code::zf] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 6);
    flags[flag_code::sf] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 7);
    flags[flag_code::df] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 10);
    flags[flag_code::of] = std::make_shared<PartRegister>(register_code::flags, flags_reg, 1, 11);
}

void ArithemeticLogicUnit::execute() {
    flags_register->set_value(0);

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

        default:

            break;
    }

    if (destination->get_value() == 0) flags[flag_code::zf]->set_value(1);

}
