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
            dword PC; /**< Program Counter (to next instruction) */
            struct
            {
                byte PCL;
                byte PCH;
            };
        };
        byte S; /**< Stack Pointer (to last inserted value) */
        union
        {
            byte P; /**< Processor status */
            struct
            {
                bool C : 1; /**< Carry Flag */
                bool Z : 1; /**< Zero Flag */
                bool I : 1; /**< Interrupt Disable */
                bool D : 1; /**< Decimal Mode Flag */
                bool B : 1; /**< Break Command */
                bool _ : 1; /**< EXPANSION */
                bool V : 1; /**< Overflow Flag */
                bool N : 1; /**< Negative Result */
            };
        };
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