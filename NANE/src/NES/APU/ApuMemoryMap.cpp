#include "ApuMemoryMap.h"

namespace
{
    const int JOYSTICK_STROBE_ADDRESS = 0x4016;
}

ApuMemoryMap::ApuMemoryMap()
: IMemoryRW(0x4000, 0x4017),
  sq1(cpuClockRateHz)
{
}

byte ApuMemoryMap::Read(dword address)
{
    return this->apuRegMem.Read(address);
}

void ApuMemoryMap::Write(dword address, byte value)
{
    this->apuRegMem.Write(address, value);

    switch(address)
    {
        case ApuRegisters::ApuAddresses::SQ1_VOL_ADDR:
        {
            this->sq1.SetDutyCycle(this->apuRegMem.name.SQ1.dutyNum);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ1_LO_ADDR:
        {
            dword period = (this->apuRegMem.name.SQ1.HI & 0x7 << 8) | this->apuRegMem.name.SQ1.LO;
            this->sq1.SetFreqFromPeriod(period);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ1_HI_ADDR:
        {
            dword period = (this->apuRegMem.name.SQ1.HI & 0x7 << 8) | this->apuRegMem.name.SQ1.LO;
            this->sq1.SetFreqFromPeriod(period);
            this->sq1.SetDutyCycle(0);
            break;
        }
        case ApuRegisters::ApuAddresses::SND_CHN_ADDR:
        {
            this->sq1.SetEnabled(this->apuRegMem.name.enableStatus.pulse1);
            break;
        }
    }
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

int ApuMemoryMap::GetCpuClockRateHz()
{
    return this->cpuClockRateHz;
}

const long long& ApuMemoryMap::GetTotalApuCycles() const
{
    return this->totalApuCycles;
}

void ApuMemoryMap::SetTotalApuCycles(const long long& value)
{
    this->totalApuCycles = value;
}

ApuRegisters& ApuMemoryMap::GetRegisters()
{
    return this->apuRegMem;
}

SquareWave& ApuMemoryMap::GetSquareWave1()
{
    return this->sq1;
}