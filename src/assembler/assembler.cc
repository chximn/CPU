#include "assembler.hh"

Assembler::Assembler(std::istream & is, std::ostream & os):
    driver(),
    scanner(is, os),
    parser(scanner, driver) {}

Program const & Assembler::assemble() {
    parser.parse();
    return driver.get_program();
}
