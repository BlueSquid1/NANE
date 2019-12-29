#ifndef PPU
#define PPU

#include "NES/Util/BitUtil.h"

class Ppu
{
    private:
    struct PpuRegisters
    {
        byte temp;
    };
    PpuRegisters r;
    public:
};

#endif