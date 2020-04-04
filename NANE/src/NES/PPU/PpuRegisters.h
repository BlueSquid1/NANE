#ifndef PPU_REGISTERS
#define PPU_REGISTERS

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/MemoryRepeaterArray.h"

/**
 * PPU registers. 
 * Plain-Old Data struct is used so can guarantee that fields of the struct will be laid out in memory in the order they are declared in union.
 */
class PpuRegisters : public MemoryRepeaterArray
{
    public:
    enum PpuAddresses : unsigned
    {
        PPUCTRL_ADDR   = 0x2000,
        PPUMASK_ADDR   = 0x2001,
        PPUSTATUS_ADDR = 0x2002,
        OAMADDR_ADDR   = 0x2003,
        OAMDATA_ADDR   = 0x2004,
        PPUSCROLL_ADDR = 0x2005,
        PPUADDR_ADDR   = 0x2006,
        PPUDATA_ADDR   = 0x2007,
        OAMDMA_ADDR    = 0x4014
    };

    struct RegStruct
    {
        union
        {
            byte PPUCTRL;
            struct
            {
                byte baseNameTable : 2; //Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
                bit vramDirrection : 1; //VRAM address increment per CPU read/write of PPUDATA (0: add 1, going across; 1: add 32, going down)
                bit sprite8x8PatternTable : 1; //Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
                bit backgroundPatternTable : 1; //Background pattern table address (0: $0000; 1: $1000)
                bit spriteSize : 1; //Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
                bit ppuMaster : 1; //PPU master/slave select (0: read backdrop from EXT pins; 1: output color on EXT pins)
                bit generateNmi : 1; //Generate an NMI at the start of the vertical blanking interval (0: off; 1: on)
            };
        };
        union
        {
            byte PPUMASK;
            struct
            {
                bit greyscale : 1; //Greyscale (0: normal color, 1: produce a greyscale display)
                bit showBackgroundLeftmost : 1; //Show background in leftmost 8 pixels of screen, 0: Hide
                bit showSpritesLeftmost : 1; //Show sprites in leftmost 8 pixels of screen, 0: Hide
                bit showBackground : 1; //Show background
                bit showSprites : 1; //Show sprites
                bit emphasizeRed : 1; //Emphasize red
                bit emphasizeGreen : 1; //Emphasize green
                bit emphasizeBlue : 1; //Emphasize blue
            };
        };
        union
        {
            byte PPUSTATUS;
            struct
            {
                byte lsbRegWritten : 5;
                bit spriteOverflow : 1; //more than eight sprites appear on a scanline
                bit sprite0Hit : 1; //Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps a nonzero background pixel; cleared at dot 1 of the pre-render line.  Used for raster timing.
                bit verticalBlank : 1; //Vertical blank has started (0: not in vblank; 1: in vblank). Set at dot 1 of line 241 (the line *after* the post-render line); cleared after reading $2002 and at dot 1 of the pre-render line.
            };
        };
        byte OAMADDR; //Write the address of OAM you want to access here
        byte OAMDATA;
        byte PPUSCROLL; //0x2005 fine scroll position (two writes: X scroll, Y scroll)
        byte PPUADDR; //0x2006 VRAM address (two writes: upper, lower)
        byte PPUDATA; //0x2007
    };

    // this registers don't exist on a real NES but are used to simplify different states of the PPU
    struct BackgroundRegisters
    {
        // internal registers
        // https://wiki.nesdev.com/w/index.php/PPU_scrolling#PPU_internal_registers
        dword_p curPpuAddress; //holds current VRAM address written into PPUADDR (0x2006)
        bool ppuAddressLatch; //false == write to lower curVramAddr, true == write to upper curVramAddr
        
        // buffer used for:
        // https://wiki.nesdev.com/w/index.php/PPU_registers#The_PPUDATA_read_buffer_.28post-fetch.29
        byte ppuDataReadBuffer;

        // Temporary Registers
        byte ntByte;
        byte atByte;
        byte tileLo;
        byte tileHi;
        
        // Shift Registers for background
        struct 
        {
            byte paletteAttribute1; //colour attribute 1 (controls bit 0)
            byte paletteAttribute2; //colour attribute 1 (controls bit 1)
            // bool atLatch1;
            // bool atLatch2;
            dword_p patternPlane1; //background pattern plane 1 (controls bit 0)
            dword_p patternPlane2; //background pattern plane 2 (controls bit 1)
        } shift;
    };

    static const int rawLen = 8;

    //anonymous union
    union
    {
        RegStruct name;
        byte raw[rawLen];
    };

    BackgroundRegisters bgr;

    public:
    //constructor
    PpuRegisters();

    byte Read(dword address) override;
    void Write(dword address, byte value) override;
    byte Seek(dword address) const override;
 };

#endif