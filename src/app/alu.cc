#include "alu.hh"

void ArithemeticLogicUnit::execute() const {
    switch (operation) {
        case alu_operation::mov:
            destination->set_value(source->get_value());
            break;

        case alu_operation::add:
            destination->set_value(destination->get_value() + source->get_value());
            break;

        default:

            break;
    }
}
