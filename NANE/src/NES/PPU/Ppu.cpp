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

std::unique_ptr<Ppu::BackgroundFetchInfo> Ppu::backgroundFetch(const Point& fetchTile, int curCycle, int curLine)
{
    if(fetchTile.x < 0 || fetchTile.y < 0)
    {
        //no valid fetch tile
        return nullptr;
    }

    //https://wiki.nesdev.com/w/index.php/PPU_rendering
    //called between cycles 1-248 and 321-336
    switch(curCycle % 8)
    {
        case 1: //get pattern index
        {
            this->GetRegs().vRegs.nextNametableIndex = this->dma.GetPpuMemory().GetNameTables().GetPatternIndex(fetchTile.y, fetchTile.x);

            // return updated shift registers
            std::unique_ptr<Ppu::BackgroundFetchInfo> updatedShiftRegisters = std::make_unique<Ppu::BackgroundFetchInfo>();
            updatedShiftRegisters->lsbFetchPattern = this->GetRegs().vRegs.backgroundFetchTileLsb;
            updatedShiftRegisters->msbFetchPattern = this->GetRegs().vRegs.backgroundFetchTileMsb;
            updatedShiftRegisters->paletteColour = this->GetRegs().vRegs.nextAttributeIndex;
            return updatedShiftRegisters;
            break;
        }
        case 3: //get next pallette
        {
            this->GetRegs().vRegs.nextAttributeIndex = this->dma.GetPpuMemory().GetNameTables().GetPaletteIndex(fetchTile.y, fetchTile.x);
            break;
        }

        case 5: //get lower pattern byte
        {
            int tableNum = this->GetRegs().name.backgroundPatternTable;
            PatternTables::BitTile& bitTile = this->dma.GetPatternTile(tableNum, this->GetRegs().vRegs.nextNametableIndex);

            Point nextFetchPixel = this->CalcNextFetchPixel(curCycle, curLine);
            if(nextFetchPixel.y < 0 )
            {
                // shouldn't be doing a background fetch on this cycle
                throw std::invalid_argument("invalid background fetch");
            }
            //TODO scrolling
            byte tileOffset = nextFetchPixel.y % PatternTables::TILE_HEIGHT;
            byte revBitPlane = bitTile.lsbPlane[tileOffset];
            this->GetRegs().vRegs.backgroundFetchTileLsb = BitUtil::FlipByte(revBitPlane);
            break;
        }

        case 7:
        {
            int tableNum = this->GetRegs().name.backgroundPatternTable;
            PatternTables::BitTile& bitTile = this->dma.GetPatternTile(tableNum, this->GetRegs().vRegs.nextNametableIndex);

            Point nextFetchPixel = this->CalcNextFetchPixel(curCycle, curLine);
            if(nextFetchPixel.y < 0 )
            {
                // shouldn't be doing a background fetch on this cycle
                throw std::invalid_argument("invalid background fetch");
            }
            //TODO scrolling
            byte tileOffset = nextFetchPixel.y % PatternTables::TILE_HEIGHT;
            byte revBitPlane = bitTile.msbPlane[tileOffset];
            this->GetRegs().vRegs.backgroundFetchTileMsb = BitUtil::FlipByte(revBitPlane);
            break;
        }
    }
    return nullptr;
}

