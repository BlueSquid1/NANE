#include <catch2/catch.hpp>

#include "NES/CPU/CpuMemoryMap.h"
#include "NES/Cartridge/CartridgeLoader.h"

/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("CPU: Can read and write without loading a cartridge") {
    CpuMemoryMap memoryMap;

    REQUIRE(memoryMap.Contains(0x0) == true);
    REQUIRE(memoryMap.Contains(0x1FFF) == true);

    //RAM
    memoryMap.Write(0x0000, 24);
    REQUIRE(memoryMap.Read(0x1800) == 24);

    memoryMap.Write(0x07FF, 43);
    REQUIRE(memoryMap.Read(0x1FFF) == 43);
}