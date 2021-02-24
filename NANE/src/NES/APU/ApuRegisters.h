#pragma once

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/MemoryRepeaterArray.h"

/**
 * APU registers. 
 * Plain-Old Data struct is used so can guarantee that fields of the struct will be laid out in memory in the order they are declared in union.
 * https://wiki.nesdev.com/w/index.php/APU#Registers
 * https://wiki.nesdev.com/w/index.php/2A03
 */
class ApuRegisters: public MemoryRepeaterArray
{
    public:
    enum ApuAddresses : unsigned
    {
        SQ1_VOL_ADDR = 0x4000,
        SQ1_LO_ADDR = 0x4002,
        SQ1_HI_ADDR = 0x4003,
        SQ2_VOL_ADDR = 0x4004,
        SQ2_LO_ADDR = 0x4006,
        SQ2_HI_ADDR = 0x4007,
        SND_CHN_ADDR = 0x4015
    };

    #pragma pack(push, 1)
    struct RegStruct
    {
        struct SquareWaveStruct
        {
            union
            {
                byte VOL;
                struct
                {
                    byte dutyNum : 2;
                    bit lengthCounterHault : 1;
                    bit constantVolume : 1;
                    byte volume : 4;
                };
            };
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
        union
        {
            byte SND_CHN; //0x4015
            struct
            {
                bit pulse1 : 1;
                bit pulse2 : 1;
                bit triangle : 1;
                bit noise : 1;
                bit dmc : 1;
                bit _ : 1;
                bit frame_irq : 1;
                bit dmc_irq : 1;
            }enableStatus; //0x4015
        };
        byte _1; //0x4016
        byte FRAME_COUNTER; //0x4017
    };
    #pragma pack(pop)

    static const int rawLen = 24; //24 bytes worth of registers

    //anonymous union
    union
    {
        RegStruct name;
        byte raw[rawLen];
    };

    //constructor
    ApuRegisters();
 };