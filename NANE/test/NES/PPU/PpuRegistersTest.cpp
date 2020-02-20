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

    REQUIRE(registers.rawLen > 0);
    for(int i = 0; i < registers.rawLen; ++i)
    {
        REQUIRE(registers.Read(0x2000 + i) == 0);
    }

    //test the internal registers
    REQUIRE(registers.name.V.value == 0);
    REQUIRE(registers.name.T.value == 0);
    REQUIRE(registers.name.vramAddrLatchLower == 0);
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
    REQUIRE( registers.Read(0x2002) == 0 );
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

    REQUIRE(registers.name.vramAddrLatchLower == true);
    REQUIRE(registers.name.V.value == 0x0);
    REQUIRE(registers.name.T.upper == 0x21);

    registers.Write(ppuAddr, 0x08);

    REQUIRE(registers.name.vramAddrLatchLower == false);
    REQUIRE(registers.name.V.value == 0x2108);
    REQUIRE(registers.name.T.value == 0x2108);
}