#include "PpuMemoryMap.h"

PpuMemoryMap::PpuMemoryMap(std::shared_ptr<PpuRegisters> ppuRegisters)
    : IMemoryRW(0x0000, 0x4017)
{
    this->ppuRegMem = ppuRegisters;

    std::unique_ptr<std::vector<byte>> nametableVec = std::unique_ptr<std::vector<byte>>( new std::vector<byte>(4096) );
    this->nametableMem = std::unique_ptr<MemoryRepeaterVec>( new MemoryRepeaterVec(0x2000, 0x3EFF, std::move(nametableVec)) );
    this->palettesMem = std::unique_ptr<ColourPalettes>( new ColourPalettes() );

    this->primOam = std::unique_ptr<std::vector<byte>>( new std::vector<byte>(256) );
    this->secOam = std::unique_ptr<std::vector<byte>>( new std::vector<byte>(32) );
}

byte PpuMemoryMap::Read(dword address) const
{
    if(this->nametableMem->Contains(address))
    {
        return this->nametableMem->Read(address);
    }
    else if(this->cartridge != NULL &&this->cartridge->Contains(address))
    {
        return this->cartridge->Read(address);
    }
    else if(this->palettesMem->Contains(address))
    {
        return this->palettesMem->Read(address);
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
    else if(this->cartridge != NULL && this->cartridge->Contains(address))
    {
        this->cartridge->Write(address, value);
    }
    else if(this->palettesMem->Contains(address))
    {
        this->palettesMem->Write(address, value);
    }
    else
    {
        std::cerr << "PPU Memory Map: failed to write to address: " << address << std::endl;
    }
}

void PpuMemoryMap::SetCartridge(std::shared_ptr<ICartridge> cartridge)
{
    this->cartridge = cartridge;
}

std::unique_ptr<ColourPalettes>& PpuMemoryMap::GetPalettes()
{
    return this->palettesMem;
}

std::shared_ptr<PpuRegisters>& PpuMemoryMap::GetPpuRegisters()
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