#include "Ppu.h"

Ppu::Ppu(Dma& dma)
: dma(dma), framebuffer(256, 240, {0x000000FF})
{
}

Ppu::Point Ppu::NextPixel()
{
    Ppu::Point nextPoint;
    nextPoint.y = this->dma.GetPpuMemory().GetScanLineNum();
    nextPoint.x = this->dma.GetPpuMemory().GetScanCycleNum();

    ++nextPoint.x;

    if(nextPoint.x > LAST_CYCLE)
    {
        //next scanline
        nextPoint.x = 0;
        ++nextPoint.y;

        if(nextPoint.y > LAST_SCANLINE)
        {
            //new frame
            nextPoint.y = -1;
        }
    }
    return nextPoint;
}

Ppu::Point Ppu::CalcNextBgrFetchTile(int curCycle, int curLine)
{
    if( (curLine >= PRE_SCANLINE && curLine < LAST_VISIBLE_SCANLINE)  && (curCycle >= START_NEXT_SCANLINE_FETCHING && curCycle <= LAST_NEXT_SCANLINE_FETCHING) )
    {
        //fetch from next scanline
        Ppu::Point nextTile;
        if(curLine <= -1)
        {
            nextTile.y = 0;
        }
        else
        {
            //convert from pixels to tiles (8 pixel per tile so bitshift 3 right)
            nextTile.y = curLine >> 3;
        }
        nextTile.x = (curCycle - START_NEXT_SCANLINE_FETCHING) >> 3;
        return nextTile;
    }
    else if( (curLine >= START_VISIBLE_SCANLINE && curLine <= LAST_VISIBLE_SCANLINE) && (curCycle >= START_VISIBLE_CYCLE && curCycle <= LAST_VISIBLE_CYCLE - 8) )
    {
        //fetch from current scanline
        Ppu::Point nextTile;
        nextTile.y = curLine >> 3;
        nextTile.x = (curCycle >> 3) + 2;
        return nextTile;
    }

    //fetching not needed at current scanline/cycle
    Ppu::Point invalidTile;
    invalidTile.x = -1;
    invalidTile.y = -1;
    return invalidTile;
}

bool Ppu::PowerCycle()
{
    //reset registers
    this->GetRegs().name.PPUCTRL = 0;
    this->GetRegs().name.PPUMASK = 0;
    this->GetRegs().name.PPUSTATUS = 0;
    this->GetRegs().name.OAMADDR = 0;
    this->GetRegs().name.PPUSCROLL = 0;
    this->GetRegs().name.PPUADDR = 0;
    this->GetRegs().name.PPUDATA = 0;
    
    //internal registers
    this->GetRegs().bgr.vramPpuAddress.val = 0;
    this->GetRegs().bgr.ppuAddressLatch = false;
    this->GetRegs().bgr.scrollX.val = 0;
    this->GetRegs().bgr.scrollY.val = 0;    
    this->GetRegs().bgr.ppuScrollLatch = false;
    this->GetRegs().bgr.ppuDataReadBuffer = 0;
    this->GetRegs().bgr.lsbPatternPlane.val = 0;
    this->GetRegs().bgr.msbPatternPlane.val = 0;
    this->GetRegs().bgr.lsbPalletePlane.val = 0;
    this->GetRegs().bgr.msbPalletePlane.val = 0;
    this->GetRegs().bgr.nextNametableIndex = 0;
    this->GetRegs().bgr.nextAttributeIndex = 0;
    this->GetRegs().bgr.lsbNextTile = 0;
    this->GetRegs().bgr.msbNextTile = 0;

    this->dma.GetPpuMemory().SetScanLineNum(-1);
    this->dma.GetPpuMemory().SetScanCycleNum(0);
    this->frameCountNum = 0;
    long long cycles = 0;
    this->dma.GetPpuMemory().SetTotalPpuCycles(cycles);
    this->dma.SetDmaBuffer(0);

    return true;
}

