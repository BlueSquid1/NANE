#pragma once

#include <vector>

class IWave
{
    protected:
    static const std::vector<int> LENGTH_COUNTER_LOOKUP;

    public:
    virtual void ApuClock() = 0;
    virtual float OutputSample() = 0;
};