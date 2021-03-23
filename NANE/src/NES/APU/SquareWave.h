#pragma once

#include <memory>
#include <vector>

#include "Sequencer.h"
#include "NES/Memory/BitUtil.h"

class SquareWave
{
    private:
    static const std::vector<int> LENGTH_COUNTER_LOOKUP;
    static const std::vector<std::vector<bool>> DUTY_CYCLE_TABLE;

    bool isPulse2 = false; //whether or not it is pulse 2. false: it is pulse 1, true it is pulse 2.

    std::unique_ptr<Sequencer> watchDogSeq; //sequencer to silence the channel
    bool isEnabled = false; // whether the wave is enabled. false: output is always zero, true: output is from square wave.

    std::unique_ptr<Sequencer> volumeEnvelopeSeq;
    bool volumeResetFlag = true; // tracks when a volume adjust has just happened
    bool constantVolume = false; //true: channel has constant volume. false: volume has sawtooth envelope.
    int volumeDecayEnvelope = 15; //used to track sawtooth envelope.

    std::unique_ptr<Sequencer> pulseSeq;
    byte sequencePos = 0; //which part in the square wave we are up to (think of as time).
    byte dutyCycleNum = 0; //which duty cycle is active 0: 12.5%, 1: 25%, 2: 50%, 3: 75%.

    std::unique_ptr<Sequencer> sweepSeq;
    bool sweepResetFlag = true; //tracks when a frequency sweep has just be set.
    bool frequencySweepEnabled = false; //whether or not a frequency sweep is needed.
    bool isNegative = false; //frequency needs to be adjusted in a negative dirrection.
    byte shiftPeriodAmount = 0; //amount to shift frequency (represented as a period).

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