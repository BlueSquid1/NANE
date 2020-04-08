#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/Memory/BitUtil.h"
#include "NES/PPU/PpuRegisters.h"

/**
 * test if PpuRegisters is a Plain-Old Data otherwise compiler might not place struct in the same order defined in the struct
 */
TEST_CASE("make sure PpuRegisters is Plain-Old Data otherwise") {
    REQUIRE(std::is_pod<PpuRegisters::RegStruct>::value == true);
}

/**
 * test to make sure raw length is right
 */
TEST_CASE("make sure PpuRegisters has correct raw length") {
    PpuRegisters registers;
    REQUIRE(registers.rawLen == 8);
}

/**
 * test to make sure registers are cleared after constructor
 */
TEST_CASE("registers cleared by default") {
    PpuRegisters registers;

    //test the internal registers
    REQUIRE(registers.bgr.vramPpuAddress.val == 0);
    REQUIRE(registers.bgr.ppuAddressLatch == 0);
    REQUIRE(registers.bgr.ppuDataReadBuffer == 0);
    

    REQUIRE(registers.rawLen > 0);
    for(int i = 0; i < registers.rawLen; ++i)
    {
        REQUIRE(registers.Seek(0x2000 + i) == 0);
    }
}

/**
 * test to make sure registers are cleared after constructor
 */
TEST_CASE("PPU registers write to and read from raw") {
    PpuRegisters registers;
    
    //write - start
    registers.name.PPUCTRL = 24;
    REQUIRE( registers.Read(0x2000) == 24 );
    REQUIRE( registers.Read(0x2001) == 0 );
    //write - middle
    registers.name.OAMADDR = 203;
    REQUIRE( registers.Read(0x2003) == 203 );
    REQUIRE( registers.Read(0x2004) == 0 );
    //write - end
    registers.name.PPUDATA = 138;
    REQUIRE( registers.Read(0x2006) == 0 );
    REQUIRE( registers.Read(0x2007) == 138 );
}

/**
 * test latching registers
 */
TEST_CASE("PPU latching registers") {
    PpuRegisters registers;
    dword ppuAddr = 0x2006;
    registers.Write(ppuAddr, 0x21);

    REQUIRE(registers.bgr.ppuAddressLatch == true);
    REQUIRE(registers.bgr.vramPpuAddress.upper == 0x21);

    registers.Write(ppuAddr, 0x08);

    REQUIRE(registers.bgr.ppuAddressLatch == false);
    REQUIRE(registers.bgr.vramPpuAddress.val == 0x2108);
}

/**
 * test PPUCTRL, PPUMASK and PPUSTATUS which all use unions with there bits
 */
TEST_CASE("PPUCTRL, PPUMASK and PPUSTATUS union tests") {
    PpuRegisters registers;
    
    //PPUCTRL
    registers.name.PPUCTRL = 167; //1010 0111
    REQUIRE(registers.name.baseNameTable == 3);
    REQUIRE(registers.name.vramDirrection == 1);
    REQUIRE(registers.name.sprite8x8PatternTable == 0);
    REQUIRE(registers.name.backgroundPatternTable == 0);
    REQUIRE(registers.name.spriteSize == 1);
    REQUIRE(registers.name.ppuMaster == 0);
    REQUIRE(registers.name.generateNmi == 1);

    registers.name.baseNameTable = 1;
    registers.name.sprite8x8PatternTable = 1;
    REQUIRE(registers.name.PPUCTRL == 173);

    //PPUMASK
    registers.name.PPUMASK = 89; //0101 1001
    REQUIRE(registers.name.greyscale == 1);
    REQUIRE(registers.name.showBackgroundLeftmost == 0);
    REQUIRE(registers.name.showSpritesLeftmost == 0);
    REQUIRE(registers.name.showBackground == 1);
    REQUIRE(registers.name.showSprites == 1);
    REQUIRE(registers.name.emphasizeRed == 0);
    REQUIRE(registers.name.emphasizeGreen == 1);
    REQUIRE(registers.name.emphasizeBlue == 0);

    registers.name.showSprites = 0;
    registers.name.emphasizeRed = 1;
    REQUIRE(registers.name.PPUMASK == 105);
    
    //PPUSTATUS
    registers.name.PPUSTATUS = 178; //1011 0010
    REQUIRE(registers.name.lsbRegWritten == 18);
    REQUIRE(registers.name.spriteOverflow == 1);
    REQUIRE(registers.name.sprite0Hit == 0);
    REQUIRE(registers.name.verticalBlank == 1);

    registers.name.lsbRegWritten = 7;
    registers.name.verticalBlank = 0;
    REQUIRE(registers.name.PPUSTATUS == 39); //0010 0111
}