#include "PpuRegisters.h"

const int PpuRegisters::rawLen;

PpuRegisters::PpuRegisters()
: MemoryRepeaterArray(0x2000, 0x3FFF, this->raw, this->rawLen)
{
    memset(this->raw, 0, sizeof(this->name));


    this->bgr.curPpuAddress.val = 0;
    this->bgr.ppuAddressLatch = 0;
    this->bgr.ppuDataReadBuffer = 0;
    this->bgr.ntByte = 0;
    this->bgr.atByte = 0;
    this->bgr.tileLo = 0;
    this->bgr.tileHi = 0;
    this->bgr.shift.paletteAttribute1 = 0;
    this->bgr.shift.paletteAttribute2 = 0;
    this->bgr.shift.patternPlane1.val = 0;
    this->bgr.shift.patternPlane2.val = 0;

}

byte PpuRegisters::Read(dword address)
{
    switch(address)
    {
        case PpuRegisters::PPUSTATUS_ADDR:
        {
            //fill the bottom bits with ppu data read buffer
            this->name.verticalBlank = true;
            byte returnValue = (this->name.PPUSTATUS & 0xE0) | (this->bgr.ppuDataReadBuffer & 0x1F);
            this->name.verticalBlank = false;
            this->bgr.ppuAddressLatch = false;
            return returnValue;
        }
    }
    return this->Seek(address);
    /*
    else if(address == PpuRegisters::OAMDATA_ADDR)
    {
        //TODO
    }
    return MemoryRepeaterArray::Read(address);
    */
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
            if(this->bgr.ppuAddressLatch == false)
            {
                this->bgr.curPpuAddress.upper = value;
            }
            else
            {
                this->bgr.curPpuAddress.lower = value;
            }
            //flip the vram address latch
            this->bgr.ppuAddressLatch = !this->bgr.ppuAddressLatch;
            break;
        }
        case PpuRegisters::OAMDMA_ADDR:
        {
            //TODO
        }
        default:
        {
            MemoryRepeaterArray::Write(address, value);
            break;
        }
    }
}

byte PpuRegisters::Seek(dword address) const
{
    return MemoryRepeaterArray::Seek(address);
}