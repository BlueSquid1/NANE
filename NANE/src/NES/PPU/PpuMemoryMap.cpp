#include "PpuMemoryMap.h"

PpuMemoryMap::PpuMemoryMap(std::unique_ptr<ColourPalettes>& ppuPalettes)
    : IMemoryRW(0x0000, 0x4017)
{
    std::unique_ptr<std::vector<byte>> ramVec = std::unique_ptr<std::vector<byte>>( new std::vector<byte>(4096) );
    this->vram = std::unique_ptr<MemoryRepeaterVec>( new MemoryRepeaterVec(0x2000, 0x27FF, std::move(ramVec)) );
    this->ppuPalettes = std::unique_ptr<IMemoryRepeater>( new MemoryRepeaterArray(0x3F00, 0x3F1F, ppuPalettes->raw, ppuPalettes->rawLen) );
}

byte PpuMemoryMap::Read(dword address) const
{
    if(this->vram->Contains(address))
    {
        return this->vram->Read(address);
    }
    else if(this->cartridge != NULL &&this->cartridge->Contains(address))
    {
        return this->cartridge->Read(address);
    }
    else if(this->ppuPalettes->Contains(address))
    {
        return this->ppuPalettes->Read(address);
    }
    else
    {
        std::cerr << "PPU Memory Map: failed to read from address: " << address << std::endl;
    }
    return 0;
}

void PpuMemoryMap::Write(dword address, byte value)
{
    if(this->vram->Contains(address))
    {
        this->vram->Write(address, value);
    }
    else if(this->cartridge != NULL && this->cartridge->Contains(address))
    {
        this->cartridge->Write(address, value);
    }
    else if(this->ppuPalettes->Contains(address))
    {
        this->ppuPalettes->Write(address, value);
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