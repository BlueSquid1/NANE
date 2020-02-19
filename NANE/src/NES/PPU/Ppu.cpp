#include "Ppu.h"

Ppu::Ppu(std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters)
{
    this->registers = ppuRegisters;
    this->apuRegisters = apuRegisters;
    this->palettes = std::unique_ptr<ColourPalettes>( new ColourPalettes(0x3F00, 0x3F1F) );
    this->memory = std::unique_ptr<PpuMemoryMap>( new PpuMemoryMap(this->palettes) );
    this->primOam = std::unique_ptr<std::vector<byte>>( new std::vector<byte>(256) );
    this->secOam = std::unique_ptr<std::vector<byte>>( new std::vector<byte>(32) );
    this->framebuffer = std::shared_ptr<std::vector<NesColour>>( new std::vector<NesColour>(256 * 4 * 240) );

}

NesColour Ppu::calc_background_pixel()
{
    //get pattern value
    bit patBit0 = BitUtil::GetBits(this->curBackPatternPlane1, 0);
    bit patBit1 = BitUtil::GetBits(this->curBackPatternPlane2, 0);
    byte patternValue = (patBit1 << 1) | patBit0;

    //get palette index
    bit palBit0 = BitUtil::GetBits(this->curBackPaletteAttribute1, 0);
    bit palBit1 = BitUtil::GetBits(this->curBackPaletteAttribute2, 0);
    byte attributeIndex = (palBit1 << 1) | palBit0;

    //shift background specific registers
    this->curBackPatternPlane1 <<= 1;
    this->curBackPatternPlane2 <<= 1;
    this->curBackPaletteAttribute1 <<= 1;
    this->curBackPaletteAttribute2 <<= 1;

    byte nesColourIndex = this->palettes->name.backgroundPalette[attributeIndex][patternValue];
    return NesColour(nesColourIndex);
}

NesColour Ppu::calc_sprite_pixel()
{
    
}

bool Ppu::PowerCycle()
{
    this->registers->name.PPUCTRL = 0;
    this->registers->name.PPUMASK = 0;
    this->registers->name.PPUSTATUS = 0;
    this->registers->name.OAMADDR = 0;
    this->registers->name.PPUSCROLL = 0;
    this->registers->name.PPUADDR = 0;
    this->registers->name.PPUDATA = 0;

    this->scanlineNum = -1;
    this->scanCycleNum = 0;
    this->frameCountNum = 0;
}

int Ppu::Step()
{
    if(scanlineNum < 239)
    {
        NesColour bPixel = this->calc_background_pixel();
        NesColour sPixel = this->calc_sprite_pixel();

        //pick between bPixel and sPixel based on some state
        NesColour pixel = bPixel;
        //if(this->registers->name.PPUMASK_S.showBackground || this->registers->name.PPUMASK_S.showSprites)
        {
            this->background_fetch();
            this->sprite_fetch();
        }

        if(scanlineNum != -1) //not the preview scanline
        {
            //update right pixel with: 
            //this->framebuffer
        }
    } 
    else if( scanlineNum == 241 )
    {
        if(this->scanCycleNum == 1)
        {
            //set VBlank flag to 1 at second cycle of scanline 241
        }
    }
    else if( scanlineNum == 260 )
    {
        //clear the VBlank flag
    }

    //update counters
    ++this->scanCycleNum;

    if(this->scanCycleNum > 340)
    {
        //next scanline
        this->scanCycleNum = 0;
        ++this->scanlineNum;

        if(this->scanlineNum > 260)
        {
            //last scanline
            this->scanlineNum = -1;
            ++this->frameCountNum;
        }
    }
}

//https://wiki.nesdev.com/w/index.php/PPU_rendering
void Ppu::background_fetch()
{
    if(this->scanCycleNum == 0)
    {
        //NOP for 1 cycle
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



/*
const Color PPU::palette [64] = {
  0x666666, 0x002A88, 0x1412A7, 0x3B00A4, 0x5C007E, 0x6E0040, 0x6C0600, 0x561D00,
  0x333500, 0x0B4800, 0x005200, 0x004F08, 0x00404D, 0x000000, 0x000000, 0x000000,
  0xADADAD, 0x155FD9, 0x4240FF, 0x7527FE, 0xA01ACC, 0xB71E7B, 0xB53120, 0x994E00,
  0x6B6D00, 0x388700, 0x0C9300, 0x008F32, 0x007C8D, 0x000000, 0x000000, 0x000000,
  0xFFFEFF, 0x64B0FF, 0x9290FF, 0xC676FF, 0xF36AFF, 0xFE6ECC, 0xFE8170, 0xEA9E22,
  0xBCBE00, 0x88D800, 0x5CE430, 0x45E082, 0x48CDDE, 0x4F4F4F, 0x000000, 0x000000,
  0xFFFEFF, 0xC0DFFF, 0xD3D2FF, 0xE8C8FF, 0xFBC2FF, 0xFEC4EA, 0xFECCC5, 0xF7D8A5,
  0xE4E594, 0xCFEF96, 0xBDF4AB, 0xB3F3CC, 0xB5EBF2, 0xB8B8B8, 0x000000, 0x000000,
};
*/

std::shared_ptr<std::vector<NesColour>> Ppu::GetFrameDisplay() const
{

}