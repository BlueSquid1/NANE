#pragma once

#include <memory>

#include "NES/Memory/ThreadSafeQueue.h"

#include "ApuRegisters.h"
#include "NES/Memory/Dma.h"

class Apu
{
    private:
    const int clockRateHz = 1789773; //how many times per second the Step() method is called

    //how many audio samples need to be produced per second for the game engine speaker
    int audioSamplesPerSecond;

    std::shared_ptr<Dma> dma;

    // Output audio
    std::shared_ptr<ThreadSafeQueue<float>> audioStream;

    ApuRegisters& GetRegs();

    public:
    Apu(int samplesPerSecond, std::shared_ptr<Dma> dma);

    void Step();

    std::shared_ptr<ThreadSafeQueue<float>> GetAudio();
};