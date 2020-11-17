#include <catch2/catch.hpp>
#include <vector>

#include "NES/Memory/BitUtil.h"

#include "NES/PPU/OamSecondary.h"

OamPrimary::Sprite GenerateSprite(int startPosX, int startPosY)
{
    OamPrimary::Sprite sprite;
    sprite.posY = startPosY;
    sprite.posX = startPosX;
    sprite.index = 0;
    sprite.palette = 0;
    sprite.backgroundPriority = 0;
    sprite.flipHorizontally = 0;
    sprite.flipVertically = 0;
    return sprite;
}

OamSecondary::SpritePatternTiles GenerateTestTiles(bool isLargeTiles)
{
    OamSecondary::SpritePatternTiles spriteTiles;

    spriteTiles.numOfTiles = 1;
    for(int i = 0; i < 8; ++i)
    {
        spriteTiles.firstTile.lsbPlane[i] = i;
        spriteTiles.firstTile.msbPlane[i] = i;
    }

    if(isLargeTiles)
    {
        spriteTiles.numOfTiles = 2;
        for(int i = 0; i < 8; ++i)
        {
            spriteTiles.secondTile.lsbPlane[i] = 8 + i;
            spriteTiles.secondTile.msbPlane[i] = 8 + i;
        }
    }
    return spriteTiles;
}

/**
 * Test filling fetched sprite buffer
 */
TEST_CASE("Appending fetched sprites") 
{
    OamSecondary oam;

    // Create some sprites
    std::vector<OamSecondary::IndexedSprite> inputSprites;
    for(int i = 0; i < oam.TotalNumOfSprites; ++i)
    {
        OamSecondary::IndexedSprite inputSprite;
        inputSprite.sprite.posY = 101 + i;
        inputSprite.sprite.index = 121 + i;
        inputSprite.sprite.attributes = 54 + i;
        inputSprite.sprite.posX = 59 + i;
        inputSprite.primaryOamIndex = i;
        inputSprites.push_back(inputSprite);
    }

    for(unsigned int i = 0; i < inputSprites.size(); ++i)
    {
        oam.AppendFetchedSprite(inputSprites.at(i).sprite, inputSprites.at(i).primaryOamIndex);
    }

    REQUIRE( oam.GetSpriteFetchNum() == oam.TotalNumOfSprites );

    for(int i = 0; i < oam.TotalNumOfSprites; ++i)
    {
        OamPrimary::Sprite sprite = inputSprites.at(i).sprite;
        int primaryIndex = inputSprites.at(i).primaryOamIndex;
        OamSecondary::IndexedSprite fetchedSprite = oam.GetFetchedSprite(i);
        REQUIRE( fetchedSprite.sprite.posY == sprite.posY );
        REQUIRE( fetchedSprite.sprite.index == sprite.index );
        REQUIRE( fetchedSprite.sprite.attributes == sprite.attributes );
        REQUIRE( fetchedSprite.sprite.posX == sprite.posX );
        REQUIRE( fetchedSprite.primaryOamIndex == primaryIndex );
    }

    oam.ClearFetchData();
    REQUIRE( oam.GetSpriteFetchNum() == 0);
}

TEST_CASE("calculate scanline for straight sprite")
{
    int spriteYPos = 10;
    OamSecondary oam;
    OamPrimary::Sprite straightSprite = GenerateSprite(0, spriteYPos);
    OamSecondary::SpritePatternTiles spriteTiles = GenerateTestTiles(false);

    for(int i = 0; i < 8; ++i)
    {
        OamSecondary::ScanlineTile scanline = oam.CalcSpriteBuffer(spriteYPos + i, straightSprite, spriteTiles);
        REQUIRE( BitUtil::FlipByte(scanline.lsbSpriteTile) == spriteTiles.firstTile.lsbPlane[i] );
        REQUIRE( BitUtil::FlipByte(scanline.msbSpriteTile) == spriteTiles.firstTile.msbPlane[i] );
    }
}

TEST_CASE("calculate scanline for vertical flipped sprite")
{
    int spriteYPos = 10;
    OamSecondary oam;
    OamPrimary::Sprite flippedSprite = GenerateSprite(0, spriteYPos);
    flippedSprite.flipVertically = 1;
    OamSecondary::SpritePatternTiles spriteTiles = GenerateTestTiles(false);

    for(int i = 0; i < 8; ++i)
    {
        OamSecondary::ScanlineTile scanline = oam.CalcSpriteBuffer(spriteYPos + i, flippedSprite, spriteTiles);
        REQUIRE( BitUtil::FlipByte(scanline.lsbSpriteTile) == spriteTiles.firstTile.lsbPlane[7 - i] );
        REQUIRE( BitUtil::FlipByte(scanline.msbSpriteTile) == spriteTiles.firstTile.msbPlane[7 - i] );
    }
}

TEST_CASE("calculate scanline for horizontal flipped sprite")
{
    int spriteYPos = 10;
    OamSecondary oam;
    OamPrimary::Sprite flippedSprite = GenerateSprite(0, spriteYPos);
    flippedSprite.flipHorizontally = 1;
    OamSecondary::SpritePatternTiles spriteTiles = GenerateTestTiles(false);

    for(int i = 0; i < 8; ++i)
    {
        OamSecondary::ScanlineTile scanline = oam.CalcSpriteBuffer(spriteYPos + i, flippedSprite, spriteTiles);
        REQUIRE( scanline.lsbSpriteTile == spriteTiles.firstTile.lsbPlane[i] );
        REQUIRE( scanline.msbSpriteTile == spriteTiles.firstTile.msbPlane[i] );
    }
}

