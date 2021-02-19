#pragma once

#include <memory>

#include "NES/Memory/ThreadSafeQueue.h"

#include "ApuRegisters.h"
#include "NES/Memory/Dma.h"

class Apu
{
    private:
    //TODO 
    //1786800
    //1789773
    const int clockRateHz = 1786800; //how many times per second the Step() method is called

    //how many audio samples need to be produced per second for the game engine speaker
    int audioSamplesPerSecond;

    std::shared_ptr<Dma> dma;

    // Output audio
    std::shared_ptr<ThreadSafeQueue<float>> audioStream;

    public:
    Apu(int samplesPerSecond, std::shared_ptr<Dma> dma);

    /* step is called for every CPU clock */
    void Step();

    std::shared_ptr<ThreadSafeQueue<float>> GetAudio();

    /**
     * Calculates the time in seconds from audio cycles 
     */
    float CalRealTimeSec();
};