#ifndef P2A_H
#define P2A_H

#include <cstddef>
#include <cstdint>

constexpr int K = 64;
constexpr uint64_t mask = 0b0001ull;


inline uint64_t morton3d(uint64_t x, uint64_t y, uint64_t z) {
    uint64_t output = 0b0000ull;
    for (size_t i = 0; i < K; i += 3) {
        output += ((x >> (i / 3)) & mask) << (i);
        output += ((y >> (i / 3)) & mask) << (i + 1);
        output += ((z >> (i / 3)) & mask) << (i + 2);
    }
    return output;
}

#endif  // P2A_H