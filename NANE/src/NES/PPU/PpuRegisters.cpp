#include "PpuRegisters.h"

const int PpuRegisters::rawLen;

PpuRegisters::PpuRegisters()
: MemoryRepeaterArray(0x2000, 0x3FFF, this->raw, this->rawLen)
{
    this->PowerCycle();
}

bool PpuRegisters::PowerCycle()
{
    memset(this->raw, 0, this->rawLen);

    this->vRegs.bckgndDrawing.loopyLatch = false;
    this->vRegs.bckgndDrawing.activeLoopyReg.address.val = 0;
    this->vRegs.bckgndDrawing.nextLoopyReg.address.val = 0;
    this->vRegs.bckgndDrawing.scrollXFine = 0;
    this->vRegs.bckgndDrawing.lsbPatternPlane.val = 0;
    this->vRegs.bckgndDrawing.msbPatternPlane.val = 0;
    this->vRegs.bckgndDrawing.lsbPalletePlane.val = 0;
    this->vRegs.bckgndDrawing.msbPalletePlane.val = 0;
    this->vRegs.nextNametableIndex = 0;
    this->vRegs.nextAttributeIndex = 0;
    this->vRegs.backgroundFetchTileLsb = 0;
    this->vRegs.backgroundFetchTileMsb = 0;
    this->vRegs.ppuDataReadBuffer = 0;
    return true;
}

byte PpuRegisters::Read(dword address)
{
    switch(address)
    {
        case PpuRegisters::PPUSTATUS_ADDR:
        {
            //fill the bottom bits with ppu data read buffer
            byte returnValue = (this->name.PPUSTATUS & 0xE0) | (this->vRegs.ppuDataReadBuffer & 0x1F);
            
            // reading this register also clears a bunch of internal registers
            this->name.verticalBlank = false;
            this->vRegs.bckgndDrawing.loopyLatch = false;
            return returnValue;
        }
    }
    return this->Seek(address);
}

void PpuRegisters::Write(dword address, byte value)
{
    switch(address)
    {
        case PpuRegisters::PPUCTRL_ADDR:
        {
            this->vRegs.bckgndDrawing.nextLoopyReg.scrollXMsb = BitUtil::GetBits(value, 0);
            this->vRegs.bckgndDrawing.nextLoopyReg.scrollYMsb = BitUtil::GetBits(value, 1);
            break;
        }
        case PpuRegisters::PPUSCROLL_ADDR:
        {
            if(this->vRegs.bckgndDrawing.loopyLatch == false)
            {
                this->vRegs.bckgndDrawing.nextLoopyReg.scrollXCourse = BitUtil::GetBits(value, 3, 7);
                this->vRegs.bckgndDrawing.scrollXFine = BitUtil::GetBits(value, 0, 2);
                this->vRegs.bckgndDrawing.loopyLatch = true;
            }
            else
            {
                this->vRegs.bckgndDrawing.nextLoopyReg.scrollYCourse = BitUtil::GetBits(value, 3, 7);
                this->vRegs.bckgndDrawing.nextLoopyReg.scrollYFine = BitUtil::GetBits(value, 0, 2);
                this->vRegs.bckgndDrawing.loopyLatch = false;
            }
            break;
        }
        case PpuRegisters::PPUADDR_ADDR:
        {
            if(this->vRegs.bckgndDrawing.loopyLatch == false)
            {
                this->vRegs.bckgndDrawing.nextLoopyReg.address.upper = BitUtil::GetBits(value, 0, 5);
                this->vRegs.bckgndDrawing.loopyLatch = true;
            }
            else
            {
                this->vRegs.bckgndDrawing.nextLoopyReg.address.lower = value;
                this->vRegs.bckgndDrawing.activeLoopyReg = this->vRegs.bckgndDrawing.nextLoopyReg;
                this->vRegs.bckgndDrawing.loopyLatch = false;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    MemoryRepeaterArray::Write(address, value);
}

byte PpuRegisters::Seek(dword address) const
{
    return MemoryRepeaterArray::Seek(address);
}

void PpuRegisters::TransferScrollX()
{
    this->vRegs.bckgndDrawing.activeLoopyReg.scrollXMsb = this->vRegs.bckgndDrawing.nextLoopyReg.scrollXMsb;
    this->vRegs.bckgndDrawing.activeLoopyReg.scrollXCourse = this->vRegs.bckgndDrawing.nextLoopyReg.scrollXCourse;
}

void PpuRegisters::TransferScrollY()
{
    this->vRegs.bckgndDrawing.activeLoopyReg.scrollYMsb = this->vRegs.bckgndDrawing.nextLoopyReg.scrollYMsb;
    this->vRegs.bckgndDrawing.activeLoopyReg.scrollYCourse = this->vRegs.bckgndDrawing.nextLoopyReg.scrollYCourse;
    this->vRegs.bckgndDrawing.activeLoopyReg.scrollYFine = this->vRegs.bckgndDrawing.nextLoopyReg.scrollYFine;
}

dword PpuRegisters::GetActiveScrollX() const
{
    dword scrollXMsb = this->vRegs.bckgndDrawing.activeLoopyReg.scrollXMsb;
    dword scrollXCourse = this->vRegs.bckgndDrawing.activeLoopyReg.scrollXCourse;
    dword scrollXFine = this->vRegs.bckgndDrawing.scrollXFine;
    return (scrollXMsb << 8) + (scrollXCourse << 3) + scrollXFine;
}

dword PpuRegisters::GetActiveScrollY() const
{
    dword scrollYMsb = this->vRegs.bckgndDrawing.activeLoopyReg.scrollYMsb;
    dword scrollYCourse = this->vRegs.bckgndDrawing.activeLoopyReg.scrollYCourse;
    dword scrollYFine = this->vRegs.bckgndDrawing.activeLoopyReg.scrollYFine;
    return (scrollYMsb << 8) + (scrollYCourse << 3) + scrollYFine;
}

dword PpuRegisters::GetVRamAddress() const
{
    return this->vRegs.bckgndDrawing.activeLoopyReg.address.val;
}

void PpuRegisters::IncrementVRamAddress()
{
    //increment vram pointer
    dword incrementAmount = 1;
    if(this->name.vramDirrection == 1)
    {
        incrementAmount = 32;
    }
    this->vRegs.bckgndDrawing.activeLoopyReg.address.val += incrementAmount;
}