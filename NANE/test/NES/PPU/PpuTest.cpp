#include <catch2/catch.hpp>

#include "NES/PPU/Ppu.h"
#include "NES/Memory/Dma.h"

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

TEST_CASE("PPU populate the frame buffer correctly")
{
    Dma dma;
    Ppu ppu(dma);
    REQUIRE(ppu.PowerCycle() == true);

    // create patterntable
    
    // configure ppu settings
    PpuRegisters ppuRegs = dma.GetPpuMemory().GetRegisters();
    ppuRegs.name.showBackgroundLeftmost = true;
    ppuRegs.name.showSpritesLeftmost = true;
    ppuRegs.name.showBackground = true;
    ppuRegs.name.showSprites = true;

    // generate frame
    while(ppu.GetTotalFrameCount() == 0)
    {
        ppu.Step();
    }
    Matrix<rawColour> display = ppu.GetFrameDisplay();

    // compare output
}