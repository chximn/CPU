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
    uint64_t mask = 0;
    mask = ~mask;
    mask = mask >> (64 - size);

    auto offset = address_register->get_value();
    uint64_t* pointer = const_cast<uint64_t *>(reinterpret_cast<const uint64_t *>(data + offset));
    uint64_t old_value = *pointer;
    uint64_t new_value = data_register->get_value();

    uint64_t value = (new_value & mask) | (old_value & (~mask));

    *pointer = value;
}
