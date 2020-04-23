#ifndef APU_REGISTERS
#define APU_REGISTERS

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
    struct RegStruct
    {
        struct Pulse
        {
            byte VOL;
            byte SWEEP;
            byte LO;
            byte HI;
        };
        Pulse SQ1;
        Pulse SQ2;
        struct
        {
            byte LINEAR;
            byte _;
            byte LO;
            byte HI;
        }TRI;
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
        byte OAMDMA;
        byte SND_CHN;
    };

    //anonymous union
    union
    {
        RegStruct name;
        byte raw[22];
    };
    static const int rawLen = 22;

    //constructor
    ApuRegisters();
 };

#endif