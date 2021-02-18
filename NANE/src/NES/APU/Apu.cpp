#include <math.h>

#include "Apu.h"

Apu::Apu(int samplesPerSecond, std::shared_ptr<Dma> dma)
{
    this->dma = dma;
    this->audioSamplesPerSecond = samplesPerSecond;
    this->audioStream = std::make_shared<ThreadSafeQueue<float>>(this->audioSamplesPerSecond);
}

ApuRegisters& Apu::GetRegs()
{
    return this->dma->GetApuMemory().GetApuRegisters();
}

void Apu::Step()
{
    //step through channels
    const long long& totalClockCycles = this->dma->GetApuMemory().GetTotalApuCycles();
    if(totalClockCycles % 2 == 0)
    {
        //Pulse channels run at half the CPU clock speed
        this->GetRegs().vRegs.sq1.Step();
    }

    int cyclesPerSample = this->clockRateHz / this->audioSamplesPerSecond;
    if(totalClockCycles % cyclesPerSample == 0)
    {
        // time to generate an output sample
        float sample = this->GetRegs().vRegs.sq1.OutputSample();

        this->audioStream->Push(sample);
    }
    this->dma->GetApuMemory().SetTotalApuCycles(totalClockCycles + 1);
}

std::shared_ptr<ThreadSafeQueue<float>> Apu::GetAudio()
{
    return this->audioStream;
}