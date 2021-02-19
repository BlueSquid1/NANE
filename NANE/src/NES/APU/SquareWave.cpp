#include <math.h>

#include "SquareWave.h"

namespace
{
    std::vector<float> DUTY_CYCLE_TABLE = 
    {
        0.125,
        0.250,
        0.500,
        0.750
    };
}

float SquareWave::OutputSample(float elapsedTime)
{
    if(this->enabled == false)
    {
        return 0.0f;
    }
    
    double a = 0;
    double b = 0;
    double p = this->dutyCycle * 2.0 * M_PI;

    auto approxsin = [](float t)
    {
        float j = t * 0.15915;
        j = j - (int)j;
        return 20.785 * j * (j - 0.5) * (j - 1.0f);
    };

    for (double n = 1; n < harmonics; n++)
    {
        double c = n * frequency * 2.0 * M_PI * elapsedTime;
        a += -approxsin(c) / n;
        b += -approxsin(c - p * n) / n;
    }

    return (2.0 / M_PI) * (a - b);
}

void SquareWave::SetFreqFromPeriod(dword period)
{
    this->frequency = 1789773.0f / (16.0f * (float)(period + 1));
}

void SquareWave::SetDutyCycle(int dutyCycleNum)
{
    this->dutyCycle = DUTY_CYCLE_TABLE.at(dutyCycleNum);
}

void SquareWave::SetEnabled(bool isEnabled)
{
    this->enabled = isEnabled;
}