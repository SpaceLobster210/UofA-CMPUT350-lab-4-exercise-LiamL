#include <cstddef>
#include <random>
#include <iostream>

#include "timer.h"

constexpr size_t SIZE = 16000000;
constexpr size_t ROWS = 4000;
constexpr size_t COLS = 4000;


void printResult(auto elapsed, uint64_t sum) {
    std::cout << elapsed << " " << sum << "\n";
}

int main() { 
    std::vector<uint64_t> vect1;
    vect1.reserve(SIZE);
    std::vector<uint64_t> vect2;
    vect2.reserve(SIZE);

    std::mt19937_64 rng(0);
    uint64_t next;


    //Filling vect1, row-major order
    rng.seed(0);
    for (size_t i = 0; i < SIZE; i++) {
        next = rng();
        vect1[i] = next;
    }

    //Filling vect2, column-major order
    rng.seed(0);
    for (size_t r = 0; r < ROWS; r++) {
        for (size_t c = 0; c < COLS; c++) {
            next = rng();
            vect2[(c * COLS) + r] = next;
        }
    }


    Timer myTime;
    uint64_t vect1Sum = 0;
    uint64_t vect2Sum = 0;

    //Sum vect1, row-major order
    myTime.restart();
    for (size_t i = 0; i < SIZE; i++) {
        vect1Sum += vect1[i];
    }
    auto elapsedTime = myTime.click<Timer::Micros>();
    printResult(elapsedTime, vect1Sum);

    //Sum vect2, column-major order
    myTime.restart();

    for (size_t r = 0; r < ROWS; r++) {
        for (size_t c = 0; c < COLS; c++) {
            vect2Sum += vect2[(c * COLS) + r];
        }
    }
    elapsedTime = myTime.click<Timer::Micros>();
    printResult(elapsedTime, vect2Sum);


    return 0;
}