int Ppu::Step()
{
    int curLine = this->dma.GetPpuMemory().GetScanLineNum();
    int curCycle = this->dma.GetPpuMemory().GetScanCycleNum();

    //special cases
    if(curLine == PRE_SCANLINE && curCycle == START_VISIBLE_CYCLE)
    {
        //start of new frame so clear previous frame registers
        this->GetRegs().name.verticalBlank = false;
        this->GetRegs().name.spriteOverflow = false;
        ++this->frameCountNum;
    }
    else if( curLine == 241 && curCycle == START_VISIBLE_CYCLE )
    {
        //end of frame, set VBlank flag
        this->GetRegs().name.verticalBlank = true;
        if(this->GetRegs().name.generateNmi == true)
        {
            this->dma.SetNmi(true);
        }
    }

    //attempt to fetch next tile
    const Ppu::Point& fetchTile = this->CalcNextBgrFetchTile(curCycle, curLine);
    if(fetchTile.x != -1)
    {
        this->backgroundFetch(fetchTile, curCycle, curLine);
    }

    //fetch sprites
    this->SpriteFetch(curCycle, curLine);

    // visible scanlines
    if( (curLine >= START_VISIBLE_SCANLINE && curLine <= LAST_VISIBLE_SCANLINE) && (curCycle >= START_VISIBLE_CYCLE && curCycle <= LAST_VISIBLE_CYCLE) )
    {
        std::unique_ptr<Ppu::BackgroundPixelInfo> bPixel = this->calcBackgroundPixel();
        std::unique_ptr<Ppu::ForegroundPixelInfo> sPixel = this->calcForgroundPixel(curCycle);

        // -sprite priority with background
        // -if a sprite is present
        // -if sprite colour is transparent
        // -if background is transparent

        //pick between bPixel and sPixel
        rawColour pixelColour = bPixel->pixelColour;
        if(sPixel != nullptr)
        {
            pixelColour = sPixel->pixelColour;
        }
        if(this->GetRegs().name.showBackground == true)
        {
            int pixelX = curCycle - START_VISIBLE_CYCLE;
            int pixelY = curLine - START_VISIBLE_SCANLINE;
            this->framebuffer.Set(pixelY, pixelX, pixelColour);
        }
    }

    //move to next pixel
    const Ppu::Point& nextPoint = this->NextPixel();
    this->dma.GetPpuMemory().SetScanLineNum(nextPoint.y);
    this->dma.GetPpuMemory().SetScanCycleNum(nextPoint.x);

    this->dma.GetPpuMemory().IncTotalPpuCycles();
    return true;
}

