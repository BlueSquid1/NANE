#include "Ppu.h"

Ppu::Ppu(Dma& dma)
: dma(dma), framebuffer(256, 240)
{
}

rawColour Ppu::calc_background_pixel()
{
    //get pattern value
    bit patBit0 = BitUtil::GetBits(this->GetRegs().bgr.shift.patternPlane1.val, 0);
    bit patBit1 = BitUtil::GetBits(this->GetRegs().bgr.shift.patternPlane1.val, 0);
    byte patternValue = (patBit1 << 1) | patBit0;

    //get palette index
    bit palBit0 = BitUtil::GetBits(this->GetRegs().bgr.shift.paletteAttribute1, 0);
    bit palBit1 = BitUtil::GetBits(this->GetRegs().bgr.shift.paletteAttribute2, 0);
    byte attributeIndex = (palBit1 << 1) | palBit0;

    //shift background specific registers
    this->GetRegs().bgr.shift.patternPlane1.val <<= 1;
    this->GetRegs().bgr.shift.patternPlane2.val <<= 1;
    this->GetRegs().bgr.shift.paletteAttribute1 <<= 1;
    this->GetRegs().bgr.shift.paletteAttribute2 <<= 1;

    //this->dma.GetPpuMemory().GetPalettes()->name.backgroundPalettes
    byte nesColourIndex = this->dma.GetPpuMemory().GetPalettes().GetColourPalettes().backgroundPalettes[attributeIndex][patternValue];
    return NesColour::GetRawColour(nesColourIndex);
}

rawColour Ppu::calc_sprite_pixel()
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
    this->GetRegs().bgr.ppuAddressLatch = false;
    this->GetRegs().bgr.curPpuAddress.val = 0;
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
    if(curLine < 239)
    {
        rawColour bPixel = this->calc_background_pixel();
        rawColour sPixel = this->calc_sprite_pixel();

        //pick between bPixel and sPixel based on some state
        rawColour pixel = bPixel;
        //if(this->memory->GetPpuRegisters()->name.PPUMASK_S.showBackground || this->memory->GetPpuRegisters()->name.PPUMASK_S.showSprites)
        {
            this->background_fetch();
            this->sprite_fetch();
        }

        if(curLine != -1) //not the preview scanline
        {
            //update right pixel with: 
            //this->framebuffer
        }
    } 
    else if( curLine == 241 )
    {
        if(curCycle == 1)
        {
            //set VBlank flag to 1 at second cycle of scanline 241
        }
    }
    else if( curLine == 260 )
    {
        //clear the VBlank flag
    }

    //update counters
    ++curCycle;

    if(curCycle > 340)
    {
        //next scanline
        curCycle = 0;
        ++curLine;

        if(curLine > 260)
        {
            //last scanline
            curLine = -1;
            ++this->frameCountNum;
        }
    }

    this->dma.GetPpuMemory().SetScanLineNum(curLine);
    this->dma.GetPpuMemory().SetScanCycleNum(curCycle);

    return true;
}

//https://wiki.nesdev.com/w/index.php/PPU_rendering
void Ppu::background_fetch()
{
    int curCycle = this->dma.GetPpuMemory().GetScanCycleNum();
    if(curCycle == 0)
    {
        //NOP for 1 cycle
    } 
    else if((curCycle >= 1 && curCycle < 257)
            || (curCycle >= 321 && curCycle < 337 ))
    {
        switch(curCycle % 8)
        {
            case 2:
            //fetch Nametable byte
            this->GetRegs().bgr.ntByte = this->GetRegs().name.PPUCTRL;


            // this->memory->GetPpuRegisters()->bgr.shift.patternPlane1.lower = this->memory->GetPpuRegisters()->bgr.tileLo;
            // this->memory->GetPpuRegisters()->bgr.shift.patternPlane2.lower = this->memory->GetPpuRegisters()->bgr.tileHi;

            // this->memory->GetPpuRegisters()->bgr.shift.paletteAttribute1 = BitUtil::GetBits(this->memory->GetPpuRegisters()->bgr.atByte, 0); //first bit
            // this->memory->GetPpuRegisters()->bgr.shift.paletteAttribute2 = BitUtil::GetBits(this->memory->GetPpuRegisters()->bgr.atByte, 1); //second bit
            break;
            
            case 4:
            //fetch Attribute Table byte
            this->GetRegs().bgr.atByte = this->dma.GetPpuMemory().Seek( 0x23C0 );
            break;
        }
    }
    else if(curCycle >= 257 && curCycle < 321)
    {
        //sprite fetch
    }
    //(256 cycles)
    //tile latch = Fetch a nametable entry from $2000-$2FBF.
    //tile latch = Fetch the corresponding attribute table entry from $23C0-$2FFF and increment the current VRAM address within the same row.
    //tile latch = Fetch the low-order byte of an 8x1 pixel sliver of pattern table from $0000-$0FF7 or $1000-$1FF7.
    //tile latch = Fetch the high-order byte of this sliver from an address 8 bytes higher.
    //update shift registers every 8th step from tile latch.
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