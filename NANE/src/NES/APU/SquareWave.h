#pragma once

#include <memory>
#include <vector>

#include "NES/Memory/BitUtil.h"

class SquareWave
{
    private:
    double secondsPerApuCycle;

    double elapsedTimeSec = 0.0;

    int watchdogTimer = 0;
    bool enabled = false;
    bool haltWatchdog = false;

    int harmonics = 20;

    float frequency = 0.0;
    float dutyCycle = 0.0;

    public:
    SquareWave(int cpuClockRateHz);
    void ApuClock();
    void WatchdogClock();

    //number between 0.0 to 1.0
    float OutputSample();

    //getters/setters
    void SetFreqFromPeriod(dword period);
    void SetDutyCycle(int dutyCycleNum);
    void SetWatchdogTimer(int lengthCounter);
    void SetEnabled(bool isEnabled);
    void SetHaltWatchdogTimer(bool haltWatchdog);
};