#ifndef P2A_H
#define P2A_H

#include <cstddef>
#include <cstdint>

constexpr int K = 64;


uint64_t mortonAddToOutput(uint64_t output, uint64_t value, size_t position, int shift) {
    uint64_t addValue = (value << position) & 0b0001ull;
    output += addValue << shift;
    value = value >> 1;
    return output;
}

inline uint64_t morton3d(uint64_t x, uint64_t y, uint64_t z) {
    uint64_t output = 0b0000ull;
    for (size_t i = 0; i < K; i += 3) {
        output = mortonAddToOutput(output, x, i, 0);
        if (i+1 >= K) { break; }
        output = mortonAddToOutput(output, y, i, 1);
        if (i+2 >= k) { break; }
        output = mortonAddToOutput(output, z, i, 2);
    }
}

#endif  // P2A_H