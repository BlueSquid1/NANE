#include "ApuMemoryMap.h"

namespace
{
    const int JOYSTICK_STROBE_ADDRESS = 0x4016;
}

ApuMemoryMap::ApuMemoryMap()
: IMemoryRW(0x4000, 0x4017)
{
    
}

byte ApuMemoryMap::Read(dword address)
{
    return this->apuRegMem.Read(address);
}

void ApuMemoryMap::Write(dword address, byte value)
{
    this->apuRegMem.Write(address, value);
}

byte ApuMemoryMap::Seek(dword address) const
{
    return this->apuRegMem.Seek(address);
}

bool ApuMemoryMap::Contains(dword address) const
{
    if(address == JOYSTICK_STROBE_ADDRESS)
    {
        return false;
    }
    return IMemoryRW::Contains(address);
}

const long long& ApuMemoryMap::GetTotalApuCycles() const
{
    return this->totalApuCycles;
}

void ApuMemoryMap::SetTotalApuCycles(const long long& value)
{
    this->totalApuCycles = value;
}

ApuRegisters& ApuMemoryMap::GetApuRegisters()
{
    return this->apuRegMem;
}