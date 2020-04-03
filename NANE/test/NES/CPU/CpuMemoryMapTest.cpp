#include <catch2/catch.hpp>

#include "NES/CPU/CpuMemoryMap.h"
#include "NES/Cartridge/CartridgeLoader.h"

/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("CPU: Can read and write without loading a cartridge") {
    
    std::shared_ptr<PpuRegisters> ppuRegisters( new PpuRegisters() );
    std::shared_ptr<ApuRegisters> apuRegisters( new ApuRegisters() );
    CpuMemoryMap memoryMap(ppuRegisters, apuRegisters);

    REQUIRE(memoryMap.Contains(0x0) == true);
    REQUIRE(memoryMap.Contains(0xFFFF) == true);

    //RAM
    memoryMap.Write(0x0000, 24);
    REQUIRE(memoryMap.Read(0x1800) == 24);

    memoryMap.Write(0x07FF, 43);
    REQUIRE(memoryMap.Read(0x1FFF) == 43);

    //PPU Registers
    memoryMap.Write(0x2000, 122);
    REQUIRE(memoryMap.Read(0x2008) == 122);
    REQUIRE(ppuRegisters->name.PPUCTRL == 122);

    memoryMap.Write(0x2007, 73);
    REQUIRE(memoryMap.Read(0x3FFF) == 73);
    REQUIRE(ppuRegisters->name.PPUDATA == 73);

    //APU Registers
    memoryMap.Write(0x4000, 39);
    REQUIRE(apuRegisters->name.SQ1.VOL == 39);

    memoryMap.Write(0x4017, 61);
    REQUIRE(apuRegisters->name.JOY2 == 61);

    //value just before cartridge
    REQUIRE(memoryMap.Read(0x401F) == 0);
}

/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("CPU: Can read and write with loaded cartridge") {
    const std::string nestestPath = "NANE/test/resources/nestest.nes";

    std::shared_ptr<PpuRegisters> ppuRegisters( new PpuRegisters() );
    std::shared_ptr<ApuRegisters> apuRegisters( new ApuRegisters() );
    CpuMemoryMap memoryMap(ppuRegisters, apuRegisters);

    CartridgeLoader cartLoader;
    std::shared_ptr<ICartridge> ines = cartLoader.LoadCartridge(nestestPath);
    memoryMap.SetCartridge(ines);

    //value just before cartridge
    REQUIRE(memoryMap.Read(0x401F) == 0);

    //start of PRG ROM
    REQUIRE(memoryMap.Read(0x8000) == 0x4c);
    REQUIRE(memoryMap.Read(0xC000) == 0x4c);

    //end of PRG ROM
    REQUIRE(memoryMap.Read(0xBFFF) == 0xc5);
    REQUIRE(memoryMap.Read(0xFFFF) == 0xc5);
}