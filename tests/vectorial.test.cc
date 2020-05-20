#include <catch.hpp>

#include "ram.hh"
#include "sse.hh"

TEST_CASE("vector register") {
    VectorRegister reg(register_code::xmm0);

    SECTION("low") {
        reg.set_low(0xabcd1234deadbeef);
        REQUIRE(reg.get_low() == 0xabcd1234deadbeef);
    }

    SECTION("high") {
        reg.set_high(0xabcd1234deadbeef);
        REQUIRE(reg.get_high() == 0xabcd1234deadbeef);
    }

    SECTION("as float") {
        reg.set_low(0x3f0000003fc00000);
        reg.set_high(0x40c0000040400000);

        float * value = reg.value_float();

        REQUIRE(value[0] == 1.5);
        REQUIRE(value[1] == 0.5);
        REQUIRE(value[2] == 3.0);
        REQUIRE(value[3] == 6.0);
    }
}

TEST_CASE("sse") {
    RandomAccessMemory ram;
    VectorUnit sse(ram);

    SECTION("load") {
        ram.size = 64;
        ram.data_register->set_value(0xabcd1234deadbeef);
        ram.address_register->set_value(123);
        ram.write();

        ram.data_register->set_value(0x1122334455667788);
        ram.address_register->set_value(131);
        ram.write();

        ram.address_register->set_value(123);
        sse.load_from_memory = true;
        sse.write_to_memory = false;
        sse.destination = sse.registers[0];
        sse.source = sse.temp_register;
        sse.operation = vector_operation::mov;
        sse.execute();

        REQUIRE(sse.registers[0]->get_low() == 0xabcd1234deadbeef);
        REQUIRE(sse.registers[0]->get_high() == 0x1122334455667788);
    }

    SECTION("write") {
        ram.address_register->set_value(100);
        sse.registers[0]->set_low(0x1122334455667788);
        sse.registers[0]->set_high(0x9900aabbccddeeff);

        sse.load_from_memory = false;
        sse.write_to_memory = true;
        sse.source = sse.registers[0];
        sse.destination = sse.temp_register;
        sse.operation = vector_operation::mov;
        sse.execute();

        ram.address_register->set_value(100);
        ram.size = 64;
        ram.load();
        REQUIRE(ram.data_register->get_value() == 0x1122334455667788);

        ram.address_register->set_value(108);
        ram.size = 64;
        ram.load();
        REQUIRE(ram.data_register->get_value() == 0x9900aabbccddeeff);
    }

    SECTION("addps") {
        // xmm0 = 6.0, 3.0, 0.5, 1.5
        sse.registers[0]->set_low(0x3f0000003fc00000);
        sse.registers[0]->set_high(0x40c0000040400000);

        // xmm1 = 28.0, 112.0, 7.0, 1792.0
        sse.registers[1]->set_low(0x40e0000044e00000);
        sse.registers[1]->set_high(0x41e0000042e00000);

        sse.registers[5]->set_low(0);
        sse.registers[5]->set_high(0);

        sse.operation = vector_operation::addps;
        sse.source = sse.registers[0];
        sse.source2 = sse.registers[1];
        sse.destination = sse.registers[5];

        sse.execute();

        REQUIRE(sse.registers[5]->get_low() == 0x40f0000044e03000);
        REQUIRE(sse.registers[5]->get_high() == 0x4208000042e60000);
    }
}
