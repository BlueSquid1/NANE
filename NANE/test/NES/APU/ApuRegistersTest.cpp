#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/Memory/BitUtil.h"
#include "NES/APU/ApuRegisters.h"

/**
 * test if ApuRegisters is a Plain-Old Data otherwise compiler might not place struct in the same order defined in the struct
 */
TEST_CASE("make sure ApuRegisters is Plain-Old Data otherwise") {
    REQUIRE(std::is_pod<ApuRegisters::RegStruct>::value == true);
}

/**
 * test to make sure registers are cleared after constructor
 */
TEST_CASE("APU registers cleared by default") {
    ApuRegisters registers;

    REQUIRE(registers.rawLen > 0);
    for(int i = 0; i < registers.rawLen; ++i)
    {
        REQUIRE(registers.raw[i] == 0);
    }
}

/**
 * test to make sure registers are cleared after constructor
 */
TEST_CASE("registers write to and read from raw") {
    ApuRegisters registers;
    
    //write - start
    registers.name.SQ1.VOL = 24;
    REQUIRE( registers.raw[0] == 24 );
    REQUIRE( registers.raw[1] == 0 );
    //write - middle
    registers.name.NOISE.LO = 203;
    REQUIRE( registers.raw[13] == 0 );
    REQUIRE( registers.raw[14] == 203 );
    REQUIRE( registers.raw[15] == 0 );
    //write - end
    registers.name.SND_CHN = 138;
    REQUIRE( registers.raw[20] == 0 );
    REQUIRE( registers.raw[21] == 138 );
}
