#include "PpuRegisters.h"

const int PpuRegisters::rawLen;

PpuRegisters::PpuRegisters()
: MemoryRepeaterArray(0x2000, 0x3FFF, this->raw, this->rawLen)
{
    memset(this->raw, 0, sizeof(this->name));
}

byte PpuRegisters::Read(dword address) const
{
    return MemoryRepeaterArray::Read(address);
}

void PpuRegisters::Write(dword address, byte value)
{
    if(address == 0x2006) //0x2006 = PPUADDR
    {
        if(this->name.vramAddrLatchLower == false)
        {
            this->name.T.upper = value & 0x3F;
        }
        else
        {
            this->name.T.lower = value;
            this->name.V.value = this->name.T.value;
        }
        //flip the vram address latch
        this->name.vramAddrLatchLower = !this->name.vramAddrLatchLower;
    }
    return MemoryRepeaterArray::Write(address, value);
}