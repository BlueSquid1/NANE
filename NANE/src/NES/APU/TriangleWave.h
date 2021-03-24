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

    std::unique_ptr<Sequencer> triSeq;
    byte sequencePos = 0;



    public:
    TriangleWave();

    void ApuClock() override;
    void WatchdogClock();

    //number between 0.0 to 15.0
    float OutputSample() override;

    //Getters/setters
    dword GetPeriod() const;
    void SetPeriod(dword period);
    void SetWatchdogTimerFromCode(int lengthCounterCode);
};