#pragma once

#include "NES/Memory/BitUtil.h"

/**
 * PPU registers. 
 * Plain-Old Data struct is used so can guarantee that fields of the struct will be laid out in memory in the order they are declared in union.
 */
class CpuRegisters
{
    public:
    #pragma pack(push, 1)
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
        byte S; /**< Stack Pointer (to next free value) */
        union
        {
            byte P; /**< Processor status */
            struct
            {
                bit C : 1; /**< Carry Flag */
                bit Z : 1; /**< Zero Flag */
                bit I : 1; /**< Interrupt Disable */
                bit D : 1; /**< Decimal Mode Flag */
                bit B : 1; /**< Break Command */
                bit _ : 1; /**< EXPANSION */
                bit V : 1; /**< Overflow Flag */
                bit N : 1; /**< Negative Result */
            };
        };
        byte A; /**< Accumulator */
        byte X; /**< Index Register X */
        byte Y; /**< Index Register Y */
    };
    #pragma pack(pop)

    static const int rawLen = 7; //7 bytes worth of registers
    //anonymous union
    union
    {
        RegStruct name;
        byte raw[rawLen];
    };

    //constructor
    CpuRegisters();
 };