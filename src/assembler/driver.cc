#include "driver.hh"

Program const & Driver::get_program() {
    return program;
}

void Driver::set_program(Program const & p) {
    program = p;
}
