#include "NoiseWave.h"

// values from:
// https://wiki.nesdev.com/w/index.php/APU_Noise
const std::vector<int> NoiseWave::PERIOD_TABLE = 
{
    4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
};

bool NoiseWave::IsMuted()
{
    if(this->isEnabled == false)
    {
        return true;
    }

    if(this->watchdogSeq->GetCounter() <= 0)
    {
        return true;
    }
    return false;
}

NoiseWave::NoiseWave()
{
    this->sudoRandomNumber = 0;

    this->watchdogSeq = std::make_unique<Sequencer>(-1, false, nullptr);

    this->noiseSeq = std::make_unique<Sequencer>(-1, true, [this](){
        byte bitPos = 1;
        if(this->useSixthBit)
        {
            bitPos = 6;
        }
        bit feedback = BitUtil::GetBits(this->sudoRandomNumber, 0) ^ BitUtil::GetBits(this->sudoRandomNumber, bitPos);

        this->sudoRandomNumber >>= 1;

        this->sudoRandomNumber = BitUtil::GetBits(this->sudoRandomNumber, 0, 13) | feedback << 14;
    });
}

void NoiseWave::ApuClock()
{
    noiseSeq->Clock();
}

void NoiseWave::WatchdogClock()
{
    this->watchdogSeq->Clock();
}

void NoiseWave::EnvelopeClock()
{
    this->volEnvelope.Clock();
}

void NoiseWave::ResetVolumeDecayEnvelope()
{
    this->volEnvelope.ResetVolumeDecayEnvelope();
}

float NoiseWave::OutputSample()
{
    if(this->IsMuted())
    {
        return 0.0f;
    }

    float normalizedSound = !BitUtil::GetBits(this->sudoRandomNumber, 0);

    float multipler = this->volEnvelope.CalculateVolume();

    return normalizedSound * multipler;
}

void NoiseWave::SetEnabled(bool isEnabled)
{
    this->isEnabled = isEnabled;
}

void NoiseWave::SetUseSixthBit(bool useSixthBit)
{
    this->useSixthBit = useSixthBit;
}

dword NoiseWave::GetPeriod() const
{
    return this->noiseSeq->GetPeriod();
}

void NoiseWave::SetPeriodFromLookupTable(int tableIndex)
{
    this->noiseSeq->SetPeriod(PERIOD_TABLE.at(tableIndex), true);
}

void NoiseWave::SetWatchdogTimer(int lengthCounter)
{
    this->watchdogSeq->SetPeriod(lengthCounter, true);
}

void NoiseWave::SetWatchdogTimerFromCode(int lengthCounterCode)
{
    this->watchdogSeq->SetPeriod(IWave::LENGTH_COUNTER_LOOKUP.at(lengthCounterCode), true);
}

void NoiseWave::SetHaltWatchdogTimer(bool haltWatchdog)
{
    this->watchdogSeq->SetHaltCounter(haltWatchdog);
    this->volEnvelope.SetIsLooping(haltWatchdog);
}

void NoiseWave::SetMaxVolumeOrEnvelopePeriod(int volume)
{
    this->volEnvelope.SetMaxVolumeOrEnvelopePeriod(volume);
}

void NoiseWave::SetConstantVolume(bool constantVol)
{
    this->volEnvelope.SetConstantVolume(constantVol);
}