#include "PpuRegisters.h"

const int PpuRegisters::rawLen;

PpuRegisters::PpuRegisters()
: MemoryRepeaterArray(0x2000, 0x3FFF, this->raw, this->rawLen)
{
    memset(this->raw, 0, sizeof(this->name));
}

byte PpuRegisters::Read(dword address)
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
        dword address = this->name.curPpuAddress.val;
        this->name.ppuReadBuffer;
    }
    return MemoryRepeaterArray::Read(address);
}

void PpuRegisters::Write(dword address, byte value)
{
    switch(address)
    {
        case PpuRegisters::OAMDATA_ADDR:
        {
            //TODO
            break;
        }
        case PpuRegisters::PPUSCROLL_ADDR:
        {
            //TODO
            break;
        }
        case PpuRegisters::PPUADDR_ADDR:
        {
            if(this->name.ppuAddressLatch == false)
            {
                this->name.curPpuAddress.upper = value;
            }
            else
            {
                this->name.curPpuAddress.lower = value;
            }
            //flip the vram address latch
            this->name.ppuAddressLatch = !this->name.ppuAddressLatch;
            break;
            case PpuRegisters::PPUDATA_ADDR:
            {
                //TODO
            }
            case PpuRegisters::OAMDMA_ADDR:
            {
                //TODO
            }
        }
        default:
        {
            MemoryRepeaterArray::Write(address, value);
            break;
        }
    }
}