void Ppu::SpriteFetch(int curCycle, int curLine)
{
    // cycle breakdown and high level description for sprite fetching can be found here:
    // https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation#Details

    // fetching sprites for the next scanline
    int nextScanline = curLine + 1;
    if(nextScanline <= LAST_VISIBLE_SCANLINE)
    {
        // moving sprites on next scanline to secondary OAM
        OamPrimary& primaryOam = this->dma.GetPpuMemory().GetPrimaryOam();
        OamSecondary& secondaryOam = this->dma.GetPpuMemory().GetSecondaryOam();

        if(curCycle == START_VISIBLE_CYCLE && nextScanline) // clear previous secondary OAM
        {
            secondaryOam.ClearFetchData();
        }
        else if( curCycle == START_SPRITE_EVALUATION_CYCLE ) // load sprites on the next scanline into secondary OAM
        {
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
                    secondaryOam.AppendFetchedSprite(primarySprite, i);
                }
            }

            // set overflow register
            this->GetRegs().name.spriteOverflow = (secondaryOam.GetSpriteFetchNum() > 8);
        }

        if(curCycle == START_SPRITE_TILE_FETCH_CYCLE) // update active sprite buffers
        {
            //clear sprite buffer by setting new size
            secondaryOam.ClearActiveBuffer();
            for(int i = 0; i < secondaryOam.GetSpriteFetchNum(); ++i)
            {
                const OamSecondary::IndexedSprite& indexedSprite = secondaryOam.GetFetchedSprite(i);
                patternIndex spriteIndex = indexedSprite.sprite.index;

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
                    int tableNum = BitUtil::GetBits(spriteIndex, 0);
                    patternIndex spritePattern = BitUtil::GetBits(spriteIndex, 1, 7);
                    spriteTiles.firstTile = this->dma.GetPatternTile(tableNum, spritePattern);
                    spriteTiles.secondTile = this->dma.GetPatternTile(tableNum, spritePattern + 1);
                }
                OamSecondary::IndexSpriteBuffer indexedScanline;
                indexedScanline.primaryOamIndex = indexedSprite.primaryOamIndex;
                indexedScanline.sprite = indexedSprite.sprite;
                indexedScanline.scanlineTile = secondaryOam.CalcSpriteBuffer(nextScanline, indexedSprite.sprite, spriteTiles);
                secondaryOam.AppendToActiveBuffer(indexedScanline);
            }
        }
    }
}

Ppu::BackgroundPixelInfo Ppu::CalcBackgroundPixel(int curCycle, const PpuRegisters::VirtualRegisters::BackgroundDrawRegisters& bDrawingRegs)
{
    //get pattern value
    byte bufferOffset = (curCycle - START_VISIBLE_CYCLE) % 8;
    bufferOffset = bufferOffset + bDrawingRegs.scrollX.fineX;
    bit lsbPattern = BitUtil::GetBits(bDrawingRegs.lsbPatternPlane.val, bufferOffset);
    bit msPattern = BitUtil::GetBits(bDrawingRegs.msbPatternPlane.val, bufferOffset);
    byte patternValue = (msPattern << 1) | lsbPattern;

    //get palette index
    bit lsbPallete = BitUtil::GetBits(bDrawingRegs.lsbPalletePlane.val, bufferOffset);
    bit msbPallete = BitUtil::GetBits(bDrawingRegs.msbPalletePlane.val, bufferOffset);
    byte attributeIndex = (msbPallete << 1) | lsbPallete;

    rawColour nesColour = this->dma.GetPpuMemory().GetPalettes().PatternValueToColour(attributeIndex, patternValue);
    Ppu::BackgroundPixelInfo backgroundPixel;
    backgroundPixel.pixelColour = nesColour;
    backgroundPixel.isTransparent = (patternValue == 0);
    return backgroundPixel;
}

