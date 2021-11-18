#pragma once
#include <chrono>

class Timer
{
private:
    static std::chrono::high_resolution_clock::time_point start_time;
public:
    // the absolute time since application start, in seconds.
    // updated every frame.
    static float time;
    // the time between the previous frame and the current frame, in seconds.
    // updated every frame.
    static float dt;
    // called from the application main loop
    static void updateTime();
};