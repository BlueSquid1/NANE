#include "ApuRegisters.h"

ApuRegisters::ApuRegisters()
: MemoryRepeaterArray(0x4000, 0x401F, this->raw, this->rawLen)
{
    memset(this->raw, 0, this->rawLen);
}