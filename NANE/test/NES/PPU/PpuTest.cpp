#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/PPU/PpuRegisters.h"

/**
 * test if Ppu structs are Plain-Old Data otherwise compiler might not place struct in the same order defined in the struct
 */
TEST_CASE("make sure Ppu structs are Plain-Old Data otherwise") 
{
    REQUIRE(std::is_pod<PpuRegisters::BackgroundRegisters>::value == true);
}