#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/Util/BitUtil.h"
#include "NES/PPU/PpuRegisters.h"

/**
 * test if PpuRegisters is a Plain-Old Data otherwise compiler might not place struct in the same order defined in the struct
 */
TEST_CASE("make sure PpuRegisters is Plain-Old Data otherwise") {
    REQUIRE(std::is_pod<PpuRegisters::RegStruct>::value == true);
}

/**
 * test to make sure registers are cleared after constructor
 */
TEST_CASE("registers cleared by default") {
    PpuRegisters registers;

    REQUIRE(registers.rawLen > 0);
    for(int i = 0; i < registers.rawLen; ++i)
    {
        REQUIRE(registers.raw[i] == 0);
    }
}

/**
 * test to make sure registers are cleared after constructor
 */
TEST_CASE("PPU registers write to and read from raw") {
    PpuRegisters registers;
    
    //write - start
    registers.name.PPUCTRL = 24;
    REQUIRE( registers.raw[0] == 24 );
    REQUIRE( registers.raw[1] == 0 );
    //write - middle
    registers.name.OAMADDR = 203;
    REQUIRE( registers.raw[2] == 0 );
    REQUIRE( registers.raw[3] == 203 );
    REQUIRE( registers.raw[4] == 0 );
    //write - end
    registers.name.PPUDATA = 138;
    REQUIRE( registers.raw[6] == 0 );
    REQUIRE( registers.raw[7] == 138 );
}
