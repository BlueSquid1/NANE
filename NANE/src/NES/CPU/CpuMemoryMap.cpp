#include "CpuMemoryMap.h"
#include <exception>

CpuMemoryMap::CpuMemoryMap()
    : IMemoryRW(0x0000, 0x1FFF)
{
    std::unique_ptr<std::vector<byte>> ramVec = std::unique_ptr<std::vector<byte>>( new std::vector<byte>(2048) );
    this->cpuRam = std::unique_ptr<MemoryRepeaterVec>( new MemoryRepeaterVec(0x0000, 0x1FFF, std::move(ramVec)) );
}

byte CpuMemoryMap::Read(dword address)
{
    if(this->cpuRam->Contains(address))
    {
        return this->cpuRam->Read(address);
    }
    else
    {
        throw std::out_of_range("CPU Memory Map: tried to read from an invalid memory address.");
    }
    return 0;
}
void CpuMemoryMap::Write(dword address, byte value)
{
    if(this->cpuRam->Contains(address))
    {
        this->cpuRam->Write(address, value);
    }
    else
    {
        throw std::out_of_range("CPU Memory Map: tried to write to an invalid memory address.");
    }
}

byte CpuMemoryMap::Seek(dword address) const
{
    if(this->cpuRam->Contains(address))
    {
        return this->cpuRam->Seek(address);
    }
    else
    {
        throw std::out_of_range("CPU Memory Map: tried to seek to an invalid memory address.");
    }
    return 0;
}


CpuRegisters& CpuMemoryMap::GetRegisters()
{
    return this->cpuRegMem;
}