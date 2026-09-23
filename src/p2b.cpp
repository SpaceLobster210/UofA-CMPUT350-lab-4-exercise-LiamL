#include <iostream>
#include <cstddef>
#include <random>
#include <iostream>
#include <cassert>

#include "timer.h"
#include "p2a.h"

constexpr size_t SIZE = 16777216;
constexpr size_t X = 256;
constexpr size_t Y = 256;
constexpr size_t Z = 256;

constexpr size_t KFULLSIZE = 64;
std::vector<uint64_t> KA;
std::vector<uint64_t> KB;
constexpr size_t KSIZE = 4;
constexpr size_t KSTRIDE = 4;

std::vector<uint64_t> initializeStandard() {
    std::vector<uint64_t> vect;
    vect.reserve(SIZE);
    std::mt19937_64 rng(0);
    uint64_t next;

    for (int i = 0; i < (SIZE); i++) {
        next = rng();
        vect[i] = next;
    }
    return vect;
}

std::vector<uint64_t> initializeMorton(std::vector<uint64_t> vectA, size_t size) {
    std::vector<uint64_t> vectB;
    vectB.reserve(size);
    for (int z = 0; z < Z; z++) {
        for (int y = 0; y < Y; y++) {
            for (int x = 0; x < X; x++) {
                vectB[morton3d(x, y, z)] = vectA[rowMajorIndexA(x, y, z)];
            }
        }
    }
    return vectB;
}

void initializeKA() {
    for (int z = 0; z < KSIZE; z++) {
        for (int y = 0; y < KSIZE; y++) {
            for (int x = 0; x < KSIZE; x++) {
                KA[rowMajorIndexK(x, y, z)] = x + y + z;
            }
        }
    }
}

size_t rowMajorIndexA(size_t x, size_t y, size_t z) {
    return z * Y * X + y * X + x;
}

size_t rowMajorIndexK(size_t x, size_t y, size_t z) {
    return z * KSIZE * KSIZE + y * KSIZE + x;
}

size_t rowMajorIndexConv(size_t x, size_t y, size_t z) {
    return (z / KSTRIDE) * Y * X + (y / KSTRIDE) * X + (x / KSTRIDE);
}

int main() {
    //Initialize 3D vectors
    std::vector<uint64_t> A = initializeStandard();
    std::vector<uint64_t> B = initializeMorton(A, SIZE);

    //Initialize Kernels
    KA.reserve(KFULLSIZE);
    initializeKA();
    KB.reserve(KFULLSIZE);
    KB = initializeMorton(KA, KFULLSIZE);


    Timer myTime;
    size_t outSize = (X / KSTRIDE) * (Y / KSTRIDE) * (Z / KSTRIDE);

    //Convolution of A
    std::vector<uint64_t> aOut;
    aOut.reserve(outSize);
    uint64_t tileValue;

    myTime.restart();
    for (int z = 0; z < Z; z += KSTRIDE) { //Iteration through A
        for (int y = 0; y < Y; y += KSTRIDE) {
            for (int x = 0; x < X; x += KSTRIDE) {

                //Calculate A * K and add to aOut
                tileValue = 0;
                for (int f = 0; f < KSIZE; f++) { //Iteration through KA
                    for (int e = 0; e < KSIZE; e++) {
                        for (int d = 0; d < KSIZE; d++) {
                            tileValue += KA[rowMajorIndexK(d, e, f)] * A[rowMajorIndexA(x + d, y + e, z + f)];
                        }
                    }
                }
                aOut[rowMajorIndexConv(x, y, z)] = tileValue;
            }
        }
    }
    auto elapsedTime = myTime.click<Timer::Micros>();
    std::cout << elapsedTime << "\n";


    //Convolution of B
    std::vector<uint64_t> bOut;
    bOut.reserve(outSize);
    size_t strideSize = KSTRIDE * KSTRIDE * KSTRIDE;

    myTime.restart();
    for (int i = 0; i < SIZE; i += strideSize) { //Iteration through B
        //Calculate B * K and add to bOut
        tileValue = 0;
        for (int g = 0; g < KFULLSIZE; g++) {
            tileValue += KB[g] * B[i + g];
        }
        bOut[i / strideSize] = tileValue;
    }
    elapsedTime = myTime.click<Timer::Micros>();
    std::cout << elapsedTime << "\n";


    //Test that output is correct
    for (int i = 0; i < outSize; i++) {
        assert(aOut[i] == bOut[i]);
    }


    return 0;
}
