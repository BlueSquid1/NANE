#ifndef PPU_REGISTERS
#define PPU_REGISTERS

#include "NES/Util/BitUtil.h"

/**
 * PPU registers. 
 * Plain-Old Data struct is used so can guarantee that fields of the struct will be laid out in memory in the order they are declared in union.
 */
class PpuRegisters
{
    public:
    struct RegStruct
    {
        byte PPUCTRL;
        byte PPUMASK;
        byte PPUSTATUS;
        byte OAMADDR;
        byte OAMDATA;
        byte PPUSCROLL;
        byte PPUADDR;
        byte PPUDATA;
    };

    //anonymous union
    union
    {
        RegStruct name;
        byte raw[sizeof(RegStruct)];
    };
    const int rawLen = sizeof(RegStruct);

    //constructor
    PpuRegisters();
 };

#endif