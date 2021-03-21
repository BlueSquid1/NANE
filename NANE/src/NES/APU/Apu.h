#pragma once

#include <memory>

#include "NES/Memory/ThreadSafeQueue.h"

#include "ApuRegisters.h"
#include "NES/Memory/Dma.h"

class Apu
{
    private:
    //how many audio samples need to be produced per second for the game engine speaker
    int audioSamplesPerSecond;

    std::shared_ptr<Dma> dma;

    // Output audio
    std::shared_ptr<ThreadSafeQueue<float>> audioStream;

    public:
    Apu(int samplesPerSecond, std::shared_ptr<Dma> dma);

    /**
     * @brief Trigger a power reset for the APU.
     * @return true is successfully.
     */
    bool PowerCycle();

    /* step is called for every CPU clock */
    void Step();

    std::shared_ptr<ThreadSafeQueue<float>> GetAudio();

    float MixChannels(float sq1, float sq2);

    float Filter(float sample);

    void ClockFrameCounter(const long long& apuClockCycle);

    void ClockChannels(const long long& apuClockCycle);

    void ClockEnvelopes();

    void ClockWatchdogs();

    void ClockFreqSweeps();
};