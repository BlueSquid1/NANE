#include <exception>

#include "CpuMemoryMap.h"

CpuMemoryMap::CpuMemoryMap(std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters)
    : cpuRam(2048)
{
    this->ppuRegisters = ppuRegisters;
    this->apuRegisters = apuRegisters;
}


void CpuMemoryMap::SetCartridge(std::shared_ptr<ICartridge> cartridge)
{
    this->cartridge = cartridge;
}

byte CpuMemoryMap::Read(dword address)
{
    
}
void CpuMemoryMap::Write(dword address, byte value)
{

}