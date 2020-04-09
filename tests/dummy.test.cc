#include <catch.hpp>
#include "example.hh"

TEST_CASE("Dummy says hello") {
    Dummy dummy;
    REQUIRE(dummy.say_hello() == "Hello, World!");
}
