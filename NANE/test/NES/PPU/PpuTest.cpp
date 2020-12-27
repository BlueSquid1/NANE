#include <catch2/catch.hpp>
#include <math.h>
#include <string>
#include <memory>

#include "NES/PPU/Ppu.h"
#include "NES/Memory/Dma.h"
#include "NES/Cartridge/CartridgeLoader.h"
#include "NES/Memory/Matrix.h"
#include "NES/PPU/NesColour.h"

std::vector<colourIndex> refColourPallete = 
{
    32, //background red=0xFF green=0xFE blue=0xFF alpha=0xFF
    0, 
    0, 
    13 //forground red=0x00 green=0x00 blue=0x00 alpha=0xFF
};

Dma dma;

std::string referenceNameTable =
    "01234567890123456789012345678912"
    "abcdefghijklmnopqrstuvwxyzabcdef"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF"
    "01234567890123456789012345678912"
    "abcdefghijklmnopqrstuvwxyzabcdef"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF"
    "01234567890123456789012345678912"
    "abcdefghijklmnopqrstuvwxyzabcdef"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF"
    "01234567890123456789012345678912"
    "abcdefghijklmnopqrstuvwxyzabcdef"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF"
    "01234567890123456789012345678912"
    "abcdefghijklmnopqrstuvwxyzabcdef"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF"
    "01234567890123456789012345678912"
    "abcdefghijklmnopqrstuvwxyzabcdef"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF"
    "01234567890123456789012345678912"
    "abcdefghijklmnopqrstuvwxyzabcdef"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF"
    "01234567890123456789012345678912"
    "abcdefghijklmnopqrstuvwxyzabcdef"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF"
    "01234567890123456789012345678912"
    "abcdefghijklmnopqrstuvwxyzabcdef"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF"
    "01234567890123456789012345678912"
    "abcdefghijklmnopqrstuvwxyzabcdef"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF";

Ppu SetupPpu()
{
    std::shared_ptr<Dma> dma = std::make_shared<Dma>();
    Ppu ppu(dma);
    
    // use the testrom pattern table
    const std::string nestestPath = "NANE/test/resources/nestest.nes";
    CartridgeLoader cartridgeLoader;
    std::shared_ptr<ICartridge> cartridge = cartridgeLoader.LoadCartridge(nestestPath);
    REQUIRE(cartridge);
    dma->SetCartridge(std::move(cartridge));

    //initalize PPU
    REQUIRE(ppu.PowerCycle() == true);

    //set nametable
    for(unsigned int i = 0; i < referenceNameTable.size(); ++i)
    {
        dma->GetPpuMemory().Write(i + 0x2000, referenceNameTable[i]);
    }

    //set colour palette
    for(unsigned int i = 0; i < refColourPallete.size(); ++i)
    {
        dma->GetPpuMemory().Write(0x3F00 + i, refColourPallete[i]);
    }

    // configure ppu settings
    ppu.SetDisassemblePalette(0);
    PpuRegisters& ppuRegs = dma->GetPpuMemory().GetRegisters();
    ppuRegs.name.showBackgroundLeftmost = true;
    ppuRegs.name.showSpritesLeftmost = true;
    ppuRegs.name.showBackground = true;
    ppuRegs.name.showSprites = true;
    return ppu;
}

void RenderDisplay(Matrix<rawColour> display)
{
    for(int row = 0; row < display.GetHeight(); ++row)
    {
        for(int col = 0; col < display.GetWidth(); ++col)
        {
            //TODO
            if(display.Get(row, col).channels.blue != 0)
            {
                std::cout << ".";
            }
            else
            {
                std::cout << "*";
            }
        }
        std::cout << "\n";
    }
}


TEST_CASE("test powerup state") 
{
    std::shared_ptr<Dma> dma = std::make_shared<Dma>();
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
    REQUIRE(dma->GetPpuMemory().GetScanCycleNum() == 0);
    REQUIRE(dma->GetPpuMemory().GetScanLineNum() == -1);
    REQUIRE(ppu.GetTotalFrameCount() == 0);
    REQUIRE(dma->GetPpuMemory().GetTotalPpuCycles() == 0);
    REQUIRE(dma->GetDmaBuffer() == 0);
}

TEST_CASE("PPU render background test")
{
    Ppu ppu = SetupPpu();
    std::shared_ptr<Dma> dma = ppu.GetDma();

    // generate frame
    while(ppu.GetTotalFrameCount() <= 1 )
    {
        ppu.Step();
    }
    Matrix<rawColour> display = ppu.GetFrameDisplay();

    // compare output
    std::unique_ptr<PatternTables> patternTables = dma->GeneratePatternTablesFromRom();
    REQUIRE(display.GetWidth() == 256);
    REQUIRE(display.GetHeight() == 240);

    for(unsigned int i = 0; i < referenceNameTable.size(); ++i)
    {
        int baseX = (i % 32) * 8;
        int baseY = (floor(i / 32.0)) * 8;
        
        int encodingValue = referenceNameTable.at(i);
        int encodingX = encodingValue % 16;
        int encodingY = floor(encodingValue / 16.0);
        Matrix<patternIndex> refTile = patternTables->GetTile(0, encodingY, encodingX);

        for(int row = 0; row < refTile.GetHeight(); ++row)
        {
            for(int col = 0; col < refTile.GetWidth(); ++col)
            {
                patternIndex refPattern = refTile.Get(row, col);
                rawColour refColour = NesColour::GetRawColour(refColourPallete.at(refPattern));
                
                int displayPixelX = baseX + col;
                int displayPixelY = baseY + row;

                rawColour actualColour = display.Get(displayPixelY, displayPixelX);
                INFO("Pixel (x: " << displayPixelX << ", y: " << displayPixelY << ") is wrong");
                REQUIRE(refColour.raw == actualColour.raw);
            }
        }
    }
}