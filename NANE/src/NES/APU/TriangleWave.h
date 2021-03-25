#pragma once

#include <memory>

#include "NES/Memory/BitUtil.h"
#include "Sequencer.h"

#include "IWave.h"

class TriangleWave : public IWave
{
    private:
    static const std::vector<float> SEQUENCE_TABLE;

    std::unique_ptr<Sequencer> watchdogSeq;

    std::unique_ptr<Sequencer> linearCounterSeq;
    bool linearResetFlag = false;

    std::unique_ptr<Sequencer> triSeq;
    byte sequencePos = 0;



    public:
    TriangleWave();

    void ApuClock() override;
    void WatchdogClock();
    void LinearCounterClock();

    //number between 0.0 to 15.0
    float OutputSample() override;

    void TriggerLinearReset();

    //Getters/setters
    void SetEnabled(bool isEnabled) override;
    dword GetPeriod() const;
    void SetPeriod(dword period);
    void SetWatchdogTimer(int lengthCounter);
    void SetWatchdogTimerFromCode(int lengthCounterCode);
    void SetLinearCounter(byte countDown);
    void SetHaltTimers(bool haltTimers);
};