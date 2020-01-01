#include "PpuMemoryMap.h"

PpuMemoryMap::PpuMemoryMap()
    : IMemoryRW(0x0000, 0x3FFF)
{

}

byte PpuMemoryMap::Read(dword address) const
{

}

void PpuMemoryMap::Write(dword address, byte value)
{

}