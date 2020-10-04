#ifndef PATTERN_TABLES
#define PATTERN_TABLES

#include <vector>
#include <memory>

#include "NES/Memory/Matrix.h"
#include "NES/Memory/BitUtil.h"
#include "NesColour.h"
#include "ColourPalettes.h"

typedef byte patternIndex;

class PatternTables
{
    public:
    static const byte NUM_OF_TABLES = 2;
    static const byte TABLE_WIDTH = 16; //tiles
    static const byte TABLE_HEIGHT = 16; //titles
    static const byte TILE_WIDTH = 8; //pixels
    static const byte TILE_HEIGHT = 8; //pixels

    /**
     * these structs are used to dynamically case the chr rom to a BitPatternTables without having to write a decoder (pretty cool right?).
     */
    struct BitTile
    {
        byte lsbPlane[TILE_HEIGHT];
        byte msbPlane[TILE_HEIGHT];
    };
    struct BitPatternTable
    {
        union
        {
            BitTile titles[TABLE_HEIGHT][TABLE_WIDTH];
            BitTile raw[TABLE_HEIGHT * TABLE_WIDTH];
        };
    };
    struct BitPatternTables
    {
        BitPatternTable tables[NUM_OF_TABLES];
    };

    private:
    /**
     * @brief pattern table structure
     */
    struct PatternTable
    {
        Matrix<Matrix<byte>> tiles;

        PatternTable()
        : tiles(TABLE_HEIGHT, TABLE_WIDTH, Matrix<byte>(TILE_HEIGHT, TILE_WIDTH))
        {
        }
    };
    std::vector<PatternTable> patternTableData;

    public:
    PatternTables(std::shared_ptr<std::vector<byte>> chrRomVec);

    Matrix<byte>& GetTile(int tableNum, int y, int x);

    // dissassembly method(s)
    std::unique_ptr<Matrix<rawColour>> GeneratePatternDisplay(const ColourPalettes& colourPalettes, byte disassemblePalette);
};

#endif