#include "Ppu.h"

Point Ppu::calcNextPosition(int curCycle, int curLine)
{
    Point nextPoint;
    nextPoint.y = curLine;
    nextPoint.x = curCycle;

    ++nextPoint.x;

    if(nextPoint.x > LAST_CYCLE)
    {
        //next scanline
        nextPoint.x = 0;
        ++nextPoint.y;

        if(nextPoint.y > LAST_SCANLINE)
        {
            //new frame
            nextPoint.y = PRE_SCANLINE;
        }
    }
    return nextPoint;
}

Point Ppu::CalcNextFetchPixel(int curCycle, int curLine)
{
    if( (curLine >= PRE_SCANLINE && curLine < LAST_VISIBLE_SCANLINE)  && (curCycle >= START_NEXT_SCANLINE_FETCHING_CYCLE && curCycle <= LAST_NEXT_SCANLINE_FETCHING_CYCLE) )
    {
        //fetch from next scanline
        int nextYPixel = (curLine - START_VISIBLE_SCANLINE) + 1;
        int nextXPixel = (curCycle - START_NEXT_SCANLINE_FETCHING_CYCLE);

        Point nextFetchPixel;
        nextFetchPixel.y = nextYPixel;
        nextFetchPixel.x = nextXPixel;
        return nextFetchPixel;
    }
    else if( (curLine >= START_VISIBLE_SCANLINE && curLine <= LAST_VISIBLE_SCANLINE) && (curCycle >= START_VISIBLE_CYCLE && curCycle <= LAST_VISIBLE_FETCH_CYCLE) )
    {
        //fetch from current scanline
        int nextYPixel = curLine - START_VISIBLE_SCANLINE;
        int nextXPixel = (curCycle - START_VISIBLE_CYCLE) + (2 * PatternTables::TILE_HEIGHT);

        Point nextFetchPixel;
        nextFetchPixel.y = nextYPixel;
        nextFetchPixel.x = nextXPixel;
        return nextFetchPixel;
    }

    //fetching not needed at current scanline/cycle
    Point invalidFetchPixel;
    invalidFetchPixel.x = -1;
    invalidFetchPixel.y = -1;
    return invalidFetchPixel;
}

void Ppu::backgroundFetch(const Point& fetchTile, int curCycle, int curLine)
{
    //https://wiki.nesdev.com/w/index.php/PPU_rendering
    //called between 1-248 and 321-336
    switch(curCycle % 8)
    {
        case 1: //get pattern index
        {
            // reload shift registers
            this->GetRegs().bgr.lsbPatternPlane.upper = this->GetRegs().bgr.lsbNextTile;
            this->GetRegs().bgr.msbPatternPlane.upper = this->GetRegs().bgr.msbNextTile;
            this->GetRegs().bgr.lsbPalletePlane.upper = BitUtil::GetBits(this->GetRegs().bgr.nextAttributeIndex, 0) ? 0xFF : 0x00;
            this->GetRegs().bgr.msbPalletePlane.upper = BitUtil::GetBits(this->GetRegs().bgr.nextAttributeIndex, 1) ? 0xFF : 0x00;

            this->GetRegs().bgr.nextNametableIndex = this->dma.GetPpuMemory().GetNameTables().GetPatternIndex(fetchTile.y, fetchTile.x);
            break;
        }
        case 3: //get next pallette
        {
            this->GetRegs().bgr.nextAttributeIndex = this->dma.GetPpuMemory().GetNameTables().GetPaletteIndex(fetchTile.y, fetchTile.x);
            break;
        }

        case 5:
        {
            //get lower pattern byte
            int tableNum = this->GetRegs().name.backgroundPatternTable;
            //TODO scrolling
            PatternTables::BitTile& bitTile = this->dma.GetPatternTile(tableNum, this->GetRegs().bgr.nextNametableIndex);

            Point nextFetchPixel = this->CalcNextFetchPixel(curCycle, curLine);
            if(nextFetchPixel.y < 0 )
            {
                // shouldn't be doing a background fetch on this cycle
                throw std::invalid_argument("invalid background fetch");
            }
            byte fineY = nextFetchPixel.y % PatternTables::TILE_HEIGHT;
            byte revBitPlane = bitTile.LsbPlane[fineY];
            this->GetRegs().bgr.lsbNextTile = BitUtil::FlipByte(revBitPlane);
            break;
        }

        case 7:
        {
            //get upper pattern byte
            int tableNum = this->GetRegs().name.backgroundPatternTable;
            //TODO scrolling
            PatternTables::BitTile& bitTile = this->dma.GetPatternTile(tableNum, this->GetRegs().bgr.nextNametableIndex);
            
            Point nextFetchPixel = this->CalcNextFetchPixel(curCycle, curLine);
            if(nextFetchPixel.y < 0 )
            {
                // shouldn't be doing a background fetch on this cycle
                throw std::invalid_argument("invalid background fetch");
            }
            byte fineY = nextFetchPixel.y % PatternTables::TILE_HEIGHT;
            byte revBitPlane = bitTile.MsbPlane[fineY];
            this->GetRegs().bgr.msbNextTile = BitUtil::FlipByte(revBitPlane);
            break;
        }
    }
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

        if(curCycle == START_SPRITE_TILE_FETCH_CYCLE)
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
                    spriteTiles.firstTile = this->dma.GetPatternTile(tableNum, spritePattern);
                    spriteTiles.secondTile = this->dma.GetPatternTile(tableNum, spritePattern + 1);
                }
                const OamSecondary::ScanlineTile& spriteBuffer = secondaryOam.CalcSpriteBuffer(nextScanline, activeSprite, spriteTiles);
                secondaryOam.SetSpriteScanlineTile(i, spriteBuffer);
            }
        }
    } 
}

