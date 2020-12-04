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
    REQUIRE(ppu.PowerCycle() == true);
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
    REQUIRE(dma.GetPpuMemory().GetScanCycleNum() == 0);
    REQUIRE(dma.GetPpuMemory().GetScanLineNum() == -1);
    REQUIRE(ppu.GetTotalFrameCount() == 0);
    REQUIRE(dma.GetPpuMemory().GetTotalPpuCycles() == 0);
    REQUIRE(dma.GetDmaBuffer() == 0);
}