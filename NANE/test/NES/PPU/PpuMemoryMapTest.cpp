#include <catch2/catch.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "NES/PPU/PpuMemoryMap.h"
#include "NES/Cartridge/CartridgeLoader.h"

template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}



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

    std::unique_ptr<PatternTables> patternTables = memoryMap.GeneratePatternTablesFromRom();
    //test that first tile in test rom is blank
    Matrix<byte> firstTile = patternTables->GetTile(0,0,0);
    std::vector<byte> firstData = firstTile.dump();
    for(int i = 0; i < firstData.size(); ++i)
    {
        REQUIRE(firstData.at(i) == 0);
    }

    Matrix<byte> questionTile = patternTables->GetTile(0,3,15);

    std::string expectedValues[] = {
        "0 3 3 3 3 3 3 0",
        "0 3 3 0 0 0 3 3",
        "0 0 0 0 0 0 3 3",
        "0 0 0 0 0 3 3 0",
        "0 0 0 3 3 3 0 0",
        "0 0 0 0 0 0 0 0",
        "0 0 0 3 3 0 0 0",
        "0 0 0 3 3 0 0 0"
    };

    for(int y = 0; y < patternTables->TILE_HEIGHT; ++y)
    {
        std::vector<std::string> expectedRowVec = split(expectedValues[y], ' ');
        for(int x = 0; x < patternTables->TILE_WIDTH; ++x)
        {
            REQUIRE(questionTile.Get(y, x) == std::stoi(expectedRowVec.at(x)));
        }
        std::cout << std::endl;
    }
}