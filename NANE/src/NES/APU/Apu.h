#pragma once

#include <memory>

#include "NES/Memory/ThreadSafeQueue.h"

class Apu
{
    private:
    std::shared_ptr<ThreadSafeQueue<float>> audioStream;
    const int clockRate = 1786800;
    long long totalClockCycles = 0;
    int samplesPerSecond;


    //TODO
    double timeElapsed = 0.0;

    public:
    Apu(int samplesPerSecond);
    void Step();
    std::shared_ptr<ThreadSafeQueue<float>> GetAudio();
};