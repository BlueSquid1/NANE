#pragma once

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/MemoryRepeaterArray.h"

#include "SquareWave.h"

/**
 * APU registers. 
 * Plain-Old Data struct is used so can guarantee that fields of the struct will be laid out in memory in the order they are declared in union.
 * https://wiki.nesdev.com/w/index.php/APU#Registers
 * https://wiki.nesdev.com/w/index.php/2A03
 */
class ApuRegisters: public MemoryRepeaterArray
{
    public:
    #pragma pack(push, 1)
    struct RegStruct
    {
        struct SquareWaveStruct
        {
            byte VOL;
            byte SWEEP;
            byte LO;
            byte HI;
        } SQ1, SQ2;
        struct
        {
            byte LINEAR;
            byte _;
            byte LO;
            byte HI;
        } TRI;
        struct
        {
            byte VOL;
            byte _;
            byte LO;
            byte HI;
        }NOISE;
        struct
        {
            byte FREQ;
            byte RAW;
            byte START;
            byte LEN;
        }DMC;
        byte _0; //0x4014
        byte SND_CHN; //4015
        byte _1; //0x4016
        byte FRAME_COUNTER; //0x4017
    };
    #pragma pack(pop)

    struct VirtualRegisters
    {
        SquareWave sq1;
        SquareWave sq2;
    };

    static const int rawLen = 24; //24 bytes worth of registers

    //anonymous union
    union
    {
        RegStruct name;
        byte raw[rawLen];
    };

    VirtualRegisters vRegs;

    //constructor
    ApuRegisters();
 };