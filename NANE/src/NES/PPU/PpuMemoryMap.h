#ifndef PPU_MEMORY_MAP
#define PPU_MEMORY_MAP

#include "NES/Util/BitUtil.h"

class PpuMemoryMap
{
    private:
    public:
    byte Read(dword address);
    void Write(dword address, byte value);
};
#endif