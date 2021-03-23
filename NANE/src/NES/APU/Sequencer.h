#pragma once

#include <functional>

class Sequencer
{
    private:
    int counter = 0;
    int period;
    bool isLooping; //true: reset counter to period when it reaches 0, false don't.
    bool haltCounter = false; //won't decrement counter on future Clock() calls

    std::function<void(void)> expireHandle;

    public:
    Sequencer(int period, bool isLooping, std::function<void(void)> expireHandle = nullptr);

    void Clock();
    int GetCounter() const;
    int GetPeriod() const;
    void SetPeriod(int period, bool resetCounter);
    void ResetCounterToPeriod();
    bool GetHaltCounter() const;
    void SetHaltCounter(bool haltCounter);
};