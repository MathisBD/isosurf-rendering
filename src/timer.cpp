#include "timer.h"
#include <chrono>


std::chrono::high_resolution_clock::time_point Timer::start_time = std::chrono::high_resolution_clock::now();
float Timer::time = 0.0f;
float Timer::dt = 0.0f;

void Timer::updateTime()
{
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float, std::milli> duration;
    
    duration delta = clock::now() - Timer::start_time;
    float new_time = delta.count() / 1000.0f;

    Timer::dt = new_time - Timer::time;
    Timer::time = new_time;
}