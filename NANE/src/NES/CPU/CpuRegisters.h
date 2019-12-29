#ifndef CPU_REGISTERS
#define CPU_REGISTERS

#include "NES/Util/BitUtil.h"

/**
 * CPU registers
 */
struct CpuRegisters
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

#endif