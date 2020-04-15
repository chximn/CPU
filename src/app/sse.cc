#include "sse.hh"

#include <iostream>

VectorUnit::VectorUnit(RandomAccessMemory & r): ram(r) {
    registers[0] = std::make_shared<VectorRegister>(register_code::xmm0);
    registers[1] = std::make_shared<VectorRegister>(register_code::xmm1);
    registers[2] = std::make_shared<VectorRegister>(register_code::xmm2);
    registers[3] = std::make_shared<VectorRegister>(register_code::xmm3);
    registers[4] = std::make_shared<VectorRegister>(register_code::xmm4);
    registers[5] = std::make_shared<VectorRegister>(register_code::xmm5);
    registers[6] = std::make_shared<VectorRegister>(register_code::xmm6);
    registers[7] = std::make_shared<VectorRegister>(register_code::xmm7);
}



void VectorUnit::execute() {
    switch (operation) {
        case vector_operation::load: {
            // load low half
            ram.size = 64;
            ram.load();
            destination->set_low(ram.data_register->get_value());

            // load high half
            ram.address_register->set_value(ram.address_register->get_value() + 8);
            ram.load();
            destination->set_high(ram.data_register->get_value());

            break;
        }

        case vector_operation::write: {
            // write low half
            ram.size = 64;
            ram.data_register->set_value(source->get_low());
            ram.write();

            // write high half
            ram.address_register->set_value(ram.address_register->get_value() + 8);
            ram.data_register->set_value(source->get_high());
            ram.write();
            break;
        }

        case vector_operation::add: {
            if (operation_type == vector_operation_type::_float) {
                auto s1 = _mm_load_ps(source->value_float());
                auto s2 = _mm_load_ps(source2->value_float());
                _mm_store_ps(destination->value_float(), _mm_add_ps(s1, s2));
            }
            break;
        }

        default:
            throw "unknown or unimplemented vector operation";
            break;
    }
}
