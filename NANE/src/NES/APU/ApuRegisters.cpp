#include "ApuRegisters.h"

const int ApuRegisters::rawLen;

ApuRegisters::ApuRegisters()
: MemoryRepeaterArray(0x4000, 0x4017, this->raw, this->rawLen)
{
    this->PowerCycle();
}

bool ApuRegisters::PowerCycle()
{
    memset(this->raw, 0, this->rawLen);

    this->vRegs.frameCounterSeqNum = 0;
    return true;
}