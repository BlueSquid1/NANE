#pragma once

#include <vector>

class IWave
{
    protected:
    static const std::vector<int> LENGTH_COUNTER_LOOKUP;

    public:
    virtual void ApuClock() = 0;
    virtual void WatchdogClock() = 0;
    virtual void SetWatchdogTimer(int lengthCounter) = 0;
    virtual void SetWatchdogTimerFromCode(int lengthCounterCode) = 0;
    virtual float OutputSample() = 0;
    virtual void SetEnabled(bool isEnabled) = 0;
};