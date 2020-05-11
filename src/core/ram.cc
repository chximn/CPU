#include "ram.hh"

RandomAccessMemory::RandomAccessMemory():
    data{0},
    address_register(std::make_shared<FullRegister>(register_code::mar)),
    data_register(std::make_shared<FullRegister>(register_code::mdr)) {}

void RandomAccessMemory::load() const {
    auto offset = address_register->get_value();
    auto pointer = const_cast<uint64_t *>(reinterpret_cast<const uint64_t *>(data + offset));
    data_register->set_value(*pointer);

    // std::cout << "read operation from memory: address=" << helpers::to_hex(offset) << " value=" << helpers::to_hex(*pointer) << " size=" << std::to_string(size) << "\n";
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

    // std::cout << "write operation on memory: address=" << helpers::to_hex(offset) << " value=" << helpers::to_hex(new_value) << " size=" << std::to_string(size) << "\n";
}

std::vector<uint8_t> RandomAccessMemory::get_data(int from, int s) {
    std::vector<uint8_t> bytes;

    for (int address = from; address < from + s; address++) {
        bytes.push_back(data[address]);
    }

    return bytes;
}

std::vector<std::pair<uint64_t, std::string>> RandomAccessMemory::get_instructions(int from, int n) {
    std::vector<std::pair<uint64_t, std::string>> pairs;

    int address = from;
    for (int i = 0; i < n; i++) {
        uint64_t instruction_pointer = *reinterpret_cast<uint64_t *>(data + address);

        if (instruction_pointer != 0) {
            Instruction instruction = *reinterpret_cast<Instruction *>(instruction_pointer);
            pairs.push_back(std::pair<uint64_t, std::string>{
                address,
                instruction.to_string()
            });
        }

        else break;
        address += 8;
    }

    return pairs;
}
