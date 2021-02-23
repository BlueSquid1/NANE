#include <math.h>

#include "Apu.h"

Apu::Apu(int samplesPerSecond, std::shared_ptr<Dma> dma)
{
    this->dma = dma;
    this->audioSamplesPerSecond = samplesPerSecond;
    this->audioStream = std::make_shared<ThreadSafeQueue<float>>(this->audioSamplesPerSecond/ 5);
}

void Apu::Step()
{
    //step through channels
    const long long& totalClockCycles = this->dma->GetApuMemory().GetTotalApuCycles();
    if(totalClockCycles % 2 == 0)
    {
        //APU runs at half the CPU clock speed
        this->dma->GetApuMemory().GetSquareWave1().Clock();
    }

    int cyclesPerSample = this->dma->GetApuMemory().GetCpuClockRateHz() / this->audioSamplesPerSecond;
    if(totalClockCycles % cyclesPerSample == 0)
    {
        float sample = this->dma->GetApuMemory().GetSquareWave1().OutputSample();

        this->audioStream->Push(sample);
    }

    // increment APU clock counter
    this->dma->GetApuMemory().SetTotalApuCycles(totalClockCycles + 1);
}

std::shared_ptr<ThreadSafeQueue<float>> Apu::GetAudio()
{
    return this->audioStream;
}