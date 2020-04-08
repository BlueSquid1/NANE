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

Ppu::Point Ppu::CalcNextFetchTile()
{
    Ppu::Point nextTile;
    int curScanLine = this->dma.GetPpuMemory().GetScanLineNum();
    int curCycle = this->dma.GetPpuMemory().GetScanCycleNum();

    nextTile.x = (curCycle - START_VISIBLE_CYCLE) >> 3;
    if(curScanLine < 0)
    {
        //handle negative values
        nextTile.y = -1;
    }
    else
    {
        nextTile.y  = (curScanLine) >> 3;
    }

    nextTile.x += 2;

    if(curCycle >= START_NEXT_SCANLINE_FETCHING)
    {
        ++nextTile.y;
        nextTile.x -= (LAST_CYCLE >> 3);
    }

    return nextTile;
}

rawColour Ppu::calc_sprite_pixel()
{
    return {0x0};
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
    this->GetRegs().bgr.ppuAddressLatch = false;
    this->GetRegs().bgr.vramPpuAddress.val = 0;
    this->GetRegs().bgr.ppuDataReadBuffer = 0;
    this->dma.GetPpuMemory().SetScanLineNum(-1);
    this->dma.GetPpuMemory().SetScanCycleNum(0);
    this->frameCountNum = 0;

    return true;
}

int Ppu::Step()
{
    int curLine = this->dma.GetPpuMemory().GetScanLineNum();
    int curCycle = this->dma.GetPpuMemory().GetScanCycleNum();
    //std::cout << curLine << " " << curCycle << std::endl; TODO

    //special cases
    if(curLine == -1 && curCycle == 1)
    {
        //start of new frame so clear VBlank flag
        this->GetRegs().name.verticalBlank = false;
        ++this->frameCountNum;
    }
    else if( curLine == 241 && curCycle == 1 )
    {
        //end of frame, set VBlank flag
        this->GetRegs().name.verticalBlank = true;
        if(this->GetRegs().name.generateNmi == true)
        {
            this->dma.SetNmi(true);
        }
    }

    
    //cycles to fetch background
    //1. when fetching for the next scanline
    //2. for current scanline fetching
    if( ( (curLine >= PRE_SCANLINE && curLine < LAST_VISIBLE_SCANLINE)  && (curCycle >= START_NEXT_SCANLINE_FETCHING && curCycle <= LAST_NEXT_SCANLINE_FETCHING) )
        ||( (curLine >= START_VISIBLE_SCANLINE && curLine <= LAST_VISIBLE_SCANLINE) && (curCycle >= START_VISIBLE_CYCLE && curCycle <= LAST_VISIBLE_CYCLE - 8) ) )
    {   
        this->background_fetch();
        //this->sprite_fetch(); TODO
    }

    // visible scanlines
    if( (curLine >= START_VISIBLE_SCANLINE && curLine <= LAST_VISIBLE_SCANLINE) && (curCycle >= START_VISIBLE_CYCLE && curCycle <= LAST_VISIBLE_CYCLE) )
    {
        rawColour bPixel = this->calc_background_pixel();

        //pick between bPixel and sPixel based on some state
        rawColour pixelColour = bPixel;
        if(this->GetRegs().name.showBackground == true)
        {
            int pixelX = curCycle - START_VISIBLE_CYCLE;
            int pixelY = curLine - START_VISIBLE_SCANLINE;
            if(pixelColour.raw != 255)
            {
                int i = 0;
            }
            this->framebuffer.Set(pixelY, pixelX, pixelColour);
        }
    }

    //move to next pixel
    Ppu::Point nextPoint = this->NextPixel();
    this->dma.GetPpuMemory().SetScanLineNum(nextPoint.y);
    this->dma.GetPpuMemory().SetScanCycleNum(nextPoint.x);

    return true;
}

//https://wiki.nesdev.com/w/index.php/PPU_rendering
void Ppu::background_fetch()
{
    //called between 1-257 and 321-337
    int curLine = this->dma.GetPpuMemory().GetScanLineNum();
    int curCycle = this->dma.GetPpuMemory().GetScanCycleNum();
    switch(curCycle % 8)
    {
        case 1: //reload shift registers and get pattern index
        {
            //reload shift registers on 1(?), 9, 17, 25 ... 257
            this->GetRegs().bgr.lsbPatternPlane.upper = this->GetRegs().bgr.lsbNextTile;
            this->GetRegs().bgr.msbPatternPlane.upper = this->GetRegs().bgr.msbNextTile;

            Ppu::Point nameTableIndex = this->CalcNextFetchTile();
            //TODO scrolling
            this->GetRegs().bgr.nextNametableIndex = this->dma.GetPpuMemory().GetNameTables().GetPatternIndex(nameTableIndex.y, nameTableIndex.x);
            break;
        }
        case 3: //get pallette index
        {
            Ppu::Point nameTableIndex = this->CalcNextFetchTile();
            //TODO scrolling
            this->GetRegs().bgr.nextAttributeIndex = this->dma.GetPpuMemory().GetNameTables().GetPaletteIndex(nameTableIndex.y, nameTableIndex.x);
            break;
        }

        case 5:
        {
            int tableNum = 0;
            //TODO scrolling
            PatternTables::BitTile& bitTile = this->dma.GetPatternTile(tableNum, this->GetRegs().bgr.nextNametableIndex);
            byte fineY = curLine % PatternTables::TILE_HEIGHT;
            this->GetRegs().bgr.lsbNextTile = bitTile.LsbPlane[fineY];
            break;
        }

        case 7:
        {
            int tableNum = 0;
            //TODO scrolling
            PatternTables::BitTile& bitTile = this->dma.GetPatternTile(tableNum, this->GetRegs().bgr.nextNametableIndex);
            byte fineY = curLine % PatternTables::TILE_HEIGHT;
            this->GetRegs().bgr.msbNextTile = bitTile.MsbPlane[fineY];
            break;
        }
    }
}

rawColour Ppu::calc_background_pixel()
{
    //get pattern value
    byte fineX = this->GetRegs().bgr.scrollX.fineX;
    bit lsbBit = BitUtil::GetBits(this->GetRegs().bgr.lsbPatternPlane.lower, fineX);
    bit msbBit = BitUtil::GetBits(this->GetRegs().bgr.msbPatternPlane.lower, fineX);
    byte patternValue = (msbBit << 1) | lsbBit;

    //get palette index
    // bit palBit0 = BitUtil::GetBits(this->GetRegs().bgr.shift.paletteAttribute1, 0);
    // bit palBit1 = BitUtil::GetBits(this->GetRegs().bgr.shift.paletteAttribute2, 0);
    byte attributeIndex = 0;

    //shift background specific registers
    this->GetRegs().bgr.lsbPatternPlane.val <<= 1;
    this->GetRegs().bgr.msbPatternPlane.val <<= 1;
    // this->GetRegs().bgr.shift.paletteAttribute1 <<= 1;
    // this->GetRegs().bgr.shift.paletteAttribute2 <<= 1;

    //this->dma.GetPpuMemory().GetPalettes()->name.backgroundPalettes
    rawColour nesColourIndex = this->dma.GetPpuMemory().GetPalettes().PatternValueToColour(attributeIndex, patternValue);
    return nesColourIndex;
}

void Ppu::sprite_fetch()
{
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