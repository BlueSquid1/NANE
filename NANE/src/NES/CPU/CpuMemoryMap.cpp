#include "CpuMemoryMap.h"

CpuMemoryMap::CpuMemoryMap(std::shared_ptr<PpuRegisters> ppuRegisters, 
    std::shared_ptr<ApuRegisters> apuRegisters, 
    std::shared_ptr<ICartridge> cartridgeMapping)
: cpuRam(2048)
{
    
}