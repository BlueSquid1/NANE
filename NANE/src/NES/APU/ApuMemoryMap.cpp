#include "ApuMemoryMap.h"

namespace
{
    const int JOYSTICK_STROBE_ADDRESS = 0x4016;
}

ApuMemoryMap::ApuMemoryMap()
: IMemoryRW(0x4000, 0x4017),
  sq1(cpuClockRateHz),
  sq2(cpuClockRateHz)
{
}

bool ApuMemoryMap::PowerCycle()
{
    bool regReturn = this->GetRegisters().PowerCycle();
    if(regReturn == false)
    {
        return false;
    }

    this->sq1.SetEnabled(false);
    this->sq2.SetEnabled(false);
    //TODO
    return true;
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
            this->sq1.SetHaltWatchdogTimer(this->apuRegMem.name.SQ1.lengthCounterHault);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ1_LO_ADDR:
        {
            dword period = (this->apuRegMem.name.SQ1.timerHigh & 0x7 << 8) | this->apuRegMem.name.SQ1.LO;
            this->sq1.SetFreqFromPeriod(period);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ1_HI_ADDR:
        {
            dword period = (this->apuRegMem.name.SQ1.timerHigh & 0x7 << 8) | this->apuRegMem.name.SQ1.LO;
            this->sq1.SetFreqFromPeriod(period);
            this->sq1.SetDutyCycle(0);
            this->sq1.SetWatchdogTimer(this->apuRegMem.name.SQ1.lengthCounter);
            break;
        }

        case ApuRegisters::ApuAddresses::SQ2_VOL_ADDR:
        {
            this->sq2.SetDutyCycle(this->apuRegMem.name.SQ2.dutyNum);
            this->sq2.SetHaltWatchdogTimer(this->apuRegMem.name.SQ1.lengthCounterHault);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ2_LO_ADDR:
        {
            dword period = (this->apuRegMem.name.SQ2.timerHigh & 0x7 << 8) | this->apuRegMem.name.SQ2.LO;
            this->sq2.SetFreqFromPeriod(period);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ2_HI_ADDR:
        {
            dword period = (this->apuRegMem.name.SQ2.timerHigh & 0x7 << 8) | this->apuRegMem.name.SQ2.LO;
            this->sq2.SetFreqFromPeriod(period);
            this->sq2.SetDutyCycle(0);
            this->sq2.SetWatchdogTimer(this->apuRegMem.name.SQ2.lengthCounter);
            break;
        }
        case ApuRegisters::ApuAddresses::SND_CHN_ADDR:
        {
            bool sq1Enabled = this->apuRegMem.name.enableStatus.pulse1;
            this->sq1.SetEnabled(sq1Enabled);
            if(sq1Enabled == false)
            {
                this->sq1.SetWatchdogTimer(0);
            }

            bool sq2Enabled = this->apuRegMem.name.enableStatus.pulse2;
            this->sq2.SetEnabled(sq2Enabled);
            if(sq2Enabled == false)
            {
                this->sq2.SetWatchdogTimer(0);
            }
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

int ApuMemoryMap::GetFrameCounterRateHz()
{
    return this->frameCounterRateHz;
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

SquareWave& ApuMemoryMap::GetSquareWave2()
{
    return this->sq2;
}