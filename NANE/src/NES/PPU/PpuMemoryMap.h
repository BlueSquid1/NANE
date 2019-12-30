#ifndef PPU_MEMORY_MAP
#define PPU_MEMORY_MAP

#include "NES/Util/BitUtil.h"
#include "NES/Memory/IMemoryRW.h"

class PpuMemoryMap
{
    private:
    public:
    virtual byte Read(dword address);
    virtual void Write(dword address, byte value);
};
#endif