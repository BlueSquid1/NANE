#include <catch2/catch.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "NES/PPU/PpuMemoryMap.h"
#include "NES/Cartridge/CartridgeLoader.h"

/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("PPU: Can read and write without loading a cartridge") 
{
    PpuMemoryMap memoryMap;
    REQUIRE(memoryMap.Contains(0x1FFF) == false);
    REQUIRE(memoryMap.Contains(0x2000) == true);
    REQUIRE(memoryMap.Contains(0x3FFF) == true);
    REQUIRE(memoryMap.Contains(0x4000) == false);

    //palette memory
    memoryMap.Write(0x3F00, 153);
    REQUIRE(memoryMap.Read(0x3F00) == 153);
    //check if memory mirroring is correct
    REQUIRE(memoryMap.Read(0x3F20) == 153);

    memoryMap.Write(0x3F1F, 249);
    REQUIRE(memoryMap.Read(0x3F1F) == 249);
    //check if memory mirroring is correct
    REQUIRE((int)memoryMap.Read(0x3FFF) == 249);
}