TEST_CASE("calculate scanline for vertical flipped large sprite")
{
    int spriteYPos = 10;
    OamSecondary oam;
    OamPrimary::Sprite flippedSprite = GenerateSprite(0, spriteYPos);
    flippedSprite.flipVertically = 1;
    OamSecondary::SpritePatternTiles spriteTiles = GenerateTestTiles(true);

    for(int i = 0; i < 7; ++i)
    {
        OamSecondary::ScanlineTile scanline = oam.CalcSpriteBuffer(spriteYPos + i, flippedSprite, spriteTiles);
        REQUIRE( BitUtil::FlipByte(scanline.lsbSpriteTile) == spriteTiles.secondTile.lsbPlane[7 - i] );
        REQUIRE( BitUtil::FlipByte(scanline.msbSpriteTile) == spriteTiles.secondTile.msbPlane[7 - i] );
    }

    for(int i = 0; i < 7; ++i)
    {
        OamSecondary::ScanlineTile scanline = oam.CalcSpriteBuffer(spriteYPos + 8 + i, flippedSprite, spriteTiles);
        REQUIRE( BitUtil::FlipByte(scanline.lsbSpriteTile) == spriteTiles.firstTile.lsbPlane[7 - i] );
        REQUIRE( BitUtil::FlipByte(scanline.msbSpriteTile) == spriteTiles.firstTile.msbPlane[7 - i] );
    }
}

TEST_CASE("Calculate pixels before and after a sprite")
{
    int spritePosX = 10;
    OamSecondary oam;
    OamSecondary::IndexSpriteBuffer spriteBuffer;
    spriteBuffer.sprite = GenerateSprite(spritePosX, 0);
    spriteBuffer.primaryOamIndex = 0;
    spriteBuffer.scanlineTile.lsbSpriteTile = 0x7A;
    spriteBuffer.scanlineTile.msbSpriteTile = 0x28;

    oam.AppendToActiveBuffer(spriteBuffer);

    OamSecondary::IndexPattern preIndex = oam.CalcForgroundPixel(spritePosX - 1);
    REQUIRE(preIndex.primaryOamIndex == -1);

    for(int i = 0; i < 8; ++i)
    {
        OamSecondary::IndexPattern indexedPattern = oam.CalcForgroundPixel(spritePosX + i);
        byte lsb = BitUtil::GetBits(spriteBuffer.scanlineTile.lsbSpriteTile, i);
        byte msb = BitUtil::GetBits(spriteBuffer.scanlineTile.msbSpriteTile, i);
        byte expectedPattern = lsb | (msb << 1);
        REQUIRE(indexedPattern.pattern == expectedPattern );
    }

    OamSecondary::IndexPattern postIndex = oam.CalcForgroundPixel(spritePosX + 8);
    REQUIRE(postIndex.primaryOamIndex == -1);

    oam.ClearActiveBuffer();
    REQUIRE( oam.CalcForgroundPixel(0).primaryOamIndex == -1 );
}

TEST_CASE("Calculate pixels of overlapping sprites")
{
    int sprite1PosX = 16;
    int sprite2PosX = 10;
    OamSecondary oam;
    OamSecondary::IndexSpriteBuffer sprite1Buffer;
    sprite1Buffer.sprite = GenerateSprite(sprite1PosX, 0);
    sprite1Buffer.primaryOamIndex = 0;
    sprite1Buffer.scanlineTile.lsbSpriteTile = 0xFF;
    sprite1Buffer.scanlineTile.msbSpriteTile = 0x28;

    OamSecondary::IndexSpriteBuffer sprite2Buffer;
    sprite2Buffer.sprite = GenerateSprite(sprite2PosX, 0);
    sprite2Buffer.primaryOamIndex = 1;
    sprite2Buffer.scanlineTile.lsbSpriteTile = 0xFF;
    sprite2Buffer.scanlineTile.msbSpriteTile = 0xAA;

    oam.AppendToActiveBuffer(sprite1Buffer);
    oam.AppendToActiveBuffer(sprite2Buffer);

    for(int i = 0; i < (sprite1PosX - sprite2PosX); ++i)
    {
        OamSecondary::IndexPattern indexedPattern = oam.CalcForgroundPixel(sprite2PosX + i);
        byte lsb = BitUtil::GetBits(sprite2Buffer.scanlineTile.lsbSpriteTile, i);
        byte msb = BitUtil::GetBits(sprite2Buffer.scanlineTile.msbSpriteTile, i);
        byte expectedPattern = lsb | (msb << 1);
        REQUIRE(indexedPattern.pattern == expectedPattern );
    }

    for(int i = 0; i < 8; ++i)
    {
        OamSecondary::IndexPattern indexedPattern = oam.CalcForgroundPixel(sprite1PosX + i);
        byte lsb = BitUtil::GetBits(sprite1Buffer.scanlineTile.lsbSpriteTile, i);
        byte msb = BitUtil::GetBits(sprite1Buffer.scanlineTile.msbSpriteTile, i);
        byte expectedPattern = lsb | (msb << 1);
        REQUIRE(indexedPattern.pattern == expectedPattern );
    }
}