//https://wiki.nesdev.com/w/index.php/PPU_rendering
void Ppu::backgroundFetch(const Ppu::Point& fetchTile, int curCycle, int curLine)
{
    //called between 1-256 and 321-337
    curCycle -= PRE_SCANLINE;
    switch(curCycle % 8)
    {
        case 0: //reload shift registers and get pattern index
        {
            //reload shift registers on 1(?), 9, 17, 25 ... 257
            this->GetRegs().bgr.lsbPatternPlane.upper = this->GetRegs().bgr.lsbNextTile;
            this->GetRegs().bgr.msbPatternPlane.upper = this->GetRegs().bgr.msbNextTile;
            this->GetRegs().bgr.lsbPalletePlane.upper = BitUtil::GetBits(this->GetRegs().bgr.nextAttributeIndex, 0) ? 0xFF : 0x00;
            this->GetRegs().bgr.msbPalletePlane.upper = BitUtil::GetBits(this->GetRegs().bgr.nextAttributeIndex, 1) ? 0xFF : 0x00;

            //get next pattern
            this->GetRegs().bgr.nextNametableIndex = this->dma.GetPpuMemory().GetNameTables().GetPatternIndex(fetchTile.y, fetchTile.x);
            break;
        }
        case 2: //get next pallette
        {
            this->GetRegs().bgr.nextAttributeIndex = this->dma.GetPpuMemory().GetNameTables().GetPaletteIndex(fetchTile.y, fetchTile.x);
            break;
        }

        case 4:
        {
            //get lower pattern byte
            int tableNum = this->GetRegs().name.backgroundPatternTable;
            //TODO scrolling
            PatternTables::BitTile& bitTile = this->dma.GetPatternTile(tableNum, this->GetRegs().bgr.nextNametableIndex);
            byte fineY = curLine % PatternTables::TILE_HEIGHT;
            byte revBitPlane = bitTile.LsbPlane[fineY];
            this->GetRegs().bgr.lsbNextTile = BitUtil::FlipByte(revBitPlane);
            break;
        }

        case 6:
        {
            //get upper pattern byte
            int tableNum = this->GetRegs().name.backgroundPatternTable;
            //TODO scrolling
            PatternTables::BitTile& bitTile = this->dma.GetPatternTile(tableNum, this->GetRegs().bgr.nextNametableIndex);
            byte fineY = curLine % PatternTables::TILE_HEIGHT;
            byte revBitPlane = bitTile.MsbPlane[fineY];
            this->GetRegs().bgr.msbNextTile = BitUtil::FlipByte(revBitPlane);
            break;
        }
    }
}

std::unique_ptr<Ppu::BackgroundPixelInfo> Ppu::calcBackgroundPixel()
{
    //get pattern value
    byte fineX = this->GetRegs().bgr.scrollX.fineX;
    bit lsbPattern = BitUtil::GetBits(this->GetRegs().bgr.lsbPatternPlane.lower, fineX);
    bit msPattern = BitUtil::GetBits(this->GetRegs().bgr.msbPatternPlane.lower, fineX);
    byte patternValue = (msPattern << 1) | lsbPattern;

    //get palette index
    bit lsbPallete = BitUtil::GetBits(this->GetRegs().bgr.lsbPalletePlane.lower, fineX);
    bit msbPallete = BitUtil::GetBits(this->GetRegs().bgr.msbPalletePlane.lower, fineX);
    byte attributeIndex = (msbPallete << 1) | lsbPallete;

    //shift background specific registers
    this->GetRegs().bgr.lsbPatternPlane.val >>= 1;
    this->GetRegs().bgr.msbPatternPlane.val >>= 1;
    this->GetRegs().bgr.lsbPalletePlane.val >>= 1;
    this->GetRegs().bgr.msbPalletePlane.val >>= 1;

    rawColour nesColour = this->dma.GetPpuMemory().GetPalettes().PatternValueToColour(attributeIndex, patternValue);
    std::unique_ptr<Ppu::BackgroundPixelInfo> backgroundPixel = std::make_unique<Ppu::BackgroundPixelInfo>();
    backgroundPixel->pixelColour = nesColour;
    backgroundPixel->isTransparent = (patternValue == 0);
    return backgroundPixel;
}

std::unique_ptr<Ppu::ForegroundPixelInfo> Ppu::calcForgroundPixel(int curCycle)
{
    OamSecondary& secondaryOam = this->dma.GetPpuMemory().GetSecondaryOam();
    const OamSecondary::SpritePixel& spritePixel = secondaryOam.CalcForgroundPixel(curCycle);

    if(spritePixel.primaryOamIndex < 0)
    {
        // no sprite at current PPU cycle
        return nullptr;
    }

    // lookup colour for pattern value (only can use the last 4 palletes)
    rawColour nesColour = this->dma.GetPpuMemory().GetPalettes().PatternValueToColour(4, spritePixel.pattern);

    // handle sprite zero hits here (cycle 4)
    std::unique_ptr<Ppu::ForegroundPixelInfo> forgroundPixel = std::make_unique<Ppu::ForegroundPixelInfo>();
    forgroundPixel->frontOfBackground = true;
    forgroundPixel->isTransparent = (spritePixel.pattern == 0);
    forgroundPixel->pixelColour = nesColour;
    return forgroundPixel;
}

