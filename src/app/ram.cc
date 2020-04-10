#include "ram.hh"

RandomAccessMemory::RandomAccessMemory():
    address_register(std::make_shared<FullRegister>(register_code::mar)),
    data_register(std::make_shared<FullRegister>(register_code::mdr)) {}

void RandomAccessMemory::load() const {
    auto offset = address_register->get_value();
    auto pointer = const_cast<uint64_t *>(reinterpret_cast<const uint64_t *>(data + offset));
    data_register->set_value(*pointer);
}

void RandomAccessMemory::write() {
    auto offset = address_register->get_value();
    uint64_t* pointer = const_cast<uint64_t *>(reinterpret_cast<const uint64_t *>(data + offset));
    *pointer = data_register->get_value();
}
