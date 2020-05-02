#include "Oam.h"

const int Oam::rawLen;

Oam::Oam()
: MemoryRepeaterArray(0x0, 0x100, this->raw, this->rawLen)
{
    
}