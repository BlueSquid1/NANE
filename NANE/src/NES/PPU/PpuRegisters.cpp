#include "PpuRegisters.h"

const int PpuRegisters::rawLen;

PpuRegisters::PpuRegisters()
: MemoryRepeaterArray(0x2000, 0x3FFF, this->raw, this->rawLen)
{
    memset(this->raw, 0, sizeof(this->name));


    this->bgr.vramPpuAddress.val = 0;
    this->bgr.ppuAddressLatch = false;
    this->bgr.scrollX.val = 0;
    this->bgr.scrollY.val = 0;
    this->bgr.ppuScrollLatch = false;
    this->bgr.ppuDataReadBuffer = 0;
    //todo
    // this->bgr.ntByte = 0;
    // this->bgr.atByte = 0;
    // this->bgr.tileLo = 0;
    // this->bgr.tileHi = 0;
    // this->bgr.shift.paletteAttribute1 = 0;
    // this->bgr.shift.paletteAttribute2 = 0;
    // this->bgr.shift.patternPlane1.val = 0;
    // this->bgr.shift.patternPlane2.val = 0;

}

byte PpuRegisters::Read(dword address)
{
    switch(address)
    {
        case PpuRegisters::PPUSTATUS_ADDR:
        {
            //fill the bottom bits with ppu data read buffer
            byte returnValue = (this->name.PPUSTATUS & 0xE0) | (this->bgr.ppuDataReadBuffer & 0x1F);
            this->name.verticalBlank = false;
            this->bgr.ppuAddressLatch = false;
            return returnValue;
        }
    }
    return this->Seek(address);
}

void PpuRegisters::Write(dword address, byte value)
{
    switch(address)
    {
        case PpuRegisters::PPUSCROLL_ADDR:
        {
            if(this->bgr.ppuScrollLatch == false)
            {
                this->bgr.scrollX.val = value;
            }
            else
            {
                this->bgr.scrollY.val = value;
            }
            //flip the vram address latch
            this->bgr.ppuScrollLatch = !this->bgr.ppuAddressLatch;
            break;
        }
        case PpuRegisters::PPUADDR_ADDR:
        {
            if(this->bgr.ppuAddressLatch == false)
            {
                this->bgr.vramPpuAddress.upper = value;
            }
            else
            {
                this->bgr.vramPpuAddress.lower = value;
            }
            //flip the vram address latch
            this->bgr.ppuAddressLatch = !this->bgr.ppuAddressLatch;
            break;
        }
        case PpuRegisters::OAMDMA_ADDR:
        {
            //TODO
        }
        case PpuRegisters::OAMDATA_ADDR:
        {
            //TODO
            break;
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