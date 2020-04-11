#include <catch.hpp>
#include "ram.hh"

TEST_CASE("ram") {
    RandomAccessMemory ram;

    SECTION("64bit") {
        ram.address_register->set_value(10);
        ram.data_register->set_value(0xabcd1234deadbeef);
        ram.size = 64;
        ram.write();

        ram.address_register->set_value(12);
        ram.load();

        auto value = ram.data_register->get_value() & 0xffffffffffff;
        REQUIRE(value == 0xabcd1234dead);
    }

    SECTION("32bit") {
        ram.address_register->set_value(10);
        ram.data_register->set_value(0xdeadbeef);
        ram.size = 32;
        ram.write();

        ram.address_register->set_value(12);
        ram.load();

        auto value = ram.data_register->get_value() & 0xffff;
        REQUIRE(value == 0xdead);
    }
}
