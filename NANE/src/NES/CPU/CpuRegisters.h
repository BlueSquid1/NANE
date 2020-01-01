#ifndef CPU_REGISTERS
#define CPU_REGISTERS

#include "NES/Memory/BitUtil.h"

/**
 * PPU registers. 
 * Plain-Old Data struct is used so can guarantee that fields of the struct will be laid out in memory in the order they are declared in union.
 */
class CpuRegisters
{
    public:
    struct RegStruct
    {
        union
        {
            unsigned short int PC; /**< Program Counter (to next instruction) */
            struct
            {
                byte PCL;
                byte PCH;
            };
        };
        byte S; /**< Stack Pointer (to last inserted value) */
        byte P; /**< Processor status */
        byte A; /**< Accumulator */
        byte X; /**< Index Register X */
        byte Y; /**< Index Register Y */
    };

    //anonymous union
    union
    {
        RegStruct name;
        byte raw[sizeof(RegStruct)];
    };
    int rawLen = sizeof(RegStruct);

    //constructor
    CpuRegisters();
 };

#endif