#include "helpers.hh"

using namespace helpers;

uint64_t helpers::hi(uint64_t x, uint8_t size) {
    return x >> size;
}

uint64_t helpers::lo(uint64_t x, uint8_t size) {
    return ((1L << size) - 1) & x;
}

void helpers::multiply(uint64_t a, uint64_t b, uint64_t & result, uint64_t & carry, uint8_t full_size) {
    uint8_t size = full_size / 2;
    uint64_t s0, s1, s2, s3;

    uint64_t x = lo(a, size) * lo(b, size);
    s0 = lo(x, size);

    x = hi(a, size) * lo(b, size) + hi(x, size);
    s1 = lo(x, size);

    s2 = hi(x, size);

    x = s1 + lo(a, size) * hi(b, size);
    s1 = lo(x, size);

    x = s2 + hi(a, size) * hi(b, size) + hi(x, size);
    s2 = lo(x, size);
    s3 = hi(x, size);

    result = s1 << size | s0;
    carry  = s3 << size | s2;
}

void helpers::devide(uint64_t a, uint64_t b, uint64_t & q, uint64_t & r, uint8_t full_size) {
    uint64_t mask = (~static_cast<uint64_t>(0) >> (64 - full_size));

    r = (a % b) & mask;
    q = (a / b) & mask;
}

std::string helpers::to_hex(uint64_t value) {
    return helpers::to_hex(value, "0x");
}

std::string helpers::to_hex(uint64_t value, std::string const & prefix) {
    std::stringstream stream;
    stream << std::hex << value;
    auto result = prefix;
    if (value < 16) result += "0";
    result += stream.str();
    return result;
}
