#include "PpuRegisters.h"

const int PpuRegisters::rawLen;

PpuRegisters::PpuRegisters()
: MemoryRepeaterArray(0x2000, 0x3FFF, this->raw, this->rawLen)
{
    memset(this->raw, 0, sizeof(this->name));
}

byte PpuRegisters::Read(dword address) const
{
    if(address == PpuRegisters::PPUSTATUS_ADDR)
    {
        //TODO
    }
    else if(address == PpuRegisters::OAMDATA_ADDR)
    {
        //TODO
    }
    else if(address == PpuRegisters::PPUDATA_ADDR)
    {
        //TODO
    }
    return MemoryRepeaterArray::Read(address);
}

void PpuRegisters::Write(dword address, byte value)
{
    if(address == PpuRegisters::OAMDATA_ADDR)
    {
        //TODO
    }
    else if(address == PpuRegisters::PPUSCROLL_ADDR)
    {
        //TODO
    }
    else if(address == PpuRegisters::PPUADDR_ADDR)
    {
        if(this->name.ppuRegLatch == false)
        {
            this->name.T.upper = value & 0x3F;
        }
        else
        {
            this->name.T.lower = value;
            this->name.V.value = this->name.T.value;
        }
        //flip the vram address latch
        this->name.ppuRegLatch = !this->name.ppuRegLatch;
    }
    else if(address == PpuRegisters::PPUDATA_ADDR)
    {
        //TODO
    }
    return MemoryRepeaterArray::Write(address, value);
}