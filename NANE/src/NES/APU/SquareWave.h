#pragma once

#include <memory>
#include <vector>

#include "NES/Memory/BitUtil.h"

#include "ApuRegisters.h"

class SquareWave
{
    private:
    double period = 0;

    //internal counter for when to shift duty cycle
    dword cycleCounter = 0;

    byte curDutyCycle;

    public:
    SquareWave();

    void Step();
    float OutputSample();

    //getters/setters
    void SetPeriod(dword period);
    void SetDutyCycle(int dutyCycleNum);
};