#include <math.h>

#include "SquareWave.h"

const std::vector<int> SquareWave::LENGTH_COUNTER_LOOKUP = 
{
     10, 254, 20,  2, 40,  4, 80,  6,
    160,   8, 60, 10, 14, 12, 26, 14,
     12,  16, 24, 18, 48, 20, 96, 22,
    192,  24, 72, 26, 16, 28, 32, 30
};

const std::vector<float> SquareWave::DUTY_CYCLE_TABLE = 
{
    0.125,
    0.250,
    0.500,
    0.750
};

SquareWave::SquareWave(int cpuClockRateHz)
{
    this->secondsPerApuCycle = 1.0 / cpuClockRateHz;
}

void SquareWave::ApuClock()
{
    this->elapsedTimeSec += this->secondsPerApuCycle;
}

void SquareWave::WatchdogClock()
{
    if(this->watchdogTimer > 0 && this->haltWatchdog == false)
    {
        --this->watchdogTimer;
    }
}

void SquareWave::EnvelopeClock()
{
    if(this->resetFlag)
    {
        this->volumeDecayEnvelope = 15;
        this->envelopePeriod = this->maxVolumeOrEnvelopePeriod;
        this->resetFlag = false;
        return;
    }

    if(this->envelopePeriod > 0)
    {
        --this->envelopePeriod;
    }
    else
    {
        this->envelopePeriod = this->maxVolumeOrEnvelopePeriod;

        if(this->volumeDecayEnvelope > 0)
        {
            --this->volumeDecayEnvelope;
        }
        else
        {
            if(this->haltWatchdog == true)
            {
                this->volumeDecayEnvelope = 15;
            }
        }
    }
}

float SquareWave::OutputSample()
{
    if(this->watchdogTimer <= 0 || this->frequency <= 0.0f)
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
        double c = n * this->frequency * 2.0 * M_PI * this->elapsedTimeSec;
        a += -BitUtil::approxsin(c) / n;
        b += -BitUtil::approxsin(c - (phase * n)) / n;
    }

    //instead of multipling by 4 we will only multiple by 1 and the add 1 so the output is never above 1 or below 0
    float normalizedSound = (1.0f / M_PI) * (a - b) + 1;

    // now multiply it by the volume multiplier
    float multiplier = 0.0f;
    if(this->constantVolume)
    {
        multiplier = this->maxVolumeOrEnvelopePeriod;
    }
    else
    {
        multiplier = this->volumeDecayEnvelope;
    }
    return multiplier * normalizedSound;
}

void SquareWave::ResetVolumeDecayEnvelope()
{
    this->resetFlag = true;
}
#include <iostream>
void SquareWave::SetFreqFromPeriod(dword period)
{
    // if period < 8, the corresponding pulse channel is silenced.
    // https://wiki.nesdev.com/w/index.php/APU#Pulse_.28.244000-4007.29 
    if(period < 8)
    {
        this->frequency = 0.0f;
        return;
    }
    this->frequency = 1789773.0f / (16.0f * (float)(period + 1));
}

void SquareWave::SetDutyCycle(int dutyCycleNum)
{
    this->dutyCycle = DUTY_CYCLE_TABLE.at(dutyCycleNum);
}

void SquareWave::SetWatchdogTimer(int lengthCounter)
{
    this->watchdogTimer = lengthCounter;
}

void SquareWave::SetWatchdogTimerFromCode(int lengthCounterCode)
{
    this->watchdogTimer = LENGTH_COUNTER_LOOKUP.at(lengthCounterCode);
}

void SquareWave::SetHaltWatchdogTimer(bool haltWatchdog)
{
    this->haltWatchdog = haltWatchdog;
}

void SquareWave::SetMaxVolumeOrEnvelopePeriod(int volume)
{
    this->maxVolumeOrEnvelopePeriod = volume;
}

void SquareWave::SetConstantVolume(bool constantVol)
{
    this->constantVolume = constantVol;
}