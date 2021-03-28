#pragma once

#include <memory>

#include "Sequencer.h"
#include "NES/Memory/BitUtil.h"
#include "VolumeEnvelope.h"

#include "IWave.h"


class NoiseWave : public IWave
{
    private:
    static const std::vector<int> PERIOD_TABLE;

    bool isEnabled = false;
    std::unique_ptr<Sequencer> watchdogSeq;

    std::unique_ptr<Sequencer> noiseSeq;
    // sudo random number is 15 bits width:
    // https://wiki.nesdev.com/w/index.php/APU_Noise
    dword sudoRandomNumber : 15;
    bool useSixthBit = false;

    VolumeEnvelope volEnvelope;
    
    bool IsMuted();

    public:
    NoiseWave();
    void ApuClock() override;
    void WatchdogClock() override;
    void EnvelopeClock();
    void ResetVolumeDecayEnvelope();

    void SetWatchdogTimer(int lengthCounter) override;
    void SetWatchdogTimerFromCode(int lengthCounterCode) override;
    float OutputSample() override;
    void SetEnabled(bool isEnabled) override;
    void SetUseSixthBit(bool useSixthBit);
    dword GetPeriod() const;
    void SetPeriodFromLookupTable(int tableIndex);

    void SetHaltWatchdogTimer(bool haltWatchdog);
    void SetMaxVolumeOrEnvelopePeriod(int volume);
    void SetConstantVolume(bool constantVol);
};