Ppu::ForegroundPixelInfo Ppu::CalcForgroundPixel(int curCycle)
{
    OamSecondary& secondaryOam = this->dma.GetPpuMemory().GetSecondaryOam();
    int pixelX = curCycle - START_VISIBLE_CYCLE;
    const OamSecondary::IndexPattern& forgroundSprite = secondaryOam.CalcForgroundPixel(pixelX);

    if(forgroundSprite.primaryOamIndex < 0)
    {
        // no sprite at current PPU cycle. just say the pixel has a transparent foreground
        ForegroundPixelInfo forgroundPixel;
        forgroundPixel.isTransparent = true;
        forgroundPixel.primaryOamIndex = -1;
        return forgroundPixel;
    }

    // lookup colour for pattern value (only can use the last 4 palletes)
    const OamPrimary::Sprite& selectedSprite = forgroundSprite.sprite;
    rawColour nesColour = this->dma.GetPpuMemory().GetPalettes().PatternValueToColour(4 + selectedSprite.palette, forgroundSprite.pattern);


    //return pixel
    ForegroundPixelInfo forgroundPixel;
    forgroundPixel.frontOfBackground = !selectedSprite.backgroundPriority;
    forgroundPixel.isTransparent = (forgroundSprite.pattern == 0);
    forgroundPixel.pixelColour = nesColour;
    forgroundPixel.primaryOamIndex = forgroundSprite.primaryOamIndex;
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
    this->GetRegs().vRegs.vramPpuAddress.val = 0;
    this->GetRegs().vRegs.ppuAddressLatch = false;
    this->GetRegs().vRegs.bckgndDrawing.scrollX.val = 0;
    this->GetRegs().vRegs.bckgndDrawing.scrollY.val = 0;    
    this->GetRegs().vRegs.ppuScrollLatch = false;
    this->GetRegs().vRegs.ppuDataReadBuffer = 0;
    this->GetRegs().vRegs.bckgndDrawing.lsbPatternPlane.val = 0;
    this->GetRegs().vRegs.bckgndDrawing.msbPatternPlane.val = 0;
    this->GetRegs().vRegs.bckgndDrawing.lsbPalletePlane.val = 0;
    this->GetRegs().vRegs.bckgndDrawing.msbPalletePlane.val = 0;
    this->GetRegs().vRegs.nextNametableIndex = 0;
    this->GetRegs().vRegs.nextAttributeIndex = 0;
    this->GetRegs().vRegs.backgroundFetchTileLsb = 0;
    this->GetRegs().vRegs.backgroundFetchTileMsb = 0;

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

    //fetch background
    PpuRegisters::VirtualRegisters::BackgroundDrawRegisters& bDrawingRegs = this->GetRegs().vRegs.bckgndDrawing;
    const Point& fetchPixel = this->CalcNextFetchPixel(curCycle, curLine);
    const Point& fetchTile = this->dma.GetPpuMemory().GetNameTables().CalcBgrFetchTile(fetchPixel);
    std::unique_ptr<Ppu::BackgroundFetchInfo> fetchInfo = this->backgroundFetch(fetchTile, curCycle, curLine);
    if(fetchInfo != nullptr)
    {
        // new background fetch data - update shift registers
        bDrawingRegs.lsbPatternPlane.lower = bDrawingRegs.lsbPatternPlane.upper;
        bDrawingRegs.msbPatternPlane.lower = bDrawingRegs.msbPatternPlane.upper;
        bDrawingRegs.lsbPalletePlane.lower = bDrawingRegs.lsbPalletePlane.upper;
        bDrawingRegs.msbPalletePlane.lower = bDrawingRegs.msbPalletePlane.upper;

        bDrawingRegs.lsbPatternPlane.upper = fetchInfo->lsbFetchPattern;
        bDrawingRegs.msbPatternPlane.upper = fetchInfo->msbFetchPattern;
        bDrawingRegs.lsbPalletePlane.upper = BitUtil::GetBits(fetchInfo->paletteColour, 0) ? 0xFF : 0x00;
        bDrawingRegs.msbPalletePlane.upper = BitUtil::GetBits(fetchInfo->paletteColour, 1) ? 0xFF : 0x00;
    }

    //fetch sprites
    this->SpriteFetch(curCycle, curLine);

    // visible scanlines
    if( (curLine >= START_VISIBLE_SCANLINE && curLine <= LAST_VISIBLE_SCANLINE) && (curCycle >= START_VISIBLE_CYCLE && curCycle <= LAST_VISIBLE_CYCLE) )
    {
        Ppu::BackgroundPixelInfo bPixel = this->CalcBackgroundPixel(curCycle, bDrawingRegs);
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