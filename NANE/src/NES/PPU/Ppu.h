#ifndef PPU
#define PPU

#include <memory> //std::shared_ptr

#include "PpuRegisters.h"
#include "NES/Util/BitUtil.h"

/**
 * PPU registers. 
 * Struct is used so can guarantee that fields of the struct will be laid out in memory in the order they are declared.
 */
class Ppu
{
    private:
    std::shared_ptr<PpuRegisters> r;
    public:
};

#endif