#include <math.h>

#include "SquareWave.h"

const std::vector<int> SquareWave::LENGTH_COUNTER_LOOKUP = 
{
     10, 254, 20,  2, 40,  4, 80,  6,
    160,   8, 60, 10, 14, 12, 26, 14,
     12,  16, 24, 18, 48, 20, 96, 22,
    192,  24, 72, 26, 16, 28, 32, 30
};

const std::vector<std::vector<bool>> SquareWave::DUTY_CYCLE_TABLE = 
{
    { 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 0, 0, 0 },
    { 1, 0, 0, 1, 1, 1, 1, 1 }
};

SquareWave::SquareWave(int cpuClockRateHz)
{
    this->secondsPerApuCycle = 1.0 / cpuClockRateHz;
}

void SquareWave::ApuClock()
{
    if(this->timerVal <= 0)
    {
        this->timerVal = this->pulsePeriod;
        this->sequencePos = (this->sequencePos + 1) % 8;
        return;
    }
    --this->timerVal;
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
    if(this->volumeResetFlag)
    {
        this->volumeDecayEnvelope = 15;
        this->envelopePeriod = this->maxVolumeOrEnvelopePeriod;
        this->volumeResetFlag = false;
        return;
    }

    if(this->envelopePeriod <= 0)
    {
        this->envelopePeriod = this->maxVolumeOrEnvelopePeriod;

        if(this->volumeDecayEnvelope <= 0)
        {
            if(this->haltWatchdog == true)
            {
                this->volumeDecayEnvelope = 15;
            }
            return;
        }

        --this->volumeDecayEnvelope;
        return;
    }

    --this->envelopePeriod;
}

void SquareWave::SweepClock()
{
    if(this->sweepResetFlag)
    {
        this->sweepCounter = this->sweepUnitPeriod + 1;
        this->sweepResetFlag = false;
        return;
    }

    if(this->sweepCounter <= 0)
    {
        this->sweepCounter = this->sweepUnitPeriod + 1;

        if(this->frequencySweepEnabled)
        {
            this->pulsePeriod = this->CalTargetPeriod();
        }
        return;
    }
    --this->sweepCounter;
}

float SquareWave::OutputSample()
{
    if(this->IsOutputMuted())
    {
        return 0.0f;
    }

    float normalizedSound = this->DUTY_CYCLE_TABLE.at(this->dutyCycleNum).at(this->sequencePos);

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
    this->volumeResetFlag = true;
}

bool SquareWave::IsOutputMuted()
{
    if(this->isEnabled == false)
    {
        return true;
    }

    if(this->watchdogTimer <= 0 )
    {
        return true;
    }

    // if period < 8, the corresponding pulse channel is silenced.
    // https://wiki.nesdev.com/w/index.php/APU#Pulse_.28.244000-4007.29
    // also if the period is > 0x7FF then it is silenced.
    // https://wiki.nesdev.com/w/index.php/APU_Sweep
    dword targetPeriod = this->CalTargetPeriod();
    if(targetPeriod < 8 || targetPeriod > 0x7FF)
    {
        return true;
    }

    return false;
}

dword SquareWave::CalTargetPeriod() const
{
    dword target_period = this->pulsePeriod;

    dword changeAmount = this->pulsePeriod >> this->shiftPeriodAmount;
    if(this->isNegative == false)
    {
        target_period += changeAmount;
    }
    else
    {
        target_period -= changeAmount;
    }
    return target_period;
}

void SquareWave::SetIsEnabled(bool isEnabled)
{
    this->isEnabled = isEnabled;
}

dword SquareWave::GetPulsePeriod()
{
    return this->pulsePeriod;
}

void SquareWave::SetPulsePeriod(dword period)
{
    this->pulsePeriod = period;
}

void SquareWave::SetDutyCycle(int dutyCycleNum)
{
    this->dutyCycleNum = dutyCycleNum;
}

int SquareWave::GetWatchdogTimer()
{
    return this->watchdogTimer;
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

void SquareWave::SetFrequencySweep(bool isEnabled, byte sweepUnitPeriod, bool isNegative, byte shiftPeriodAmount)
{
    this->frequencySweepEnabled = isEnabled;
    this->sweepUnitPeriod = sweepUnitPeriod;
    this->isNegative = isNegative;
    this->shiftPeriodAmount = shiftPeriodAmount;
    this->sweepResetFlag = true;
}