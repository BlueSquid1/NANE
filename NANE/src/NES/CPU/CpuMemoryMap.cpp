#include "CpuMemoryMap.h"

CpuMemoryMap::CpuMemoryMap(std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters)
    : IMemoryRW(0x0000, 0xFFFF)
{
    std::unique_ptr<std::vector<byte>> ramVec = std::unique_ptr<std::vector<byte>>( new std::vector<byte>(2048) );
    this->cpuRam = std::unique_ptr<MemoryRepeater>( new MemoryRepeater(0x0000, 0x1FFF, std::move(ramVec)) );
    this->ppuRegisters = ppuRegisters;
    this->ppuRegMem = std::unique_ptr<MemoryRepeater>( new MemoryRepeater(0x2000, 0x3FFF) );

    this->apuRegisters = apuRegisters;
    this->apuRegMem = std::unique_ptr<MemoryRepeater>( new MemoryRepeater(0x4000, 0x4017) );
}


void CpuMemoryMap::SetCartridge(std::shared_ptr<ICartridge> cartridge)
{
    this->cartridge = cartridge;
}

byte CpuMemoryMap::Read(dword address)
{
    if(this->cpuRam->Contains(address))
    {
        return this->cpuRam->Read(address);
    }
    else if(this->ppuRegMem->Contains(address))
    {
        dword lowerOffset = this->ppuRegMem->LowerOffset(address);
        return this->ppuRegisters->raw[lowerOffset];
    }
    else if(this->apuRegMem->Contains(address))
    {
        dword lowerOffset = this->apuRegMem->LowerOffset(address);
        return this->apuRegisters->raw[lowerOffset];
    }
    else if(this->cartridge->Contains(address))
    {
        return this->cartridge->Read(address);
    }
    else
    {
        std::cerr << "CPU Memory Map: failed to read from address: " << address << std::endl;
    }
    return 0;
}
void CpuMemoryMap::Write(dword address, byte value)
{
    if(this->cpuRam->Contains(address))
    {
        this->cpuRam->Write(address, value);
    }
    else if(this->ppuRegMem->Contains(address))
    {
        dword lowerOffset = this->ppuRegMem->LowerOffset(address);
        this->ppuRegisters->raw[lowerOffset] = value;
    }
    else if(this->apuRegMem->Contains(address))
    {
        dword lowerOffset = this->apuRegMem->LowerOffset(address);
        this->apuRegisters->raw[lowerOffset] = value;
    }
    else if(this->cartridge->Contains(address))
    {
        this->cartridge->Write(address, value);
    }
    else
    {
        std::cerr << "CPU Memory Map: failed to write to address: " << address << std::endl;
    }
}