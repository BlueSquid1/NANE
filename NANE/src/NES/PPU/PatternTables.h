#ifndef PATTERN_TABLES
#define PATTERN_TABLES

#include <vector>
#include "Matrix.h"
#include "NES/Memory/BitUtil.h"

class PatternTables
{
    public:
    static const byte NUM_OF_TABLES = 2;
    static const byte TABLE_WIDTH = 16; //tiles
    static const byte TABLE_HEIGHT = 16; //titles
    static const byte TILE_WIDTH = 8; //pixels
    static const byte TILE_HEIGHT = 8; //pixels

    
    private:
    /**
     * these structs are used to dynamically case the chr rom to a BitPatternTables without having to write a decoder (pretty cool right?).
     */
    struct BitTile
    {
        byte LsbPlane[TILE_HEIGHT];
        byte MsbPlane[TILE_HEIGHT];
    };
    struct BitPatternTable
    {
        BitTile titles[TABLE_HEIGHT][TABLE_WIDTH];
    };
    struct BitPatternTables
    {
        BitPatternTable tables[NUM_OF_TABLES];
    };

    //the user friendly interface
    class PatternTable
    {
        public:
        Matrix<Matrix<byte>> tiles;

        PatternTable()
        : tiles(TABLE_HEIGHT, TABLE_WIDTH, Matrix<byte>(TILE_HEIGHT, TILE_WIDTH))
        {
        }
    };
    std::vector<PatternTable> patternTableData;

    public:
    PatternTables(std::unique_ptr<std::vector<byte>>& chrRomVec);

    Matrix<byte>& GetTile(int tableNum, int y, int x);
};

#endif