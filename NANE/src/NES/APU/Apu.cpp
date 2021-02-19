#include <math.h>

#include "Apu.h"

Apu::Apu(int samplesPerSecond, std::shared_ptr<Dma> dma)
{
    this->dma = dma;
    this->audioSamplesPerSecond = samplesPerSecond;
    this->audioStream = std::make_shared<ThreadSafeQueue<float>>(this->audioSamplesPerSecond);
}

void Apu::Step()
{
    //step through channels
    const long long& totalClockCycles = this->dma->GetApuMemory().GetTotalApuCycles();
    if(totalClockCycles % 2 == 0)
    {
        //APU runs at half the CPU clock speed
        //TODO
    }

    int cyclesPerSample = this->clockRateHz / this->audioSamplesPerSecond;
    if(totalClockCycles % cyclesPerSample == 0)
    {
        // time to generate an output sample
        float sample = this->dma->GetApuMemory().GetSquareWave1().OutputSample(this->CalRealTimeSec());

        this->audioStream->Push(sample);
    }

    // increment APU clock counter
    this->dma->GetApuMemory().SetTotalApuCycles(totalClockCycles + 1);
}

std::shared_ptr<ThreadSafeQueue<float>> Apu::GetAudio()
{
    return this->audioStream;
}

float Apu::CalRealTimeSec()
{
    float secondsPerApuCycle = 1.0f/this->clockRateHz;
    return secondsPerApuCycle * this->dma->GetApuMemory().GetTotalApuCycles();
}