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
    Point nextFetchPixel;
    nextFetchPixel.x = -1;
    nextFetchPixel.y = -1;
    if( (curLine >= PRE_SCANLINE && curLine < LAST_VISIBLE_SCANLINE)  && (curCycle >= START_NEXT_SCANLINE_FETCHING_CYCLE && curCycle <= LAST_NEXT_SCANLINE_FETCHING_CYCLE) )
    {
        //fetch from next scanline
        nextFetchPixel.x = (curCycle - START_NEXT_SCANLINE_FETCHING_CYCLE);
        nextFetchPixel.y = (curLine - START_VISIBLE_SCANLINE) + 1;
    }
    else if( (curLine >= START_VISIBLE_SCANLINE && curLine <= LAST_VISIBLE_SCANLINE) && (curCycle >= START_VISIBLE_CYCLE && curCycle <= LAST_VISIBLE_FETCH_CYCLE) )
    {
        //fetch from current scanline
        nextFetchPixel.x = (curCycle - START_VISIBLE_CYCLE) + (2 * PatternTables::TILE_HEIGHT);
        nextFetchPixel.y = curLine - START_VISIBLE_SCANLINE;
    }

    //handle scrolling
    nextFetchPixel.x += this->GetRegs().GetActiveScrollX();
    nextFetchPixel.y += this->GetRegs().GetActiveScrollY();

    //handle wrapping at ends
    nextFetchPixel.x %= NameTables::nametablesWidth;
    nextFetchPixel.y %= NameTables::nametablesHeight;

    return nextFetchPixel;
}

std::unique_ptr<Ppu::BackgroundFetchInfo> Ppu::backgroundFetch(int curCycle, int curLine)
{
    //update scroll registers
    if((curLine >= PRE_SCANLINE && curLine <= LAST_VISIBLE_SCANLINE) && curCycle == TRANSFER_SCOLLX_CYCLE)
    {
        if(this->GetRegs().name.showSprites || this->GetRegs().name.showBackground)
        {
            this->GetRegs().TransferScrollX();
        }
    }

    if(curLine == PRE_SCANLINE && curCycle >= TRANSFER_SCROLLY_START_CYCLE && curCycle <= TRANSFER_SCROLLY_END_CYCLE)
    {
        if(this->GetRegs().name.showSprites || this->GetRegs().name.showBackground)
        {
            this->GetRegs().TransferScrollY();
        }
    }

    Point fetchPixel = this->CalcNextFetchPixel(curCycle, curLine);
    if(fetchPixel.x < 0 || fetchPixel.y < 0)
    {
        //no valid fetch pixel
        return nullptr;
    }

    //https://wiki.nesdev.com/w/index.php/PPU_rendering
    //called between cycles 1-248 and 321-336
    switch(curCycle % 8)
    {
        case 1: //get pattern index
        {
            Point fetchTile = this->dma.GetPpuMemory().GetNameTables().CalcBgrFetchTile(fetchPixel);
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
            // calculate where the nametable was fetched
            Point nextFetchPixel = this->CalcNextFetchPixel(curCycle - 2, curLine);
            Point fetchTile = this->dma.GetPpuMemory().GetNameTables().CalcBgrFetchTile(nextFetchPixel);
            this->GetRegs().vRegs.nextAttributeIndex = this->dma.GetPpuMemory().GetNameTables().GetPaletteIndex(fetchTile.y, fetchTile.x);
            break;
        }

        case 5: //get lower pattern byte
        {
            int tableNum = this->GetRegs().name.backgroundPatternTable;
            PatternTables::BitTile& bitTile = this->dma.GetPatternTile(tableNum, this->GetRegs().vRegs.nextNametableIndex);

            Point nextFetchPixel = this->CalcNextFetchPixel(curCycle - 4, curLine);
            if(nextFetchPixel.y < 0)
            {
                // shouldn't be doing a background fetch on this cycle
                throw std::invalid_argument("invalid background fetch");
            }
            byte tileOffset = nextFetchPixel.y % PatternTables::TILE_HEIGHT;
            byte revBitPlane = bitTile.lsbPlane[tileOffset];
            this->GetRegs().vRegs.backgroundFetchTileLsb = BitUtil::FlipByte(revBitPlane);
            break;
        }

        case 7:
        {
            int tableNum = this->GetRegs().name.backgroundPatternTable;
            PatternTables::BitTile& bitTile = this->dma.GetPatternTile(tableNum, this->GetRegs().vRegs.nextNametableIndex);

            Point nextFetchPixel = this->CalcNextFetchPixel(curCycle - 6, curLine);
            if(nextFetchPixel.y < 0 )
            {
                // shouldn't be doing a background fetch on this cycle
                throw std::invalid_argument("invalid background fetch");
            }
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

        if(curCycle == START_VISIBLE_CYCLE) // clear previous secondary OAM
        {
            secondaryOam.ClearFetchData();
        }
        else if( curCycle == START_SPRITE_EVALUATION_CYCLE ) // load sprites on the next scanline into secondary OAM
        {
            for(int i = 0; i < OamPrimary::TotalNumOfSprites; ++i)
            {
                const OamPrimary::Sprite& primarySprite = primaryOam.GetSprite(i);

                // check if sprite is on next scanline
                byte spriteHeight = PatternTables::TILE_HEIGHT;
                if(this->GetRegs().name.spriteSize)
                {
                    spriteHeight = 2 * PatternTables::TILE_HEIGHT;
                }

                int spriteStart = primarySprite.posY;
                int spriteEnd = primarySprite.posY + spriteHeight;
                if( (nextScanline >= spriteStart) && (nextScanline < spriteEnd) )
                {
                    //sprite appears on next scanline
                    secondaryOam.AppendFetchedSprite(primarySprite, i);
                }
            }

            // set overflow register
            this->GetRegs().name.spriteOverflow = (secondaryOam.GetSpriteFetchNum() > 8);
        }
        else if(curCycle == START_SPRITE_TILE_FETCH_CYCLE) // update active sprite buffers
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
    dword xPos = curCycle - START_VISIBLE_CYCLE;
    byte bufferOffset = (xPos) % 8;
    bufferOffset += (this->GetRegs().GetActiveScrollX() % 8);
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
    bool regPowerCycle = this->GetRegs().PowerCycle();

    if(regPowerCycle == false)
    {
        return false;
    }

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
    std::unique_ptr<Ppu::BackgroundFetchInfo> fetchInfo = this->backgroundFetch(curCycle, curLine);
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

    // increment PPU cycles
    long long nextPpuCycle = this->dma.GetPpuMemory().GetTotalPpuCycles() + 1;
    this->dma.GetPpuMemory().SetTotalPpuCycles(nextPpuCycle);
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