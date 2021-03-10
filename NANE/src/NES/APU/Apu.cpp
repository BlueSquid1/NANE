#include <math.h>

#include "Apu.h"

Apu::Apu(int samplesPerSecond, std::shared_ptr<Dma> dma)
{
    this->dma = dma;
    this->audioSamplesPerSecond = samplesPerSecond;
    this->audioStream = std::make_shared<ThreadSafeQueue<float>>(this->audioSamplesPerSecond/ 5);
}

bool Apu::PowerCycle()
{
    bool apuMemPowerCycle = this->dma->GetApuMemory().PowerCycle();

    if(apuMemPowerCycle == false)
    {
        return false;
    }
    return true;
}

void Apu::Step()
{
    //step through channels
    const long long& totalClockCycles = this->dma->GetApuMemory().GetTotalApuCycles();
    if(totalClockCycles % 2 == 0)
    {
        //APU runs at half the CPU clock speed
        this->dma->GetApuMemory().GetSquareWave1().ApuClock();
        this->dma->GetApuMemory().GetSquareWave2().ApuClock();
    }

    int cyclesPerFrameCounter = this->dma->GetApuMemory().GetCpuClockRateHz() / this->dma->GetApuMemory().GetFrameCounterRateHz();
    if(totalClockCycles % cyclesPerFrameCounter == 0)
    {
        switch(this->dma->GetApuMemory().GetRegisters().vRegs.frameCounterSeqNum % 4)
        {
            case 0:
            {
                this->dma->GetApuMemory().GetSquareWave1().EnvelopeClock();
                this->dma->GetApuMemory().GetSquareWave2().EnvelopeClock();
                break;
            }
            case 1:
            {
                this->dma->GetApuMemory().GetSquareWave1().EnvelopeClock();
                this->dma->GetApuMemory().GetSquareWave2().EnvelopeClock();

                this->dma->GetApuMemory().GetSquareWave1().WatchdogClock();
                this->dma->GetApuMemory().GetSquareWave1().WatchdogClock();
                break;
            }
            case 2:
            {
                this->dma->GetApuMemory().GetSquareWave1().EnvelopeClock();
                this->dma->GetApuMemory().GetSquareWave2().EnvelopeClock();
                break;
            }
            case 3:
            {
                this->dma->GetApuMemory().GetSquareWave1().EnvelopeClock();
                this->dma->GetApuMemory().GetSquareWave2().EnvelopeClock();
                
                this->dma->GetApuMemory().GetSquareWave1().WatchdogClock();
                this->dma->GetApuMemory().GetSquareWave1().WatchdogClock();
                break;
            }
        }
        ++this->dma->GetApuMemory().GetRegisters().vRegs.frameCounterSeqNum;
    }

    int cyclesPerSample = this->dma->GetApuMemory().GetCpuClockRateHz() / this->audioSamplesPerSecond;
    if(totalClockCycles % cyclesPerSample == 0)
    {
        float sq1Sample = this->dma->GetApuMemory().GetSquareWave1().OutputSample();
        float sq2Sample = this->dma->GetApuMemory().GetSquareWave2().OutputSample();

        float sample = this->MixChannels(sq1Sample, sq2Sample);

        this->audioStream->Push(sample);
    }

    // increment APU clock counter
    this->dma->GetApuMemory().SetTotalApuCycles(totalClockCycles + 1);
}

std::shared_ptr<ThreadSafeQueue<float>> Apu::GetAudio()
{
    return this->audioStream;
}

float Apu::MixChannels(float sq1, float sq2)
{
    float pulseOutput = (95.88 * (sq1 + sq2)) / (8128 + (100 * (sq1 + sq2)));

    float audioOutput = pulseOutput;
    return audioOutput;
}