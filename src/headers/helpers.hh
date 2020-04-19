#pragma once
#include <cstdint>

namespace helpers {
    uint64_t hi(uint64_t x, uint8_t size);
    uint64_t lo(uint64_t x, uint8_t size);
    void multiply(uint64_t a, uint64_t b, uint64_t & result, uint64_t & carry, uint8_t full_size);
    void devide(uint64_t a, uint64_t b, uint64_t & q, uint64_t & r, uint8_t full_size);
}
