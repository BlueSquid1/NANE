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
    std::shared_ptr<ICartridge> ines = cartLoader.LoadCartridge(nestestPath);
    dma.SetCartridge(ines);
    
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
    std::shared_ptr<ICartridge> ines = cartLoader.LoadCartridge(nestestPath);
    dma.SetCartridge(ines);

    std::unique_ptr<PatternTables> patternTables = dma.GeneratePatternTablesFromRom();
    //test that first tile in test rom is blank
    Matrix<byte> firstTile = patternTables->GetTile(0,0,0);
    std::vector<byte> firstData = firstTile.dump();
    for(unsigned int i = 0; i < firstData.size(); ++i)
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

    dma.Write(0x2001, 46);
    REQUIRE(dma.Read(0x2009) == 46);
    REQUIRE(dma.GetPpuMemory().GetRegisters().name.PPUMASK == 46);

    //APU Registers
    dma.Write(0x4000, 39);
    REQUIRE(dma.GetApuRegisters().name.SQ1.VOL == 39);
    dma.Write(0x4015, 61);
    REQUIRE(dma.GetApuRegisters().name.SND_CHN == 61);

    //Controller inputs
    REQUIRE(dma.Read(0x4016) == 65);
    REQUIRE(dma.Read(0x4017) == 65);
    dma.Write(0x4016, 0);
    REQUIRE(dma.Read(0x4016) == 64);
    REQUIRE(dma.Read(0x4017) == 64);
}


/**
 * Can read and write without loading a cartridge
 */

TEST_CASE("DMA: Can read and write with loaded cartridge - CPU") {
    Dma dma;
    const std::string nestestPath = "NANE/test/resources/nestest.nes";
    CartridgeLoader cartLoader;
    std::shared_ptr<ICartridge> ines = cartLoader.LoadCartridge(nestestPath);
    dma.SetCartridge(ines);

    //start of PRG ROM
    REQUIRE(dma.Read(0x8000) == 0x4c);
    REQUIRE(dma.Read(0xC000) == 0x4c);

    //end of PRG ROM
    REQUIRE(dma.Read(0xBFFF) == 0xc5);
    REQUIRE(dma.Read(0xFFFF) == 0xc5);
}

TEST_CASE("DMA: test PPUDATA_ADDR(0x2007) internal buffering") {
    Dma dma;
    //set vram dirrection to 0 (horrizontal)
    dma.GetPpuMemory().GetRegisters().name.vramDirrection = 0;
    //also set mirroring mode to supress warning messages
    dma.GetPpuMemory().GetNameTables().SetMirrorType(INes::MirrorType::horizontal);

    //start writing to vram address 0x3EFE
    dma.Write(0x2006, 0x3E);
    dma.Write(0x2006, 0xFE);

    dma.Write(0x2007, 43);
    dma.Write(0x2007, 254);

    //now read what was written
    dma.Write(0x2006, 0x3E);
    dma.Write(0x2006, 0xFE);

    //first value is an internal buffer value and can be discarded
    dma.Read(0x2007);
    REQUIRE(dma.Read(0x2007) == 43);

    //need to lower the vram address as addresses above 0x3EFF will overwrite the internal buffer
    dma.Write(0x2006, 0x3E);
    dma.Write(0x2006, 0x00);
    REQUIRE(dma.Read(0x2007) == 254);
}

TEST_CASE("DMA: test PPUDATA_ADDR(0x2007) overwrite buffering") {
    Dma dma;
    //set vram dirrection to 0 (horrizontal)
    dma.GetPpuMemory().GetRegisters().name.vramDirrection = 0;
    //also set mirroring mode to supress warning messages
    dma.GetPpuMemory().GetNameTables().SetMirrorType(INes::MirrorType::horizontal);

    //start writing to vram address 0x3F00
    dma.Write(0x2006, 0x3F);
    dma.Write(0x2006, 0x00);

    dma.Write(0x2007, 89);
    dma.Write(0x2007, 239);

    //now read what was written
    dma.Write(0x2006, 0x3F);
    dma.Write(0x2006, 0x00);

    REQUIRE(dma.Read(0x2007) == 89);
    REQUIRE(dma.Read(0x2007) == 239);

    //if reading from a internal buffer area it should return the last read value again
    dma.Write(0x2006, 0x3E);
    dma.Write(0x2006, 0x00);
    REQUIRE(dma.Read(0x2007) == 239);
}

TEST_CASE("DMA: test starting DMA transfer") {
    Dma dma;

    dma.Write(0x0, 23);
    dma.Write(0x1, 158);

    dma.Write(0xFE, 217);
    dma.Write(0xFF, 94);

    REQUIRE(dma.GetDmaActive() == false);
    dma.Write(0x4014, 0);

    REQUIRE(dma.GetDmaActive() == true);
    while(dma.GetDmaActive() == true)
    {
        dma.ProcessDma();

        long long nextPpuCycle = dma.GetPpuMemory().GetTotalPpuCycles() + 1;
        dma.GetPpuMemory().SetTotalPpuCycles(nextPpuCycle);
    }

    REQUIRE(dma.GetPpuMemory().GetPrimaryOam().Read(0x0) == 23 );
    REQUIRE(dma.GetPpuMemory().GetPrimaryOam().Read(0x1) == 158 );
    REQUIRE(dma.GetPpuMemory().GetPrimaryOam().Read(0xFE) == 217 );
    REQUIRE(dma.GetPpuMemory().GetPrimaryOam().Read(0xFF) == 94 );
}