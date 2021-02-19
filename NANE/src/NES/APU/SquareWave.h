#pragma once

#include <memory>
#include <vector>

#include "NES/Memory/BitUtil.h"

class SquareWave
{
    private:
    bool enabled = false;
    int harmonics = 20;

    float frequency = 0.0;
    float dutyCycle = 0.0;

    public:
    float OutputSample(float elapsedTime);

    //getters/setters
    void SetFreqFromPeriod(dword period);
    void SetDutyCycle(int dutyCycleNum);
    void SetEnabled(bool isEnabled);
};