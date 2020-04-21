#include <catch.hpp>
#include <sstream>
#include "assembler.hh"


TEST_CASE("assembler") {
    std::stringstream in;

    in << "\n\n\n324234\n\n\n"
          "2345\n\n\n\n\n\n";

    Assembler assembler(in, std::cout);
    Program program = assembler.assemble();
}
