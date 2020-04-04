#include "PpuMemoryMap.h"

#include <iostream> //std::cerr

PpuMemoryMap::PpuMemoryMap()
    : IMemoryRW(0x2000, 0x3FFF),
    primOam( 256 ),
    secOam( 32 )
{
    std::unique_ptr<std::vector<byte>> nametableVec = std::unique_ptr<std::vector<byte>>( new std::vector<byte>(4096) );
    this->nametableMem = std::unique_ptr<MemoryRepeaterVec>( new MemoryRepeaterVec(0x2000, 0x3EFF, std::move(nametableVec)) );
}

byte PpuMemoryMap::Read(dword address)
{
    if(this->nametableMem->Contains(address))
    {
        return this->nametableMem->Read(address);
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
    if(this->nametableMem->Contains(address))
    {
        this->nametableMem->Write(address, value);
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
    if(this->nametableMem->Contains(address))
    {
        return this->nametableMem->Seek(address);
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