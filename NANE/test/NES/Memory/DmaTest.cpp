#include <catch2/catch.hpp>
#include <sstream>
#include "NES/Memory/Dma.h"
#include "NES/Cartridge/CartridgeLoader.h"

template <typename Out>
void split(const std::string &s, char delim, Out result) 
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) 
{
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}


/**
 * Can read and write with loaded a cartridge
 */

TEST_CASE("DMA: Can read and write with loaded cartridge - PPU")
{
    Dma dma;
    const std::string nestestPath = "NANE/test/resources/nestest.nes";
    CartridgeLoader cartLoader;
    std::unique_ptr<ICartridge> ines = cartLoader.LoadCartridge(nestestPath);
    dma.SetCartridge(std::move(ines));
    
    //first values in cartridge
    REQUIRE(dma.PpuRead(0) == 0);
    REQUIRE(dma.PpuRead(32) == 0x80);
}

/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("DMA: test reading chr rom data") {
    Dma dma;
    const std::string nestestPath = "NANE/test/resources/nestest.nes";
    CartridgeLoader cartLoader;
    std::unique_ptr<ICartridge> ines = cartLoader.LoadCartridge(nestestPath);
    dma.SetCartridge(std::move(ines));

    std::unique_ptr<PatternTables> patternTables = dma.GeneratePatternTablesFromRom();
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
    }
}

/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("DMA: Can read and write without loading a cartridge") {
    Dma dma;

    //RAM
    dma.Write(0x0000, 24);
    REQUIRE(dma.Read(0x1800) == 24);

    dma.Write(0x07FF, 43);
    REQUIRE(dma.Read(0x1FFF) == 43);

    //PPU Registers
    dma.Write(0x2000, 122);
    REQUIRE(dma.Read(0x2008) == 122);
    REQUIRE(dma.GetPpuMemory().GetRegisters().name.PPUCTRL == 122);

    //APU Registers
    dma.Write(0x4000, 39);
    REQUIRE(dma.GetApuRegisters().name.SQ1.VOL == 39);

    dma.Write(0x4017, 61);
    REQUIRE(dma.GetApuRegisters().name.JOY2 == 61);

    //value just before cartridge
    REQUIRE(dma.Read(0x401F) == 0);
}


/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("DMA: Can read and write with loaded cartridge - CPU") {
    Dma dma;
    const std::string nestestPath = "NANE/test/resources/nestest.nes";
    CartridgeLoader cartLoader;
    std::unique_ptr<ICartridge> ines = cartLoader.LoadCartridge(nestestPath);
    dma.SetCartridge(std::move(ines));

    //value just before cartridge
    REQUIRE(dma.Read(0x401F) == 0);

    //start of PRG ROM
    REQUIRE(dma.Read(0x8000) == 0x4c);
    REQUIRE(dma.Read(0xC000) == 0x4c);

    //end of PRG ROM
    REQUIRE(dma.Read(0xBFFF) == 0xc5);
    REQUIRE(dma.Read(0xFFFF) == 0xc5);
}