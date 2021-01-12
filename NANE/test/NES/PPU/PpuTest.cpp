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

Ppu SetupPpu(bool fillBackground)
{
    std::shared_ptr<Dma> dma = std::make_shared<Dma>();
    Ppu ppu(dma);
    
    // use the testrom pattern table
    const std::string nestestPath = "NANE/test/resources/nestest.nes";
    CartridgeLoader cartridgeLoader;
    std::shared_ptr<ICartridge> cartridge = cartridgeLoader.LoadCartridge(nestestPath);
    REQUIRE(cartridge);
    dma->SetCartridge(cartridge);

    //initalize PPU
    REQUIRE(ppu.PowerCycle() == true);

    //set nametable
    if(fillBackground)
    {
        for(unsigned int i = 0; i < referenceNameTable.size(); ++i)
        {
            dma->GetPpuMemory().Write(i + 0x2000, referenceNameTable[i]);
        }
    }
    else
    {
        for(unsigned int i = 0; i < referenceNameTable.size(); ++i)
        {
            dma->GetPpuMemory().Write(i + 0x2000, 0);
        }
    }

    //set colour palettes
    for(int palleteNum = 0; palleteNum < 8; ++palleteNum)
    {
        for(unsigned int colourNum = 0; colourNum < refColourPallete.size(); ++colourNum)
        {
            dma->GetPpuMemory().Write(0x3F00 + (palleteNum * 4) + colourNum, refColourPallete[colourNum]);
        }
    }

    //default to sprites being off the screen
    for(int i = 0; i < OamPrimary::TotalNumOfSprites; ++i)
    {
        dma->GetPpuMemory().GetPrimaryOam().name.sprites[i].posY = 255;
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

std::unique_ptr<OamPrimary::Sprite> GetSpriteAtPos(std::shared_ptr<Dma> dma, int pixelY, int pixelX)
{
    for(const OamPrimary::Sprite& sprite : dma->GetPpuMemory().GetPrimaryOam().name.sprites)
    {
        int deltaX = pixelX - sprite.posX;
        int deltaY = pixelY - sprite.posY;
        if((deltaX >= 0 && deltaX < 8) && (deltaY >= 0 && deltaY < 8))
        {
            return std::make_unique<OamPrimary::Sprite>(sprite);
        }
    }
    return nullptr;
}

void RenderDisplay(Matrix<rawColour> display)
{
    for(int row = 0; row < display.GetHeight(); ++row)
    {
        for(int col = 0; col < display.GetWidth(); ++col)
        {
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
    Ppu ppu = SetupPpu(true);
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

TEST_CASE("PPU render scrolling background test")
{
    int courseScrollXOffset = 2;
    int fineScrollXOffset = 3;
    Ppu ppu = SetupPpu(true);
    std::shared_ptr<Dma> dma = ppu.GetDma();
    dma->GetPpuMemory().GetRegisters().vRegs.bckgndDrawing.nextLoopyReg.scrollXMsb = 0;
    dma->GetPpuMemory().GetRegisters().vRegs.bckgndDrawing.nextLoopyReg.scrollXCourse = courseScrollXOffset;
    dma->GetPpuMemory().GetRegisters().vRegs.bckgndDrawing.scrollXFine = fineScrollXOffset;

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

    for(int row = 0; row < 30; ++row)
    {
        for(int col = 0; col < 32; ++col)
        {
            int expectedValue = row * 32 + col + courseScrollXOffset;
            if(expectedValue >= 32)
            {
                continue;
            }

            int baseX = col * 8;
            int baseY = row * 8;

            int encodingValue = referenceNameTable.at(expectedValue);
            int encodingX = encodingValue % 16;
            int encodingY = floor(encodingValue / 16.0);
            Matrix<patternIndex> refTile = patternTables->GetTile(0, encodingY, encodingX);

            for(int row = 0; row < refTile.GetHeight(); ++row)
            {
                for(int col = 0; col < refTile.GetWidth(); ++col)
                {
                    patternIndex refPattern = refTile.Get(row, col);
                    rawColour refColour = NesColour::GetRawColour(refColourPallete.at(refPattern));
                    
                    int displayPixelX = baseX + col - fineScrollXOffset;
                    if(displayPixelX < 0)
                    {
                        continue;
                    }
                    int displayPixelY = baseY + row;

                    rawColour actualColour = display.Get(displayPixelY, displayPixelX);
                    INFO("Pixel (x: " << displayPixelX << ", y: " << displayPixelY << ") is wrong");
                    REQUIRE(refColour.raw == actualColour.raw);
                }
            }
        }
    }
}

TEST_CASE("test rendering sprites") 
{
    Ppu ppu = SetupPpu(false);
    std::shared_ptr<Dma> dma = ppu.GetDma();
    OamPrimary::Sprite& firstSprite = dma->GetPpuMemory().GetPrimaryOam().name.sprites[0];
    OamPrimary::Sprite& secondSprite = dma->GetPpuMemory().GetPrimaryOam().name.sprites[1];
    firstSprite.posX = 100;
    firstSprite.posY = 100;
    firstSprite.index = 48;
    firstSprite.palette = 0;
    firstSprite.backgroundPriority = false;
    firstSprite.flipHorizontally = false;
    firstSprite.flipVertically = false;

    secondSprite.posX = 90;
    secondSprite.posY = 100;
    secondSprite.index = 48;
    secondSprite.palette = 0;
    secondSprite.backgroundPriority = false;
    secondSprite.flipHorizontally = false;
    secondSprite.flipVertically = false;

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

    for(int pixelY = 0; pixelY < 240; ++pixelY)
    {
        for(int pixelX = 0; pixelX < 256; ++pixelX)
        {
            rawColour displayColour = display.Get(pixelY, pixelX);
            std::unique_ptr<OamPrimary::Sprite> activeSprite = GetSpriteAtPos(dma, pixelY, pixelX);
            INFO("Pixel (x: " << pixelX << ", y: " << pixelY << ") is wrong");
            if(activeSprite)
            {
                int encodingValue = activeSprite->index;
                int encodingX = encodingValue % 16;
                int encodingY = floor(encodingValue / 16.0);
                Matrix<patternIndex> refTile = patternTables->GetTile(0, encodingY, encodingX);

                int deltaX = pixelX - activeSprite->posX;
                int deltaY = pixelY - activeSprite->posY;
                REQUIRE(deltaX < refTile.GetWidth());
                REQUIRE(deltaY < refTile.GetHeight());
                patternIndex refPattern = refTile.Get(deltaY, deltaX);
                rawColour refColour = NesColour::GetRawColour(refColourPallete.at(refPattern));
                REQUIRE(displayColour.raw == refColour.raw);
            }
            else
            {
                REQUIRE(displayColour.channels.blue != 0);
                REQUIRE(displayColour.channels.red != 0);
                REQUIRE(displayColour.channels.green != 0);
            }
        }
    }
}