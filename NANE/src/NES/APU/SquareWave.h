#pragma once

#include <memory>
#include <vector>

#include "NES/Memory/BitUtil.h"

class SquareWave
{
    private:
    static const std::vector<int> LENGTH_COUNTER_LOOKUP;
    static const std::vector<float> DUTY_CYCLE_TABLE;

    int harmonics = 20;

    double secondsPerApuCycle;
    double elapsedTimeSec = 0.0;

    int watchdogTimer = 0;
    bool haltWatchdog = false;

    //between 0 to 15
    bool resetFlag = false;
    int maxVolumeOrEnvelopePeriod = 0;
    bool constantVolume = false;
    int volumeDecayEnvelope = 15;
    int envelopePeriod = 0;

    float frequency = 0.0;
    float dutyCycle = 0.0;

    public:
    SquareWave(int cpuClockRateHz);
    void ApuClock();
    void WatchdogClock();
    void EnvelopeClock();

    //number between 0.0 to 15.0
    float OutputSample();

    void ResetVolumeDecayEnvelope();

    //getters/setters
    void SetFreqFromPeriod(dword period);
    void SetDutyCycle(int dutyCycleNum);
    void SetWatchdogTimer(int lengthCounter);
    void SetWatchdogTimerFromCode(int lengthCounterCode);
    void SetHaltWatchdogTimer(bool haltWatchdog);
    void SetMaxVolumeOrEnvelopePeriod(int volume);
    void SetConstantVolume(bool constantVol);
};