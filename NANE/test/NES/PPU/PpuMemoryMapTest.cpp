#include <catch2/catch.hpp>

#include "NES/PPU/PpuMemoryMap.h"
#include "NES/Cartridge/CartridgeLoader.h"

/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("PPU: Can read and write without loading a cartridge") {
    
    std::shared_ptr<PpuRegisters> ppuRegisters( new PpuRegisters() );
    PpuMemoryMap memoryMap(ppuRegisters);

    REQUIRE(memoryMap.Contains(0x0) == true);
    REQUIRE(memoryMap.Contains(0x3FFF) == true);
    REQUIRE(memoryMap.Contains(0x4000) == false);

    //Pattern table 0
    memoryMap.Write(0x0000, 24);
    REQUIRE(memoryMap.Read(0x0000) == 0);

    memoryMap.Write(0x0FFF, 43);
    REQUIRE(memoryMap.Read(0x0FFF) == 0);

    //Pattern table 1
    memoryMap.Write(0x1000, 12);
    REQUIRE(memoryMap.Read(0x1000) == 0);

    memoryMap.Write(0x1FFF, 201);
    REQUIRE(memoryMap.Read(0x1FFF) == 0);

    //Name tables
    memoryMap.Write(0x2000, 56);
    REQUIRE(memoryMap.Read(0x2000) == 56);
    //TODO check with GetNameTable()

    memoryMap.Write(0x2FFF, 47);
    REQUIRE(memoryMap.Read(0x2FFF) == 47);
    //TODO check with GetNameTable()
    
    //check if memory mirroring for nametables are correct
    REQUIRE(memoryMap.Read(0x3000) == 56);
    memoryMap.Write(0x2EFF, 78);
    REQUIRE(memoryMap.Read(0x2EFF) == 78);

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

/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("PPU: Can read and write with loaded cartridge") {
    const std::string nestestPath = "NANE/test/resources/nestest.nes";

    std::shared_ptr<PpuRegisters> ppuRegisters( new PpuRegisters() );
    PpuMemoryMap memoryMap(ppuRegisters);

    CartridgeLoader cartLoader;
    std::shared_ptr<ICartridge> ines = cartLoader.LoadCartridge(nestestPath);
    memoryMap.SetCartridge(ines);
    
    //first values in cartridge
    REQUIRE(memoryMap.Read(0) == 0);
    REQUIRE(memoryMap.Read(32) == 0x80);
}

/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("PPU: test reading chr rom data") {
    const std::string nestestPath = "NANE/test/resources/nestest.nes";

    std::shared_ptr<PpuRegisters> ppuRegisters( new PpuRegisters() );
    PpuMemoryMap memoryMap(ppuRegisters);

    CartridgeLoader cartLoader;
    std::shared_ptr<ICartridge> ines = cartLoader.LoadCartridge(nestestPath);
    memoryMap.SetCartridge(ines);

    PatternTables& patternTables = memoryMap.GetChrDataFromRom();
}