void Ppu::SpriteFetch(int curCycle, int curLine)
{
    // cycle breakdown and high level description for sprite fetching can be found here:
    // https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation#Details

    int nextScanline = curLine + 1;
    if(nextScanline <= LAST_VISIBLE_SCANLINE)
    {
        // moving sprites on next scanline to secondary OAM
        OamPrimary& primaryOam = this->dma.GetPpuMemory().GetPrimaryOam();
        OamSecondary& secondaryOam = this->dma.GetPpuMemory().GetSecondaryOam();

        // according to diagram below clearing and sprite evaluation don't happen on pre scanline:
        // https://wiki.nesdev.com/w/images/d/d1/Ntsc_timing.png
        if(nextScanline >= START_VISIBLE_SCANLINE)
        {
            if(curCycle == START_VISIBLE_CYCLE)
            {
                // clear previous secondary OAM
                secondaryOam.Clear();
            }
            else if( curCycle == START_SPRITE_EVALUATION_CYCLE )
            {
                // load sprites on the next scanline into secondary OAM
                for(int i = 0; i < OamPrimary::TotalNumOfSprites; ++i)
                {
                    const OamPrimary::Sprite& primarySprite = primaryOam.GetSprite(i);

                    // check if sprite is on next scanline
                    int spriteHeight = PatternTables::TILE_HEIGHT;
                    if(this->GetRegs().name.spriteSize)
                    {
                        spriteHeight = 2 * PatternTables::TILE_HEIGHT;
                    }
                    if(nextScanline >= primarySprite.posY && nextScanline < primarySprite.posY + spriteHeight)
                    {
                        //sprite appears on next scanline
                        secondaryOam.AppendSprite(primarySprite, i);
                    }
                }

                // set overflow register
                this->GetRegs().name.spriteOverflow = (secondaryOam.GetActiveSpriteNum() > 8);
            }
        }

        if(curCycle == START_SPRITE_TILE_FETCH)
        {
            // set sprite buffers
            for(int i = 0; i < secondaryOam.GetActiveSpriteNum(); ++i)
            {
                const OamPrimary::Sprite& activeSprite = secondaryOam.GetSprite(i);
                patternIndex spriteIndex = activeSprite.index;

                OamSecondary::SpritePatternTiles spriteTiles;
                if(this->GetRegs().name.spriteSize == false)
                {
                    //sprites are 8 x 8
                    spriteTiles.numOfTiles = 1;
                    int tableNum = this->GetRegs().name.sprite8x8PatternTable;
                    spriteTiles.firstTile = this->dma.GetPatternTile(tableNum, spriteIndex);
                }
                else
                {
                    //sprites are 8 x 16
                    spriteTiles.numOfTiles = 2;
                    int tableNum = BitUtil::GetBits(activeSprite.index, 0);
                    patternIndex spritePattern = BitUtil::GetBits(spriteIndex, 1, 7);
                    // int yOffset = curCycle - activeSprite.posY;
                    // if(yOffset >= 8)
                    // {
                    //     //lower pattern table
                    //     spritePattern += 1;
                    // }
                    spriteTiles.firstTile = this->dma.GetPatternTile(tableNum, spritePattern);
                    spriteTiles.secondTile = this->dma.GetPatternTile(tableNum, spritePattern + 1);
                }
                const OamSecondary::ScanlineTile& spriteBuffer = secondaryOam.CalcSpriteBuffer(nextScanline, activeSprite, spriteTiles);
                secondaryOam.SetSpriteScanlineTile(i, spriteBuffer);
            }
        }
    }




    //sprite evaluation - Turn the attribute data and the pattern table data into palette indices, and combine them with data from sprite data using priority.
    
    //decrement x-position counters in all 8 OAM sprites
    //make OAM sprite active if x-position == 0
    //shift shift registers for each active OAM once every cycle    
}

