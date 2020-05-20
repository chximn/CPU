#include <catch.hpp>

#include "helpers.hh"

using namespace helpers;

TEST_CASE("helpers") {
    SECTION("low") {
        REQUIRE(lo(0xFA, 4) == 0xA);
        REQUIRE(lo(0xABCD, 8) == 0xCD);
        REQUIRE(lo(0xABCD1234, 16) == 0x1234);
        REQUIRE(lo(0xABCD1234DEADBEEF, 32) == 0xDEADBEEF);
    }

    SECTION("high") {
        REQUIRE(hi(0xFA, 4) == 0xF);
        REQUIRE(hi(0xABCD, 8) == 0xAB);
        REQUIRE(hi(0xABCD1234, 16) == 0xABCD);
        REQUIRE(hi(0xABCD1234DEADBEEF, 32) == 0xABCD1234);
    }

    SECTION("multiply") {
        uint64_t result, carry;

        multiply(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF0, result, carry, 64);
        REQUIRE(result == 0x10);
        REQUIRE(carry == 0xffffffffffffffef);

        multiply(0xFFFFFFFF, 0xFFFFFFF0, result, carry, 32);
        REQUIRE(result == 0x10);
        REQUIRE(carry == 0xffffffef);

        multiply(0xFFFF, 0xFFF0, result, carry, 16);
        REQUIRE(result == 0x10);
        REQUIRE(carry == 0xffef);

        multiply(0xFF, 0xF0, result, carry, 8);
        REQUIRE(result == 0x10);
        REQUIRE(carry == 0xef);
    }

    SECTION("devide") {
        uint64_t q, r;

        devide(10, 2, q, r, 64);
        REQUIRE(q == 5);
        REQUIRE(r == 0);

        devide(20, 3, q, r, 64);
        REQUIRE(q == 6);
        REQUIRE(r == 2);
    }
}
