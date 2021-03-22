#pragma once

class TriangleWave
{
    private:
    float previousOutput = 0.0f; //stores previous output. Used when channel is disabled.

    public:
    //number between 0.0 to 15.0
    float OutputSample();
};