#ifndef NAME_TABLES
#define NAME_TABLES

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/MemoryRepeaterArray.h"
#include "NES/Cartridge/CartridgeMapping/ICartridge.h"

/**
 * Manages the nametables
 * 
 * https://wiki.nesdev.com/w/index.php/PPU_nametables
 */

struct QuadTilePalette
{
    union
    {
        byte value;
        struct
        {
            byte topleftArea : 2;
            byte topRightArea : 2;
            byte bottomleftArea : 2;
            byte bottomrightArea : 2;
        };
    };
};

struct NameTable
{
    byte charactorCell[30][32];
    QuadTilePalette attributeTable[8][8];
};

struct NameTableStruct
{
    NameTable table[4];
};

class NameTables : public MemoryRepeaterArray
{
    private:
    static const int rawLen = 4096;
    //anonymous union
    union
    {
        NameTableStruct name;
        byte raw[rawLen];
    };

    INes::MirrorType mirroringType = INes::not_set;

    dword Redirect(dword address) const;

    public:
    NameTables();

    byte Read(dword address) override;
    void Write(dword address, byte value) override;
    byte Seek(dword address) const override;
    void SetMirrorType( INes::MirrorType mirroringType );
};

#endif