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

SquareWave::SquareWave(int cpuClockRateHz)
{
    this->secondsPerApuCycle = 2.0 / cpuClockRateHz;
}

void SquareWave::Clock()
{
    this->elapsedTimeSec += this->secondsPerApuCycle;
}

float SquareWave::OutputSample()
{
    if(this->enabled == false)
    {
        return 0.0f;
    }

    /**
     * Implement Fourier series for a square wave:
     * 
     *                harmonics
     *                 ------
     *                  \
     * a(elapsedTime) =  \   sin( elapsedTime * frequency * 2 * pi * n ) / n )
     *                   /   
     *                  /    
     *                 ------
     *                  n = 1
     * 
     *                harmonics
     *                 ------
     *                  \
     * b(elapsedTime) =  \   sin( (elapsedTime * frequency - phase) * 2 * pi * n ) / n )
     *                   /   
     *                  /    
     *                 ------
     *                  n = 1
     * 
     * square wave(elapsedTime) = (4/pi) * (a - b);
     * 
     */
    float a = 0;
    float b = 0;
    float phase = this->dutyCycle * 2.0 * M_PI;

    for (int n = 1; n < harmonics+1; n++)
    {
        double c = n * frequency * 2.0 * M_PI * this->elapsedTimeSec;
        a += -BitUtil::approxsin(c) / n;
        b += -BitUtil::approxsin(c - (phase * n)) / n;
    }

    //instead of multipling by 4 we will only multiple by 1 and the add 1 so the output is never above 1 or below 0
    return (1.0f / M_PI) * (a - b) + 1;
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