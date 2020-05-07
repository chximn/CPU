#include "sse.hh"

#include <iostream>

VectorUnit::VectorUnit(RandomAccessMemory & r): ram(r) {
    temp_register = std::make_shared<VectorRegister>(register_code::_temp_sse);
    registers[0] = std::make_shared<VectorRegister>(register_code::xmm0);
    registers[1] = std::make_shared<VectorRegister>(register_code::xmm1);
    registers[2] = std::make_shared<VectorRegister>(register_code::xmm2);
    registers[3] = std::make_shared<VectorRegister>(register_code::xmm3);
    registers[4] = std::make_shared<VectorRegister>(register_code::xmm4);
    registers[5] = std::make_shared<VectorRegister>(register_code::xmm5);
    registers[6] = std::make_shared<VectorRegister>(register_code::xmm6);
    registers[7] = std::make_shared<VectorRegister>(register_code::xmm7);
}

vector_register_ptr VectorUnit::get_register(register_code code) {
    switch (code) {
        case register_code::xmm0: return registers[0];
        case register_code::xmm1: return registers[1];
        case register_code::xmm2: return registers[2];
        case register_code::xmm3: return registers[3];
        case register_code::xmm4: return registers[4];
        case register_code::xmm5: return registers[5];
        case register_code::xmm6: return registers[6];
        case register_code::xmm7: return registers[7];
        default: throw "provided register code does not belong to the vector unit";
    }
}

void VectorUnit::execute() {
    if (load_from_memory) {
        // load low half
        ram.size = 64;
        ram.load();
        temp_register->set_low(ram.data_register->get_value());

        // load high half
        ram.address_register->set_value(ram.address_register->get_value() + 8);
        ram.load();
        temp_register->set_high(ram.data_register->get_value());
    }

    switch (operation) {

        case vector_operation::mov: {
            destination->set_low(source->get_low());
            destination->set_high(source->get_high());
            break;
        }

        case vector_operation::paddb: {
            auto s1 = _mm_loadu_si128(source->value_dq());
            auto s2 = _mm_loadu_si128(source2->value_dq());
            _mm_storeu_si128(destination->value_dq(), _mm_add_epi8(s1, s2));
            break;
        }

        case vector_operation::paddw: {
            auto s1 = _mm_loadu_si128(source->value_dq());
            auto s2 = _mm_loadu_si128(source2->value_dq());
            _mm_storeu_si128(destination->value_dq(), _mm_add_epi16(s1, s2));
            break;
        }

        case vector_operation::paddd: {
            auto s1 = _mm_loadu_si128(source->value_dq());
            auto s2 = _mm_loadu_si128(source2->value_dq());
            _mm_storeu_si128(destination->value_dq(), _mm_add_epi32(s1, s2));
            break;
        }

        case vector_operation::paddq: {
            auto s1 = _mm_loadu_si128(source->value_dq());
            auto s2 = _mm_loadu_si128(source2->value_dq());
            _mm_storeu_si128(destination->value_dq(), _mm_add_epi64(s1, s2));
            break;
        }

        case vector_operation::addps: {
            auto s1 = _mm_load_ps(source->value_float());
            auto s2 = _mm_load_ps(source2->value_float());
            _mm_store_ps(destination->value_float(), _mm_add_ps(s1, s2));
            break;
        }

        case vector_operation::addpd: {
            auto s1 = _mm_load_ps(source->value_float());
            auto s2 = _mm_load_ps(source2->value_float());
            _mm_store_ps(destination->value_float(), _mm_add_ps(s1, s2));
            break;
        }

        default:
            throw "unknown or unimplemented vector operation";
            break;
    }

    if (write_to_memory) {
        // write low half
        ram.size = 64;
        ram.data_register->set_value(temp_register->get_low());
        ram.write();

        // write high half
        ram.address_register->set_value(ram.address_register->get_value() + 8);
        ram.data_register->set_value(temp_register->get_high());
        ram.write();
    }
}
