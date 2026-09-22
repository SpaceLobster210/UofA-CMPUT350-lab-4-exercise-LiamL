#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <cstdint>

class Timer {
    public:
        //Type alias
        using Nanos = std::chrono::nanoseconds;
        using Micros = std::chrono::microseconds;
        using Millis = std::chrono::milliseconds;
        using Seconds = std::chrono::seconds;
        using Minutes = std::chrono::minutes;
        using Hours = std::chrono::hours;

        //Default constructor
        Timer() { restart(); };

        void restart() {
            lastClicked = std::chrono::steady_clock::now();
        }

        template <typename T>
        uint64_t click() {
            auto elapsedTime = glance<T>();
            restart();
            return elapsedTime;
        }

        template <typename T>
        uint64_t glance() {
            auto now = std::chrono::steady_clock::now();
            return std::chrono::duration_cast<T>(now - lastClicked).count();
        }

    private:
        std::chrono::time_point<std::chrono::steady_clock> lastClicked;
};

#endif  // TIMER_H
