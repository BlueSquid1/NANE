#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/PPU/Ppu.h"
#include "NES/Memory/Dma.h"

/**
 * 
 */
TEST_CASE("test powerup state") 
{
    Dma dma;
    Ppu ppu(dma);
    Matrix<rawColour> display = ppu.GetFrameDisplay();
    REQUIRE(display.GetHeight() == 240);
    REQUIRE(display.GetWidth() == 256);
    rawColour black;
    black.channels.alpha = 0xFF;
    black.channels.red = 0x00;
    black.channels.green = 0x00;
    black.channels.blue = 0x00;

    for(int y = 0; y < display.GetHeight(); ++y)
    {
        for(int x = 0; x < display.GetWidth(); ++x)
        {
            REQUIRE(display.Get(y,x).raw == black.raw);
        }
    }

    REQUIRE(ppu.PowerCycle() == true);
    REQUIRE(dma.GetPpuMemory().GetScanCycleNum() == 0);
    REQUIRE(dma.GetPpuMemory().GetScanLineNum() == -1);
}

// /**
//  * see next tile function
//  */
// TEST_CASE("test ppu next tile function")
// {
//     Dma dma;
//     Ppu ppu(dma);
//     ppu.PowerCycle();
//     dma.GetPpuMemory().SetScanLineNum(0);
//     dma.GetPpuMemory().SetScanCycleNum(0);
//     ppu.CalcNextFetchTile();
// }