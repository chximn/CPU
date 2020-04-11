#include <catch.hpp>
#include "alu.hh"
#include <memory>

TEST_CASE("alu") {
    ArithemeticLogicUnit alu;
    auto rax = std::make_shared<FullRegister>(register_code::rax);
    auto rbx = std::make_shared<FullRegister>(register_code::rbx);
    auto eax = std::make_shared<PartRegister>(register_code::eax, rax, 32, 0);
    auto ebx = std::make_shared<PartRegister>(register_code::ebx, rbx, 32, 0);


    SECTION("mov64 instruction") {
        rbx->set_value(2647);

        // move rax, rbx
        alu.source = rbx;
        alu.destination = rax;
        alu.operation = alu_operation::mov;
        alu.execute();

        REQUIRE(rax->get_value() == 2647);
    }

    SECTION("mov32 instruction") {
        ebx->set_value(2647);

        // move eax, ebx
        alu.source = ebx;
        alu.destination = eax;
        alu.operation = alu_operation::mov;
        alu.execute();

        REQUIRE(eax->get_value() == 2647);
        REQUIRE(rax->get_value() == 2647);
    }

    SECTION("add64 instruction") {
        rax->set_value(1000);
        rbx->set_value(1647);

        alu.source = rbx;
        alu.destination = rax;
        alu.operation = alu_operation::add;
        alu.execute();

        REQUIRE(rax->get_value() == 2647);
    }

    SECTION("add32 instruction") {
        eax->set_value(1000);
        ebx->set_value(1647);

        alu.source = ebx;
        alu.destination = eax;
        alu.operation = alu_operation::add;
        alu.execute();

        REQUIRE(eax->get_value() == 2647);
        REQUIRE(rax->get_value() == 2647);
    }
}