Ppu::BackgroundPixelInfo Ppu::CalcBackgroundPixel()
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
    Ppu::BackgroundPixelInfo backgroundPixel;
    backgroundPixel.pixelColour = nesColour;
    backgroundPixel.isTransparent = (patternValue == 0);
    return backgroundPixel;
}

Ppu::ForegroundPixelInfo Ppu::CalcForgroundPixel(int curCycle)
{
    OamSecondary& secondaryOam = this->dma.GetPpuMemory().GetSecondaryOam();
    const OamSecondary::SpritePixel& spritePixel = secondaryOam.CalcForgroundPixel(curCycle);

    if(spritePixel.primaryOamIndex < 0)
    {
        // no sprite at current PPU cycle. just say the pixel has a transparent foreground
        ForegroundPixelInfo forgroundPixel;
        forgroundPixel.isTransparent = true;
        forgroundPixel.primaryOamIndex = -1;
        return forgroundPixel;
    }

    // lookup colour for pattern value (only can use the last 4 palletes)
    const OamPrimary::Sprite& selectedSprite = secondaryOam.GetSprite(spritePixel.primaryOamIndex);
    rawColour nesColour = this->dma.GetPpuMemory().GetPalettes().PatternValueToColour(4 + selectedSprite.palette, spritePixel.pattern);


    //return pixel
    ForegroundPixelInfo forgroundPixel;
    forgroundPixel.frontOfBackground = !selectedSprite.backgroundPriority;
    forgroundPixel.isTransparent = (spritePixel.pattern == 0);
    forgroundPixel.pixelColour = nesColour;
    forgroundPixel.primaryOamIndex = spritePixel.primaryOamIndex;
    return forgroundPixel;
}

rawColour Ppu::CalcFinalPixel(const Ppu::BackgroundPixelInfo& bPixel, const Ppu::ForegroundPixelInfo& sPixel, int curCycle)
{
    rawColour pixelColour = bPixel.pixelColour;
    if(this->GetRegs().name.showSprites == false || sPixel.isTransparent)
    {
        //sprites disabled or transparent. draw background
        pixelColour = bPixel.pixelColour;
    }
    else if(this->GetRegs().name.showBackground == false || bPixel.isTransparent)
    {
        //background disabled draw sprites
        pixelColour = sPixel.pixelColour;
    }
    else
    {
        // sprites and background enabled

        // handle sprite zero hit
        if(sPixel.primaryOamIndex == 0)
        {
            //following logic detailed here:
            // https://wiki.nesdev.com/w/index.php/PPU_OAM#Sprite_zero_hits
            if(curCycle > 7 || (this->GetRegs().name.showBackgroundLeftmost && this->GetRegs().name.showBackgroundLeftmost))
            {
                if(curCycle != 255)
                {
                    this->GetRegs().name.spriteZeroHit = true;
                }
            }
        }

        if(sPixel.frontOfBackground)
        {
            pixelColour = sPixel.pixelColour;
        }
        else
        {
            pixelColour = bPixel.pixelColour;
        }
    }

    return pixelColour;
}

