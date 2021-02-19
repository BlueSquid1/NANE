#pragma once

#include <memory>
#include <vector>

#include "NES/Memory/BitUtil.h"

class SquareWave
{
    private:
    int harmonics = 20;

    float frequency = 0.0;
    float dutyCycle = 0.0;

    public:
    float OutputSample(float elapsedTime);

    //getters/setters
    void SetFreqFromPeriod(dword period);
    void SetDutyCycle(int dutyCycleNum);
};