PpuRegisters& Ppu::GetRegs()
{
    return this->dma.GetPpuMemory().GetRegisters();
}

std::unique_ptr<Matrix<rawColour>> Ppu::GeneratePatternTables()
{
    int outputWidth = PatternTables::NUM_OF_TABLES * PatternTables::TILE_WIDTH * PatternTables::TABLE_WIDTH;
    int outputHeight = PatternTables::TILE_HEIGHT * PatternTables::TABLE_HEIGHT;
    std::unique_ptr<Matrix<rawColour>> outputPatterns = std::unique_ptr<Matrix<rawColour>>( new Matrix<rawColour>(outputWidth, outputHeight) );

    std::unique_ptr<PatternTables> patternTables = this->dma.GeneratePatternTablesFromRom();
    for(int tableNum = 0; tableNum < PatternTables::NUM_OF_TABLES; ++tableNum)
    {
        for(int y = 0; y < PatternTables::TABLE_HEIGHT; ++y)
        {
            for(int x = 0; x < PatternTables::TABLE_WIDTH; ++x)
            {
                Matrix<byte> tilePattern = patternTables->GetTile(tableNum, y, x);
                for(int row = 0; row < PatternTables::TILE_HEIGHT; ++row)
                {
                    for(int col = 0; col < PatternTables::TILE_WIDTH; ++col)
                    {
                        byte patternVal = tilePattern.Get(row, col);

                        rawColour pixelVal = this->dma.GetPpuMemory().GetPalettes().PatternValueToColour(this->defaultPalette, patternVal);

                        int yPos = (y * PatternTables::TILE_HEIGHT) + row;
                        int xPos = (tableNum * PatternTables::TILE_WIDTH * PatternTables::TABLE_WIDTH) + (x * PatternTables::TILE_WIDTH) + col;
                        outputPatterns->Set(yPos, xPos, pixelVal);
                    }
                }
            }
        }
    }
    return outputPatterns;
}

std::unique_ptr<Matrix<rawColour>> Ppu::GenerateColourPalettes()
{
    const int colourWidth = 3;
    const int borderWidth = 2;
    rawColour backgroundColour;
    backgroundColour.raw = 0x000000FF;

    std::unique_ptr<Matrix<rawColour>> colourOutput = std::unique_ptr<Matrix<rawColour>>( new Matrix<rawColour>(borderWidth * 9 + colourWidth * 8, borderWidth + colourWidth * 4 + borderWidth, backgroundColour) );

    int curCol = borderWidth;
    ColourPalettes& palettes = this->dma.GetPpuMemory().GetPalettes();
    for(int paletteId = 0; paletteId < 8; ++paletteId)
    {
        if(paletteId == this->defaultPalette)
        {
            rawColour selectedColour = {0xBBBBBBFF};
            colourOutput->SetRegion(0, curCol - borderWidth, colourWidth + 2 * borderWidth, colourOutput->GetHeight(), selectedColour);
        }
        for(int colourNum = 0; colourNum < 4; ++colourNum)
        {
            rawColour colour = palettes.PatternValueToColour(paletteId, colourNum);
            colourOutput->SetRegion(borderWidth + (colourWidth * colourNum), curCol, colourWidth, colourWidth, colour);
        }
        curCol += colourWidth;
        curCol += borderWidth;
    }

    return colourOutput;
}

const Matrix<rawColour>& Ppu::GetFrameDisplay()
{
    return this->framebuffer;
}

byte Ppu::GetDefaultPalette() const
{
    return this->defaultPalette;
}

void Ppu::SetDefaultPalette(byte palette)
{
    this->defaultPalette = palette;
}

long long int& Ppu::GetTotalFrameCount()
{
    return this->frameCountNum;
}