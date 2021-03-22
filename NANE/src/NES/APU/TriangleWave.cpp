#include "TriangleWave.h"

float TriangleWave::OutputSample()
{
    float sample = 0.0f;
    this->previousOutput = sample;
    return sample;
}