#include <math.h>

#include "SquareWave.h"

const std::vector<std::vector<bool>> SquareWave::DUTY_CYCLE_TABLE = 
{
    { 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 0, 0, 0 },
    { 1, 0, 0, 1, 1, 1, 1, 1 }
};

bool SquareWave::IsOutputMuted()
{
    if(this->isEnabled == false)
    {
        return true;
    }

    if(this->watchDogSeq->GetCounter() <= 0 )
    {
        return true;
    }

    if(this->DUTY_CYCLE_TABLE.at(this->dutyCycleNum).at(this->sequencePos) == false)
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
    dword target_period = this->pulseSeq->GetPeriod();

    dword changeAmount = target_period >> this->shiftPeriodAmount;
    if(this->isNegative == false)
    {
        target_period += changeAmount;
    }
    else
    {
        if(!this->isPulse2)
        {
            // for pulse 1 change amount is made negative by 1's complement
            target_period -= changeAmount - 1;
        }
        else
        {
            // for pulse 2 change amount is made negative by 2's complement
            target_period -= changeAmount;
        }
    }
    return target_period;
}

SquareWave::SquareWave(bool isPulse2)
{
    this->isPulse2 = isPulse2;

    this->watchDogSeq = std::make_unique<Sequencer>(-1, false, nullptr);

    this->pulseSeq = std::make_unique<Sequencer>(-1, true, [this]()
    {
        this->sequencePos = (this->sequencePos + 1) % 8;
    });

    this->sweepSeq = std::make_unique<Sequencer>(-1, true, [this]()
    {
        if(this->frequencySweepEnabled)
        {
            this->pulseSeq->SetPeriod(this->CalTargetPeriod(), false);
        }
    });
}

void SquareWave::ApuClock()
{
    this->pulseSeq->Clock();
}

void SquareWave::WatchdogClock()
{
    watchDogSeq->Clock();
}

void SquareWave::EnvelopeClock()
{
    this->volEnvelope.Clock();
}

void SquareWave::SweepClock()
{
    if(this->sweepResetFlag)
    {
        this->sweepSeq->SetPeriod(this->sweepSeq->GetPeriod(), true);
        this->sweepResetFlag = false;
        return;
    }

    this->sweepSeq->Clock();
}

float SquareWave::OutputSample()
{
    if(this->IsOutputMuted())
    {
        return 0.0f;
    }

    float normalizedSound = this->DUTY_CYCLE_TABLE.at(this->dutyCycleNum).at(this->sequencePos);

    // now multiply it by the volume multiplier
    float multiplier = this->volEnvelope.CalculateVolume();
    return multiplier * normalizedSound;
}

void SquareWave::ResetVolumeDecayEnvelope()
{
    this->volEnvelope.ResetVolumeDecayEnvelope();
}

void SquareWave::SetEnabled(bool isEnabled)
{
    this->isEnabled = isEnabled;
}

dword SquareWave::GetPulsePeriod()
{
    return this->pulseSeq->GetPeriod();
}

void SquareWave::SetPulsePeriod(dword period)
{
    this->pulseSeq->SetPeriod(period, false);
}

void SquareWave::SetDutyCycle(int dutyCycleNum)
{
    this->dutyCycleNum = dutyCycleNum;
}

int SquareWave::GetWatchdogTimer()
{
    return this->watchDogSeq->GetCounter();
}

void SquareWave::SetWatchdogTimer(int lengthCounter)
{
    this->watchDogSeq->SetPeriod(lengthCounter, true);
}

void SquareWave::SetWatchdogTimerFromCode(int lengthCounterCode)
{
    this->watchDogSeq->SetPeriod(IWave::LENGTH_COUNTER_LOOKUP.at(lengthCounterCode), true);
}

void SquareWave::SetHaltWatchdogTimer(bool haltWatchdog)
{
    this->watchDogSeq->SetHaltCounter(haltWatchdog);
    this->volEnvelope.SetIsLooping(haltWatchdog);
}

void SquareWave::SetMaxVolumeOrEnvelopePeriod(int volume)
{
    this->volEnvelope.SetMaxVolumeOrEnvelopePeriod(volume);
}

void SquareWave::SetConstantVolume(bool constantVol)
{
    this->volEnvelope.SetConstantVolume(constantVol);
}

void SquareWave::SetFrequencySweep(bool isEnabled, byte sweepUnitPeriod, bool isNegative, byte shiftPeriodAmount)
{
    this->frequencySweepEnabled = isEnabled;
    this->sweepSeq->SetPeriod(sweepUnitPeriod, false);
    this->isNegative = isNegative;
    this->shiftPeriodAmount = shiftPeriodAmount;
    this->sweepResetFlag = true;
}