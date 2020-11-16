#include "NameTables.h"

#include <math.h>

const int NameTables::rawLen;

const int NameTables::nametablesWidth;
const int NameTables::nametablesHeight;

NameTables::NameTables()
 : MemoryRepeaterArray(0x2000, 0x3EFF, this->raw, this->rawLen)
{
    memset(this->raw, 0, this->rawLen);
}

dword NameTables::Redirect(dword address) const
{
    dword redirrectedAddress = address;
    dword offset = this->LowerOffset(address);

    switch(this->mirroringType)
    {
        case INes::vertical:
        {
            if(offset >= 0x0800)
            {
                redirrectedAddress -= 0x0800;
            }
            break;
        }
        case INes::horizontal:
        {
            if((offset >= 0x0400 && offset < 0x0800) || offset >= 0x0C00)
            {
                redirrectedAddress -= 0x0400;
            }
            break;
        }
        default:
        {
            std::cerr << "Haven't implemented the name tabling mapping" << std::endl;
        }
    }
    return redirrectedAddress;
}

NameTables::LocalTablePos NameTables::LocalFromGlobalPos(dword globalY, dword globalX) const
{
    NameTables::LocalTablePos localPos;
    if((globalY >= 0 && globalY < 30) && (globalX >= 0 && globalX < 32))
    {
        localPos.tableIndex = 0;
        localPos.localX = globalX;
        localPos.localY = globalY;
    }
    else if((globalY >= 0 && globalY < 30) && (globalX >= 32 && globalX < 64))
    {
        localPos.tableIndex = 1;
        localPos.localX = globalX - 32;
        localPos.localY = globalY;
    }
    else if((globalY >= 30 && globalY < 60) && (globalX >= 0 && globalX < 32))
    {
        localPos.tableIndex = 2;
        localPos.localX = globalX;
        localPos.localY = globalY - 30;
    }
    else if((globalY >= 30 && globalY < 60) && (globalX >= 32 && globalX < 64))
    {
        localPos.tableIndex = 3;
        localPos.localX = globalX - 32;
        localPos.localY = globalY - 30;
    }
    else
    {
        throw std::invalid_argument("y and x are outside boundary for the nametables");
    }

    if(localPos.localY >= 30)
    {
        throw std::invalid_argument("Read into the attribute section of the name table");
    }
    return localPos;
}


byte NameTables::Read(dword address)
{
    if(this->mirroringType == INes::not_set)
    {
        std::cerr << "warning, mirroring type for PPU Name tables hasn't been set. Expect corrupt graphics." << std::endl;
    }
    return this->Seek(address);
}

void NameTables::Write(dword address, byte value)
{
    if(this->mirroringType == INes::not_set)
    {
        std::cerr << "warning, mirroring type for PPU Name tables hasn't been set. Expect corrupt graphics." << std::endl;
    }

    dword redirrectAddress = this->Redirect(address);
    return MemoryRepeaterArray::Write(redirrectAddress, value);
}

byte NameTables::Seek(dword address) const
{
    if(this->mirroringType == INes::not_set)
    {
        std::cerr << "warning, mirroring type for PPU Name tables hasn't been set. Expect corrupt graphics." << std::endl;
    }
    dword redirrectAddress = this->Redirect(address);
    return MemoryRepeaterArray::Seek(redirrectAddress);
}

patternIndex NameTables::GetPatternIndex(dword globalY, dword globalX)
{
    NameTables::LocalTablePos pos = this->LocalFromGlobalPos(globalY, globalX);
    dword offset = pos.tableIndex * 1024 + pos.localY * 32 + pos.localX;
    patternIndex pattern = this->Seek(this->startAddress + offset);
    return pattern;
}

paletteIndex NameTables::GetPaletteIndex(dword globalY, dword globalX) const
{
    NameTables::LocalTablePos pos = this->LocalFromGlobalPos(globalY, globalX);
    byte paletteY = pos.localY >> 2;
    byte paletteX = pos.localX >> 2;
    dword offset = (30 * 32) + pos.tableIndex * 1024 + paletteY * 8 + paletteX;
    byte area = this->Seek(this->startAddress + offset);

    paletteIndex paletteBit = 0;

    byte fineY = pos.localY % 4;
    byte fineX = pos.localX % 4;
    if( fineY >= 2 )
    {
        //bottom left or right
        paletteBit += 4;
    }

    if(fineX >= 2)
    {
        //top right or bottom right
        paletteBit += 2;
    }
    
    paletteIndex palette = BitUtil::GetBits(area, paletteBit, paletteBit + 1);
    return palette;
}

Point NameTables::CalcBgrFetchTile(const Point& fetchPixel) const
{
    Point fetchTile;
    if(fetchPixel.x < 0)
    {
        // invalid fetch pixel
        fetchTile.x = -1;
        fetchTile.y = -1;
        return fetchTile;
    }
    // convert from pixels to tiles (8 pixel per tile so bitshift 3 right)
    fetchTile.x = fetchPixel.x >> 3;
    fetchTile.y = fetchPixel.y >> 3;
    return fetchTile;
}

std::unique_ptr<Matrix<patternIndex>> NameTables::GenerateFirstNameTable()
{
    std::unique_ptr<Matrix<patternIndex>> patterns = std::unique_ptr<Matrix<patternIndex>>( new Matrix<patternIndex>(32, 32) );

    for(int y = 0; y < patterns->GetHeight(); ++y)
    {
        for(int x = 0; x < patterns->GetWidth(); ++x)
        {
            patternIndex value = this->GetPatternIndex(y, x);
            patterns->Set(y, x, value);
        }
    }
    return patterns;
}

void NameTables::SetMirrorType( INes::MirrorType mirroringType )
{
    this->mirroringType = mirroringType;
}