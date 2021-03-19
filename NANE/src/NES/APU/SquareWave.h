#pragma once

#include <memory>
#include <vector>

#include "NES/Memory/BitUtil.h"

class SquareWave
{
    private:
    static const std::vector<int> LENGTH_COUNTER_LOOKUP;
    static const std::vector<std::vector<bool>> DUTY_CYCLE_TABLE;

    double secondsPerApuCycle;

    int watchdogTimer = 0;
    bool haltWatchdog = false;

    bool volumeResetFlag = true;
    int maxVolumeOrEnvelopePeriod = 0;
    bool constantVolume = false;
    int volumeDecayEnvelope = 15;
    int envelopePeriod = 0;

    byte sequencePos = 0;
    dword pulsePeriod = 0;
    byte dutyCycleNum = 0;
    dword timerVal = 0;

    bool sweepResetFlag = true;
    bool frequencySweepEnabled = false;
    byte sweepUnitPeriod = 0;
    bool isNegative = false;
    byte shiftPeriodAmount = 0;
    byte sweepCounter = 0;

    public:
    SquareWave(int cpuClockRateHz);
    void ApuClock();
    void WatchdogClock();
    void EnvelopeClock();
    void SweepClock();

    //number between 0.0 to 15.0
    float OutputSample();

    void ResetVolumeDecayEnvelope();

    bool IsOutputMuted();

    dword CalTargetPeriod() const;

    //getters/setters
    void SetPulsePeriod(dword period);
    void SetDutyCycle(int dutyCycleNum);
    void SetWatchdogTimer(int lengthCounter);
    void SetWatchdogTimerFromCode(int lengthCounterCode);
    void SetHaltWatchdogTimer(bool haltWatchdog);
    void SetMaxVolumeOrEnvelopePeriod(int volume);
    void SetConstantVolume(bool constantVol);

    void SetFrequencySweep(bool isEnabled, byte sweepUnitPeriod, bool isNegative, byte shiftPeriodAmount);
};