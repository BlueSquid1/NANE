#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/Memory/BitUtil.h"
#include "NES/CPU/CpuRegisters.h"

/**
 * test if ApuRegisters is a Plain-Old Data otherwise compiler might not place struct in the same order defined in the struct
 */
TEST_CASE("make sure CpuRegisters is Plain-Old Data otherwise") {
    REQUIRE(std::is_pod<CpuRegisters::RegStruct>::value == true);
}

/**
 * test to make sure registers are cleared after constructor
 */
TEST_CASE("CPU registers cleared by default") {
    CpuRegisters registers;

    REQUIRE(registers.rawLen > 0);
    for(int i = 0; i < registers.rawLen; ++i)
    {
        REQUIRE(registers.raw[i] == 0);
    }
}

/**
 * test to make sure registers are cleared after constructor
 */
TEST_CASE("CPU registers write to and read from raw") {
    CpuRegisters registers;
    
    //write - start
    registers.name.PC = 15183;
    REQUIRE( registers.raw[0] == 79 ); //01001111
    REQUIRE( registers.name.PCL == 79 );
    REQUIRE( registers.raw[1] == 59 ); //00111011
    REQUIRE( registers.name.PCH == 59 );
    //write - middle
    registers.name.P = 203;
    REQUIRE( registers.raw[2] == 0 );
    REQUIRE( registers.raw[3] == 203 );
    REQUIRE( registers.raw[4] == 0 );
    //write - end
    registers.name.Y = 138;
    REQUIRE( registers.raw[5] == 0 );
    REQUIRE( registers.raw[6] == 138 );
}


/**
 * test to make sure proccessor status booleans work as expected
 */
TEST_CASE("CPU process state flags") {
    CpuRegisters registers;
    
    //write - start
    registers.name.P = 217; //1101 1001
    REQUIRE(registers.name.C == true);
    REQUIRE(registers.name.Z == false);
    REQUIRE(registers.name.I == false);
    REQUIRE(registers.name.D == true);
    REQUIRE(registers.name.B == true);
    REQUIRE(registers.name._ == false);
    REQUIRE(registers.name.V == true);
    REQUIRE(registers.name.N == true);

    registers.name.B = false;
    REQUIRE(registers.name.P == 201); //1100 1001
}