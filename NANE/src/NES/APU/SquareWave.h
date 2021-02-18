#pragma once

class SquareWave
{
    bool enabled;
    int dutyCycle;
    int lengthCounter;

    public:
    void Step();
    float OutputSample();
};