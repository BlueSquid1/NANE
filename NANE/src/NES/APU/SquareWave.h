#pragma once

#include <memory>
#include <vector>

#include "NES/Memory/BitUtil.h"

class SquareWave
{
    private:
    double secondsPerApuCycle;

    double elapsedTimeSec = 0.0;

    bool enabled = false;
    int harmonics = 20;

    float frequency = 0.0;
    float dutyCycle = 0.0;

    public:
    SquareWave(int cpuClockRateHz);
    void Clock();
    //number between 0.0 to 1.0
    float OutputSample();

    //getters/setters
    void SetFreqFromPeriod(dword period);
    void SetDutyCycle(int dutyCycleNum);
    void SetEnabled(bool isEnabled);
};