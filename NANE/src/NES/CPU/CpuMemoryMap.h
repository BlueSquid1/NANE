#ifndef CPU_MEMORY_MAP
#define CPU_MEMORY_MAP

#include <memory> //std::shared_ptr

#include "CpuRegisters.h"
#include "NES/Util/BitUtil.h"
#include "NES/PPU/PpuRegisters.h"

class CpuMemoryMap
{
    private:
    byte cpuRam[2048];
    std::shared_ptr<PpuRegisters> ppuRegisters;

    public:
    CpuMemoryMap(std::unique_ptr<CpuRegisters> cpuRegisters, std::shared_ptr<PpuRegisters> ppuRegisters);
    byte Read(dword address);
    void Write(dword address, byte value);
};
#endif