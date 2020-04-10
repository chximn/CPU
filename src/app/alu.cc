#include "alu.hh"

void ArithemeticLogicUnit::execute() const {
    switch (operation) {
        case arithemetic_logic_operation::mov:
            destination->set_value(source->get_value());
            break;

        case arithemetic_logic_operation::add:
            destination->set_value(destination->get_value() + source->get_value());
            break;
    }
}
