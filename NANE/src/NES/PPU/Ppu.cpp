#include "Ppu.h"

Ppu::Ppu(std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters)
: ppuMemory(ppuRegisters), framebuffer(256, 240)
{

}

rawColour Ppu::calc_background_pixel()
{
    //get pattern value
    bit patBit0 = BitUtil::GetBits(this->ppuMemory.GetPpuRegisters()->bgr.shift.patternPlane1.val, 0);
    bit patBit1 = BitUtil::GetBits(this->ppuMemory.GetPpuRegisters()->bgr.shift.patternPlane1.val, 0);
    byte patternValue = (patBit1 << 1) | patBit0;

    //get palette index
    bit palBit0 = BitUtil::GetBits(this->ppuMemory.GetPpuRegisters()->bgr.shift.paletteAttribute1, 0);
    bit palBit1 = BitUtil::GetBits(this->ppuMemory.GetPpuRegisters()->bgr.shift.paletteAttribute2, 0);
    byte attributeIndex = (palBit1 << 1) | palBit0;

    //shift background specific registers
    this->ppuMemory.GetPpuRegisters()->bgr.shift.patternPlane1.val <<= 1;
    this->ppuMemory.GetPpuRegisters()->bgr.shift.patternPlane2.val <<= 1;
    this->ppuMemory.GetPpuRegisters()->bgr.shift.paletteAttribute1 <<= 1;
    this->ppuMemory.GetPpuRegisters()->bgr.shift.paletteAttribute2 <<= 1;

    //this->ppuMemory.GetPalettes()->name.backgroundPalettes
    byte nesColourIndex = this->ppuMemory.GetPalettes()->GetColourPalettes().backgroundPalettes[attributeIndex][patternValue];
    return NesColour::GetRawColour(nesColourIndex);
}

rawColour Ppu::calc_sprite_pixel()
{
    
}

bool Ppu::PowerCycle()
{
    //reset registers
    this->ppuMemory.GetPpuRegisters()->name.PPUCTRL = 0;
    this->ppuMemory.GetPpuRegisters()->name.PPUMASK = 0;
    this->ppuMemory.GetPpuRegisters()->name.PPUSTATUS = 0;
    this->ppuMemory.GetPpuRegisters()->name.OAMADDR = 0;
    this->ppuMemory.GetPpuRegisters()->name.PPUSCROLL = 0;
    this->ppuMemory.GetPpuRegisters()->name.PPUADDR = 0;
    this->ppuMemory.GetPpuRegisters()->name.PPUDATA = 0;
    
    //internal registers
    this->ppuMemory.GetPpuRegisters()->name.ppuRegLatch = false;
    this->ppuMemory.GetPpuRegisters()->name.V.value = 0;
    this->ppuMemory.GetPpuRegisters()->name.T.value = 0;
    this->ppuMemory.SetScanLineNum(-1);
    this->ppuMemory.SetScanCycleNum(0);
    this->frameCountNum = 0;
}

int Ppu::Step()
{
    int curLine = this->ppuMemory.GetScanLineNum();
    int curCycle = this->ppuMemory.GetScanCycleNum();
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

    this->ppuMemory.SetScanLineNum(curLine);
    this->ppuMemory.SetScanCycleNum(curCycle);
}

//https://wiki.nesdev.com/w/index.php/PPU_rendering
void Ppu::background_fetch()
{
    int curCycle = this->ppuMemory.GetScanCycleNum();
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
            this->ppuMemory.GetPpuRegisters()->bgr.ntByte = this->ppuMemory.GetPpuRegisters()->name.PPUCTRL;


            // this->memory->GetPpuRegisters()->bgr.shift.patternPlane1.lower = this->memory->GetPpuRegisters()->bgr.tileLo;
            // this->memory->GetPpuRegisters()->bgr.shift.patternPlane2.lower = this->memory->GetPpuRegisters()->bgr.tileHi;

            // this->memory->GetPpuRegisters()->bgr.shift.paletteAttribute1 = BitUtil::GetBits(this->memory->GetPpuRegisters()->bgr.atByte, 0); //first bit
            // this->memory->GetPpuRegisters()->bgr.shift.paletteAttribute2 = BitUtil::GetBits(this->memory->GetPpuRegisters()->bgr.atByte, 1); //second bit
            break;
            
            case 4:
            //fetch Attribute Table byte
            this->ppuMemory.GetPpuRegisters()->bgr.atByte = this->ppuMemory.Read( 0x23C0 );
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

bool Ppu::SetCartridge(std::shared_ptr<ICartridge> cartridge)
{
    this->ppuMemory.SetCartridge(cartridge);
    return true;
}

std::unique_ptr<Matrix<rawColour>> Ppu::GeneratePatternTables()
{
    int outputWidth = PatternTables::NUM_OF_TABLES * PatternTables::TILE_WIDTH * PatternTables::TABLE_WIDTH;
    int outputHeight = PatternTables::TILE_HEIGHT * PatternTables::TABLE_HEIGHT;
    std::unique_ptr<Matrix<rawColour>> outputPatterns = std::unique_ptr<Matrix<rawColour>>( new Matrix<rawColour>(outputWidth, outputHeight) );

    std::unique_ptr<PatternTables> patternTables = this->ppuMemory.GeneratePatternTablesFromRom();
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
                        byte paletteVal = tilePattern.Get(row, col);

                        //translate using a colour palette
                        //TODO
                        rawColour pixelVal;
                        switch(paletteVal)
                        {
                            case(0):
                            pixelVal.channels.red = 0x00;
                            pixelVal.channels.green = 0x00;
                            pixelVal.channels.blue = 0x00;
                            pixelVal.channels.alpha = 0xFF;
                            break;
                            case(1):
                            pixelVal.channels.red = 0xFF;
                            pixelVal.channels.green = 0xFF;
                            pixelVal.channels.blue = 0xFF;
                            pixelVal.channels.alpha = 0xFF;
                            break;
                            case(2):
                            pixelVal.channels.red = 0x00;
                            pixelVal.channels.green = 0xFF;
                            pixelVal.channels.blue = 0x00;
                            pixelVal.channels.alpha = 0xFF;
                            break;
                            case(3):
                            pixelVal.channels.red = 0x00;
                            pixelVal.channels.green = 0x00;
                            pixelVal.channels.blue = 0xFF;
                            pixelVal.channels.alpha = 0xFF;
                            break;
                        }
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

const Matrix<rawColour>& Ppu::GetFrameDisplay()
{
    return this->framebuffer;
}