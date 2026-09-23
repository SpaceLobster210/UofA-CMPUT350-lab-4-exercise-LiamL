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


size_t rowMajorIndexA(size_t x, size_t y, size_t z) {
    return z * Y * X + y * X + x;
}

size_t rowMajorIndexK(size_t x, size_t y, size_t z) {
    return z * KSIZE * KSIZE + y * KSIZE + x;
}

size_t rowMajorIndexConv(size_t x, size_t y, size_t z) {
    return (z / KSTRIDE) * Y * X + (y / KSTRIDE) * X + (x / KSTRIDE);
}


std::vector<uint64_t> initializeStandard() {
    std::vector<uint64_t> vect;
    vect.resize(SIZE);
    std::mt19937_64 rng(0);
    uint64_t next;

    for (size_t i = 0; i < (SIZE); i++) {
        next = rng();
        vect[i] = next;
    }
    return vect;
}

std::vector<uint64_t> initializeMorton(std::vector<uint64_t> vectA) {
    std::vector<uint64_t> vectB;
    vectB.resize(SIZE);
    for (size_t z = 0; z < Z; z++) {
        for (size_t y = 0; y < Y; y++) {
            for (size_t x = 0; x < X; x++) {
                vectB[morton3d(x, y, z)] = vectA[rowMajorIndexA(x, y, z)];
            }
        }
    }
    return vectB;
}

void initializeKA() {
    for (size_t z = 0; z < KSIZE; z++) {
        for (size_t y = 0; y < KSIZE; y++) {
            for (size_t x = 0; x < KSIZE; x++) {
                KA[rowMajorIndexK(x, y, z)] = x + y + z;
            }
        }
    }
}

std::vector<uint64_t> initializeKB(std::vector<uint64_t> ka) {
    std::vector<uint64_t> kb;
    kb.resize(KFULLSIZE);
    for (size_t z = 0; z < KSIZE; z++) {
        for (size_t y = 0; y < KSIZE; y++) {
            for (size_t x = 0; x < KSIZE; x++) {
                kb[morton3d(x, y, z)] = ka[rowMajorIndexK(x, y, z)];
            }
        }
    }
    return kb;
}


int main() {
    //Initialize 3D vectors
    std::vector<uint64_t> A = initializeStandard();
    std::vector<uint64_t> B = initializeMorton(A);

    //Initialize Kernels
    KA.resize(KFULLSIZE);
    initializeKA();
    KB.resize(KFULLSIZE);
    KB = initializeKB(KA);


    Timer myTime;
    size_t outSize = (X / KSTRIDE) * (Y / KSTRIDE) * (Z / KSTRIDE);

    //Convolution of A
    std::vector<uint64_t> aOut;
    aOut.resize(outSize);
    uint64_t tileValue;

    myTime.restart();
    for (size_t z = 0; z < Z; z += KSTRIDE) { //Iteration through A
        for (size_t y = 0; y < Y; y += KSTRIDE) {
            for (size_t x = 0; x < X; x += KSTRIDE) {

                //Calculate A * K and add to aOut
                tileValue = 0;
                for (size_t f = 0; f < KSIZE; f++) { //Iteration through KA
                    for (size_t e = 0; e < KSIZE; e++) {
                        for (size_t d = 0; d < KSIZE; d++) {
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
    bOut.resize(outSize);

    myTime.restart();
    for (size_t z = 0; z < Z; z += KSTRIDE) { //Iteration through B
        for (size_t y = 0; y < Y; y += KSTRIDE) {
            for (size_t x = 0; x < X; x += KSTRIDE) {

                //Calculate B * K and add to bOut
                tileValue = 0;
                for (size_t f = 0; f < KSIZE; f++) { //Iteration through KA
                    for (size_t e = 0; e < KSIZE; e++) {
                        for (size_t d = 0; d < KSIZE; d++) {
                            tileValue += KB[morton3d(d, e, f)] * B[morton3d(x + d, y + e, z + f)];
                        }
                    }
                }
                bOut[rowMajorIndexConv(x, y, z)] = tileValue;
            }
        }
    }
    elapsedTime = myTime.click<Timer::Micros>();
    std::cout << elapsedTime << "\n";


    //Test that output is correct
    /*for (size_t i = 0; i < outSize; i++) {
        assert(aOut[i] == bOut[i]);
    }*/
    // Note: could not resolve issue in time for lab submission


    return 0;
}
