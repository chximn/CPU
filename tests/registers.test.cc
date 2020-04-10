#include <catch.hpp>
#include "register.hh"

TEST_CASE("full registers") {
    FullRegister reg(register_code::rax);

    SECTION("get and set value") {
        reg.set_value(2647);
        REQUIRE(reg.get_value() == 2647);
    }

    SECTION("increment") {
        reg.set_value(2647);
        reg.increment();
        REQUIRE(reg.get_value() == 2648);
    }

    SECTION("clear") {
        reg.set_value(2647);
        reg.clear();
        REQUIRE(reg.get_value() == 0);
    }
}

TEST_CASE("part register") {
    auto rax = std::make_shared<FullRegister>(register_code::rax);
    auto eax = std::make_shared<PartRegister>(register_code::eax, rax, 32, 0);
    auto  ax = std::make_shared<PartRegister>(register_code::eax, rax, 16, 0);
    auto  ah = std::make_shared<PartRegister>(register_code::ah,  rax, 8,  8);
    auto  al = std::make_shared<PartRegister>(register_code::al,  rax, 8,  0);

    SECTION("get and set value") {
        rax->set_value(0xdeadbeef);

        REQUIRE(rax->get_value() == 0xdeadbeef);
        REQUIRE(eax->get_value() == 0xdeadbeef);
        REQUIRE( ax->get_value() == 0xbeef);
        REQUIRE( ah->get_value() == 0xbe);
        REQUIRE( al->get_value() == 0xef);

        ah->set_value(0xc0);

        REQUIRE(rax->get_value() == 0xdeadc0ef);
        REQUIRE(eax->get_value() == 0xdeadc0ef);
        REQUIRE( ax->get_value() == 0xc0ef);
        REQUIRE( ah->get_value() == 0xc0);
        REQUIRE( al->get_value() == 0xef);
    }

    SECTION("increment") {
        rax->set_value(0xdeadbeef);

        ah->increment();
        REQUIRE(rax->get_value() == 0xdeadbfef);
    }

    SECTION("clear") {
        rax->set_value(0xdeadbeef);

        ah->clear();
        REQUIRE(rax->get_value() == 0xdead00ef);
    }
}
