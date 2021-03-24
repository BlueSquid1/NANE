#include "TriangleWave.h"

const std::vector<float> TriangleWave::SEQUENCE_TABLE = 
{
    15.0f, 14.0f, 13.0f, 12.0f, 11.0f, 10.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f
};

TriangleWave::TriangleWave()
{
    this->watchdogSeq = std::make_unique<Sequencer>(-1, false, nullptr);

    this->triSeq = std::make_unique<Sequencer>(-1, true, [this]()
    {
        this->sequencePos = (this->sequencePos + 1) % 32;
    });
}

void TriangleWave::ApuClock()
{
    this->triSeq->Clock();
}

void TriangleWave::WatchdogClock()
{
    this->watchdogSeq->Clock();
}

float TriangleWave::OutputSample()
{
    float sample = SEQUENCE_TABLE.at(this->sequencePos);
    return sample;
}

dword TriangleWave::GetPeriod() const
{
    return this->triSeq->GetPeriod();
}

void TriangleWave::SetPeriod(dword period)
{
    this->triSeq->SetPeriod(period, false);
}

void TriangleWave::SetWatchdogTimerFromCode(int lengthCounterCode)
{
    this->watchdogSeq->SetPeriod(IWave::LENGTH_COUNTER_LOOKUP.at(lengthCounterCode), true);
}