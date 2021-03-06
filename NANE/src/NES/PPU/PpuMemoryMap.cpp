#include "PpuMemoryMap.h"

#include <iostream> //std::cerr

PpuMemoryMap::PpuMemoryMap()
    : IMemoryRW(0x2000, 0x3FFF)
{
}

byte PpuMemoryMap::Read(dword address)
{
    //handle special cases
    switch(address)
    {
        case PpuRegisters::OAMDATA_ADDR:
        {
            byte oamAddress = this->ppuRegMem.name.OAMADDR;
            //expose the internal secondary OAM (32 byte in length)
            if(oamAddress < 32)
            {
                return this->secondaryOamMem.Read(oamAddress);
            }
            else
            {
                return this->primaryOamMem.Read(oamAddress);
            }
            break;
        }
    }

    if(this->nametableMem.Contains(address))
    {
        return this->nametableMem.Read(address);
    }
    else if(this->palettesMem.Contains(address))
    {
        return this->palettesMem.Read(address);
    }
    else
    {
        std::cerr << "PPU Memory Map: failed to read from address: " << address << std::endl;
    }
    return 0;
}

void PpuMemoryMap::Write(dword address, byte value)
{
    if(this->nametableMem.Contains(address))
    {
        this->nametableMem.Write(address, value);
    }
    else if(this->palettesMem.Contains(address))
    {
        this->palettesMem.Write(address, value);
    }
    else
    {
        std::cerr << "PPU Memory Map: failed to write to address: " << address << std::endl;
    }
}

byte PpuMemoryMap::Seek(dword address) const
{
    if(this->nametableMem.Contains(address))
    {
        return this->nametableMem.Seek(address);
    }
    else if(this->palettesMem.Contains(address))
    {
        return this->palettesMem.Seek(address);
    }
    else
    {
        std::cerr << "PPU Memory Map: failed to seek to address: " << address << std::endl;
    }
    return 0;
}

ColourPalettes& PpuMemoryMap::GetPalettes()
{
    return this->palettesMem;
}

PpuRegisters& PpuMemoryMap::GetRegisters()
{
    return this->ppuRegMem;
}

const PpuRegisters& PpuMemoryMap::GetRegisters() const
{
    return this->ppuRegMem;
}

NameTables& PpuMemoryMap::GetNameTables()
{
    return this->nametableMem;
}

void PpuMemoryMap::SetScanLineNum(int scanLineNum)
{
    this->scanlineNum = scanLineNum;
}

int PpuMemoryMap::GetScanLineNum() const
{
    return this->scanlineNum;
}

void PpuMemoryMap::SetScanCycleNum(int scanCycleNum)
{
    this->scanCycleNum = scanCycleNum;
}

int PpuMemoryMap::GetScanCycleNum() const
{
    return this->scanCycleNum;
}

OamPrimary& PpuMemoryMap::GetPrimaryOam()
{
    return this->primaryOamMem;
}

OamSecondary& PpuMemoryMap::GetSecondaryOam()
{
    return this->secondaryOamMem;
}

const long long& PpuMemoryMap::GetTotalPpuCycles() const
{
    return this->totalPpuCycles;
}

dword PpuMemoryMap::GetVRamAddress(bool forWriting) const
{
    // check if reading from vram address during rendering
    if(forWriting == false)
    {
        if(this->scanlineNum >= -1 && this->scanlineNum <= 239)
        {
            if(this->scanCycleNum >= 1 && this->scanCycleNum <= 256)
            {
                std::cerr << "haven't implement games that use vram address during rendering" << std::endl;
            }
        }
    }
    return this->GetRegisters().GetVRamAddress();
}

void PpuMemoryMap::SetTotalPpuCycles(const long long& cycles)
{
    this->totalPpuCycles = cycles;
}