PpuRegisters& Ppu::GetRegs()
{
    return this->dma.GetPpuMemory().GetRegisters();
}

Ppu::Ppu(Dma& dma)
: dma(dma), framebuffer(256, 240, {0x000000FF})
{
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

    this->dma.GetPpuMemory().SetScanLineNum(PRE_SCANLINE);
    this->dma.GetPpuMemory().SetScanCycleNum(START_CYCLE);
    this->frameCountNum = 0;
    long long cycles = 0;
    this->dma.GetPpuMemory().SetTotalPpuCycles(cycles);
    this->dma.SetDmaBuffer(0);

    return true;
}

void Ppu::Step()
{
    int curLine = this->dma.GetPpuMemory().GetScanLineNum();
    int curCycle = this->dma.GetPpuMemory().GetScanCycleNum();

    //special cases
    if(curLine == PRE_SCANLINE && curCycle == START_VISIBLE_CYCLE)
    {
        //start of new frame so clear previous frame registers
        this->GetRegs().name.verticalBlank = false;
        this->GetRegs().name.spriteOverflow = false;
        this->GetRegs().name.spriteZeroHit = false;
        ++this->frameCountNum;
    }
    else if( curLine == VBLANK_SCANLINE && curCycle == START_VISIBLE_CYCLE )
    {
        //end of frame, set VBlank flag
        this->GetRegs().name.verticalBlank = true;
        if(this->GetRegs().name.generateNmi == true)
        {
            this->dma.SetNmi(true);
        }
    }

    //attempt to fetch next tile
    const Point& fetchPixel = this->CalcNextFetchPixel(curCycle, curLine);
    const Point& fetchTile = this->dma.GetPpuMemory().GetNameTables().CalcBgrFetchTile(fetchPixel);
    if(fetchTile.x != -1)
    {
        this->backgroundFetch(fetchTile, curCycle, curLine);
    }

    //fetch sprites
    this->SpriteFetch(curCycle, curLine);

    // visible scanlines
    if( (curLine >= START_VISIBLE_SCANLINE && curLine <= LAST_VISIBLE_SCANLINE) && (curCycle >= START_VISIBLE_CYCLE && curCycle <= LAST_VISIBLE_CYCLE) )
    {
        Ppu::BackgroundPixelInfo bPixel = this->CalcBackgroundPixel();
        Ppu::ForegroundPixelInfo sPixel = this->CalcForgroundPixel(curCycle);

        //picks between bPixel and sPixel
        rawColour pixelColour = this->CalcFinalPixel(bPixel, sPixel, curCycle);

        // draw pixel to the view
        int pixelX = curCycle - START_VISIBLE_CYCLE;
        int pixelY = curLine - START_VISIBLE_SCANLINE;
        this->framebuffer.Set(pixelY, pixelX, pixelColour);
    }

    //move to next pixel
    const Point& nextPoint = this->calcNextPosition(curCycle, curLine);
    this->dma.GetPpuMemory().SetScanLineNum(nextPoint.y);
    this->dma.GetPpuMemory().SetScanCycleNum(nextPoint.x);

    this->dma.GetPpuMemory().IncTotalPpuCycles();
}

const Matrix<rawColour>& Ppu::GetFrameDisplay()
{
    return this->framebuffer;
}

std::unique_ptr<Matrix<rawColour>> Ppu::GeneratePatternTables()
{
    ColourPalettes& colourPalettes = this->dma.GetPpuMemory().GetPalettes();
    return this->dma.GeneratePatternTablesFromRom()->GeneratePatternDisplay(colourPalettes, this->disassemblePalette);
}

std::unique_ptr<Matrix<rawColour>> Ppu::GenerateColourPalettes()
{
    return this->dma.GetPpuMemory().GetPalettes().GenerateColourPalettes(this->disassemblePalette);
}

byte Ppu::GetDisassemblePalette() const
{
    return this->disassemblePalette;
}

void Ppu::SetDisassemblePalette(byte palette)
{
    this->disassemblePalette = palette;
}

const long long int& Ppu::GetTotalFrameCount() const
{
    return this->frameCountNum;
}