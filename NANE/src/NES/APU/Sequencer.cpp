#include "Sequencer.h"

Sequencer::Sequencer(int period, bool isLooping, std::function<void(void)> expireHandle)
{
    this->counter = period;
    this->period = period;
    this->isLooping = isLooping;
    this->expireHandle = expireHandle;
}

void Sequencer::Clock()
{
    if(this->counter <= 0 && this->period > 0)
    {
        //reset if needed
        if(this->period > 0 && this->isLooping)
        {
            this->counter = this->period;
        }

        //call expire handle
        if(this->expireHandle != nullptr)
        {
            this->expireHandle();
        }
        return;
    }
    this->counter--;
}

int Sequencer::GetCounter() const
{
    return this->counter;
}

int Sequencer::GetPeriod() const
{
    return this->period;
}

void Sequencer::SetPeriod(int period, bool resetCounter)
{
    this->period = period;
    if(resetCounter)
    {
        this->counter = period;
    }
}

bool Sequencer::GetHaltCounter() const
{
    return this->haltCounter;
}

void Sequencer::SetHaltCounter(bool haltCounter)
{
    this->haltCounter = haltCounter;
}