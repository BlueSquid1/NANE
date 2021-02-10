#include <math.h>

#include "Apu.h"

//TODO
#define PI 3.14159265

Apu::Apu(int samplesPerSecond)
{
    this->samplesPerSecond = samplesPerSecond;
    this->audioStream = std::make_shared<ThreadSafeQueue<float>>(this->samplesPerSecond);
}

void Apu::Step()
{
    //this method is called 1786800 per second
    int cyclesPerSample = this->clockRate / this->samplesPerSecond;
    if(this->totalClockCycles % cyclesPerSample == 0)
    {
        //for each sample
        float frequency = 300; //Hz

        float sample = 0.3 * sin(frequency * timeElapsed * 2 * PI);

        double secondsPerSample = 1.0 / this->samplesPerSecond;
        timeElapsed += secondsPerSample;

        this->audioStream->Push(sample);
    }
    ++totalClockCycles;
}

std::shared_ptr<ThreadSafeQueue<float>> Apu::GetAudio()
{
    return this->audioStream;
}