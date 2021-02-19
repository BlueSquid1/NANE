#include "SquareWave.h"

namespace
{
    std::vector<byte> DUTY_CYCLE_TABLE = 
    {
        0x40, // 0 1 0 0 0 0 0 0
        0x60, // 0 1 1 0 0 0 0 0
        0x78, // 0 1 1 1 1 0 0 0
        0x9F  // 1 0 0 1 1 1 1 1
    };
}

SquareWave::SquareWave()
{
    this->curDutyCycle = DUTY_CYCLE_TABLE[0];
}

void SquareWave::Step()
{
    if(this->cycleCounter == 0)
    {
        //rotate right
        curDutyCycle = (curDutyCycle >> 1) | ((curDutyCycle & 0x01) << 7);

        this->cycleCounter = period;
    }

    this->cycleCounter--;
}

float SquareWave::OutputSample()
{
    return BitUtil::GetBits(curDutyCycle, 0);
}

void SquareWave::SetPeriod(dword period)
{
    this->period = period;
}

void SquareWave::SetDutyCycle(int dutyCycleNum)
{
    this->curDutyCycle = DUTY_CYCLE_TABLE.at(dutyCycleNum);
}