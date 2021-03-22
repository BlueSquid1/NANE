#pragma once

#include <memory>
#include <vector>

#include "NES/Memory/BitUtil.h"

class SquareWave
{
    private:
    static const std::vector<int> LENGTH_COUNTER_LOOKUP;
    static const std::vector<std::vector<bool>> DUTY_CYCLE_TABLE;

    bool isPulse2 = false; //whether or not it is pulse 2. false: it is pulse 1, true it is pulse 2.

    bool isEnabled = false; // whether the wave is enabled. false: output is always zero, true: output is from square wave.
    int watchdogTimer = 0; // counter until channel is silenced
    bool haltWatchdog = false; // true: stop decrementing watchdog timer, false decrement on watchdog clock.

    bool volumeResetFlag = true; // tracks when a volume adjust has just happened
    int maxVolumeOrEnvelopePeriod = 0; //if constantVolume is true, stores max volume otherwise stores volume envelope period.
    bool constantVolume = false; //true: channel has constant volume. false: volume has sawtooth envelope.
    int volumeDecayEnvelope = 15; //used to track sawtooth envelope.
    int envelopePeriod = 0; //internal variable that tracks the current sawtooth envelope period.

    byte sequencePos = 0; //which part in the square wave we are up to (think of as time).
    dword pulsePeriod = 0; //period of the square wave.
    byte dutyCycleNum = 0; //which duty cycle is active 0: 12.5%, 1: 25%, 2: 50%, 3: 75%
    dword timerVal = 0; //internal variable that tracks when the square wave is due to be updated.

    bool sweepResetFlag = true; //tracks when a frequency sweep has just be set.
    bool frequencySweepEnabled = false; //whether or not a frequency sweep is needed.
    byte sweepUnitPeriod = 0; //period at which the frequency needs to be changed.
    bool isNegative = false; //frequency needs to be adjusted in a negative dirrection.
    byte shiftPeriodAmount = 0; //amount to shift frequency (represented as a period).
    byte sweepCounter = 0; //internal variable that tracks when the frequency needs to be changed again.

    bool IsOutputMuted();
    dword CalTargetPeriod() const;

    public:
    SquareWave(bool isPulse2);
    void ApuClock();
    void WatchdogClock();
    void EnvelopeClock();
    void SweepClock();

    //number between 0.0 to 15.0
    float OutputSample();

    void ResetVolumeDecayEnvelope();

    //getters/setters
    void SetIsEnabled(bool isEnabled);
    dword GetPulsePeriod();
    void SetPulsePeriod(dword period);
    void SetDutyCycle(int dutyCycleNum);
    int GetWatchdogTimer();
    void SetWatchdogTimer(int lengthCounter);
    void SetWatchdogTimerFromCode(int lengthCounterCode);
    void SetHaltWatchdogTimer(bool haltWatchdog);
    void SetMaxVolumeOrEnvelopePeriod(int volume);
    void SetConstantVolume(bool constantVol);
    void SetFrequencySweep(bool isEnabled, byte sweepUnitPeriod, bool isNegative, byte shiftPeriodAmount);
};