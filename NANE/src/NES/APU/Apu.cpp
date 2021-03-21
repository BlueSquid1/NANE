#include <math.h>

#include "Apu.h"

Apu::Apu(int samplesPerSecond, std::shared_ptr<Dma> dma)
{
    this->dma = dma;
    this->audioSamplesPerSecond = samplesPerSecond;
    this->audioStream = std::make_shared<ThreadSafeQueue<float>>(this->audioSamplesPerSecond/ 5);

    //TODO
    // Setup filters
    this->filters[0] = new HiPassFilter (90,    1789773);
    this->filters[1] = new HiPassFilter (440,   1789773);
    this->filters[2] = new LoPassFilter (14000, 1789773);
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
    const long long& totalClockCycles = this->dma->GetApuMemory().GetTotalApuCycles();

    //handle special case when frame counter has been written too
    if(this->dma->GetApuMemory().GetResetFrameCounter())
    {
        this->ClockChannels(totalClockCycles);
        this->ClockWatchdogs();
        this->ClockFreqSweeps();
        this->dma->GetApuMemory().SetResetFrameCounter(false);
    }

    //clock channels (every CPU cycle)
    this->ClockChannels(totalClockCycles);

    //Clock frame counter (roughly at 240Hz)
    this->ClockFrameCounter(totalClockCycles);

    //collect output if needed
    int cyclesPerSample = this->dma->GetApuMemory().GetCpuClockRateHz() / this->audioSamplesPerSecond;
    if(totalClockCycles % cyclesPerSample == 0)
    {
        float sq1Sample = this->dma->GetApuMemory().GetSquareWave1().OutputSample();
        float sq2Sample = this->dma->GetApuMemory().GetSquareWave2().OutputSample();

        float sample = this->MixChannels(sq1Sample, sq2Sample);

        //sample = this->Filter(sample);
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

float Apu::Filter(float sample)
{
    for (FirstOrderFilter* filter : this->filters)
    {
        sample = filter->process(sample);
    }
    return sample;
}

void Apu::ClockFrameCounter(const long long& apuClockCycle)
{
    int cyclesPerFrameCounter = this->dma->GetApuMemory().GetCpuClockRateHz() / this->dma->GetApuMemory().GetFrameCounterRateHz();
    if(apuClockCycle % cyclesPerFrameCounter == 0)
    {
        if(this->dma->GetApuMemory().GetRegisters().name.is4StepMode)
        {
            // 4 step mode
            switch(this->dma->GetApuMemory().GetRegisters().vRegs.frameCounterSeqNum % 4)
            {
                case 0:
                {
                    this->ClockEnvelopes();
                    break;
                }
                case 1:
                {
                    this->ClockEnvelopes();
                    this->ClockWatchdogs();
                    this->ClockFreqSweeps();
                    break;
                }
                case 2:
                {
                    this->ClockEnvelopes();
                    break;
                }
                case 3:
                {
                    this->ClockEnvelopes();
                    this->ClockWatchdogs();
                    this->ClockFreqSweeps();
                    if(this->dma->GetApuMemory().GetRegisters().name.irq_inhibit == false)
                    {
                        this->dma->SetIrq(true);
                    }
                    break;
                }
            }
        }
        else
        {
            // 5 step mode
            switch(this->dma->GetApuMemory().GetRegisters().vRegs.frameCounterSeqNum % 5)
            {
                case 0:
                {
                    this->ClockEnvelopes();
                    break;
                }
                case 1:
                {
                    this->ClockEnvelopes();
                    this->ClockWatchdogs();
                    this->ClockFreqSweeps();
                    break;
                }
                case 2:
                {
                    this->ClockEnvelopes();
                    break;
                }
                case 3:
                {
                    // Do nothing
                    break;
                }
                case 4:
                {
                    this->ClockEnvelopes();
                    this->ClockWatchdogs();
                    this->ClockFreqSweeps();
                    break;
                }
            }
        }
        ++this->dma->GetApuMemory().GetRegisters().vRegs.frameCounterSeqNum;
    }
}

void Apu::ClockChannels(const long long& apuClockCycle)
{
    if(apuClockCycle % 2 == 0)
    {
        //APU runs at half the CPU clock speed
        this->dma->GetApuMemory().GetSquareWave1().ApuClock();
        this->dma->GetApuMemory().GetSquareWave2().ApuClock();
    }
}

void Apu::ClockEnvelopes()
{
    this->dma->GetApuMemory().GetSquareWave1().EnvelopeClock();
    this->dma->GetApuMemory().GetSquareWave2().EnvelopeClock();
}

void Apu::ClockWatchdogs()
{
    this->dma->GetApuMemory().GetSquareWave1().WatchdogClock();
    this->dma->GetApuMemory().GetSquareWave1().WatchdogClock();
}

void Apu::ClockFreqSweeps()
{
    this->dma->GetApuMemory().GetSquareWave1().SweepClock();
    this->dma->GetApuMemory().GetSquareWave2().SweepClock();
}