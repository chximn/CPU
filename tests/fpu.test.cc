#include <catch.hpp>

#include "fpu.hh"
#include "register.hh"

TEST_CASE("fpu") {
    FloatingPointUnit fpu;

    for (int i = 0; i < 8; i++) {
        fpu.stages[i]->set_value(i);
    }

    register_ptr reg = std::make_shared<FullRegister>(register_code::mdr);
    reg->set_value(0xdeadbeef);

    SECTION("convert") {
        REQUIRE(fpu.convert_double(0x3ffc000000000000) == 1.75);
        REQUIRE(fpu.convert_float(0x3fe00000) == 1.75);
        REQUIRE(fpu.convert_uint64(1.75) == 0x3ffc000000000000);
    }

    SECTION("push") {
        fpu.push(0xdeadbeef);
        REQUIRE(fpu.stages[0]->get_value() == 0xdeadbeef);

        for (int i = 1; i < 8; i++) {
            REQUIRE(fpu.stages[i]->get_value() == i - 1);
        }
    }

    SECTION("pop") {
        fpu.pop();

        for (int i = 0; i < 7; i++) {
            REQUIRE(fpu.stages[i]->get_value() == i + 1);
        }
    }

    SECTION("fld") {
        reg->set_value(0x3fe00000);
        fpu.operation = fpu_operation::fld;
        fpu.src_dest = reg;
        fpu.is_double = false;
        fpu.execute();

        REQUIRE(fpu.stages[0]->get_value() == 0x3ffc000000000000);
    }

    SECTION("fst") {
        fpu.push(0x3ffc000000000000);

        fpu.operation = fpu_operation::fst;
        fpu.src_dest = reg;
        fpu.is_double = false;

        SECTION("with pop") {
            fpu.perform_pop = true;
            fpu.execute();
            REQUIRE(reg->get_value() == 0x3fe00000);
            REQUIRE(fpu.stages[0]->get_value() == 0);
        }

        SECTION("without pop") {
            fpu.perform_pop = false;
            fpu.execute();
            REQUIRE(reg->get_value() == 0x3fe00000);
            REQUIRE(fpu.stages[0]->get_value() == 0x3ffc000000000000);
        }
    }

    SECTION("fadd") {
        fpu.operation = fpu_operation::fadd;
        fpu.src_dest = reg;
        fpu.stages[0]->set_value(0x3FD0000000000000);

        SECTION("source") {
            fpu.is_destination = false;
            fpu.perform_pop = false;

            SECTION("float") {
                fpu.is_double = false;
                reg->set_value(0x3fe00000);
                fpu.execute();

                REQUIRE(fpu.stages[0]->get_value() == 0x4000000000000000);
            }

            SECTION("double") {
                fpu.is_double = true;
                reg->set_value(0x3ffc000000000000);
                fpu.execute();

                REQUIRE(fpu.stages[0]->get_value() == 0x4000000000000000);
            }
        }

        SECTION("destination") {
            fpu.src_dest = fpu.stages[5];
            fpu.stages[5]->set_value(0x3ffc000000000000);
            fpu.is_destination = true;
            fpu.is_double = true;

            SECTION("without pop") {
                fpu.perform_pop = false;
                fpu.execute();

                REQUIRE(fpu.stages[5]->get_value() == 0x4000000000000000);
            }

            SECTION("with pop") {
                fpu.perform_pop = true;
                fpu.execute();

                REQUIRE(fpu.stages[4]->get_value() == 0x4000000000000000);
            }
        }
    }
}
