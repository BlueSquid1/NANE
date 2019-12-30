#ifndef PPU
#define PPU

#include <memory> //std::shared_ptr

#include "PpuRegisters.h"
#include "PpuMemoryMap.h"
#include "NES/Util/BitUtil.h"

class Ppu
{
    private:
    std::shared_ptr<PpuRegisters> registers;
    std::unique_ptr<PpuMemoryMap> memory;

    public:
    //constructor
    Ppu(std::shared_ptr<PpuRegisters> ppuRegisters);
};

#endif