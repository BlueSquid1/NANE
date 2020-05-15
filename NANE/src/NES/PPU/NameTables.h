#ifndef NAME_TABLES
#define NAME_TABLES

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/MemoryRepeaterArray.h"
#include "NES/Memory/Matrix.h"
#include "NES/Cartridge/CartridgeMapping/ICartridge.h"

#include <memory>

/**
 * Manages the nametables
 * 
 * https://wiki.nesdev.com/w/index.php/PPU_nametables
 */

typedef byte paletteIndex;
typedef byte patternIndex;

struct QuadAreaPalette
{
    union
    {
        paletteIndex value;
        struct
        {
            paletteIndex topleftArea : 2;
            paletteIndex topRightArea : 2;
            paletteIndex bottomleftArea : 2;
            paletteIndex bottomrightArea : 2;
        };
    };
};

struct NameTable
{
    patternIndex charactorCell[30][32];
    QuadAreaPalette attributeTable[8][8];
};

struct NameTableStruct
{
    NameTable table[4];
};

class NameTables : public MemoryRepeaterArray
{
    private:
    struct LocalTablePos
    {
        byte tableIndex;
        dword localX;
        dword localY;
    };

    static const int rawLen = 4096;
    //anonymous union
    union
    {
        NameTableStruct name;
        byte raw[rawLen];
    };

    INes::MirrorType mirroringType = INes::not_set;

    dword Redirect(dword address) const;
    LocalTablePos LocalFromGlobalPos(dword globalY, dword globalX);

    public:
    NameTables();

    byte Read(dword address) override;
    void Write(dword address, byte value) override;
    byte Seek(dword address) const override;

    patternIndex GetPatternIndex(dword globalY, dword globalX);
    paletteIndex GetPaletteIndex(dword globalY, dword globalX);

    //dissassemble commands
    std::unique_ptr<Matrix<patternIndex>> GenerateFirstNameTable();
    //getters/setters
    void SetMirrorType( INes::MirrorType mirroringType );
};

#endif