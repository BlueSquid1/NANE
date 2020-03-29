#include "PatternTables.h"

const byte PatternTables::NUM_OF_TABLES;
const byte PatternTables::TABLE_WIDTH;
const byte PatternTables::TABLE_HEIGHT;
const byte PatternTables::TILE_WIDTH;
const byte PatternTables::TILE_HEIGHT;

PatternTables::PatternTables(std::unique_ptr<std::vector<byte>>& chrRomVec)
: patternTableData(NUM_OF_TABLES)
{
    byte * chrRawData = chrRomVec->data();
    //do a dynmatic type conversion of the binary
    BitPatternTables& patternTables = (BitPatternTables&) *chrRawData;

    //loop through each title and update patternTableData
    for(int tableNum = 0; tableNum < NUM_OF_TABLES; ++tableNum)
    {
        BitPatternTable curInputTable = patternTables.tables[tableNum];
        PatternTable& curOutputTable = patternTableData.at(tableNum);
        for(int y = 0; y < TABLE_HEIGHT; ++y)
        {
            for(int x = 0; x < TABLE_WIDTH; ++x)
            {
                BitTile curInputTile = curInputTable.titles[y][x];
                Matrix<byte>& curOutputTitle = curOutputTable.tiles.Get(y, x);
                for(int row = 0; row < TILE_HEIGHT; ++row)
                {
                    byte lsbyte = curInputTile.LsbPlane[row];
                    byte msbyte = curInputTile.MsbPlane[row];
                    for(int col = 0; col < TILE_WIDTH; ++col)
                    {
                        bit msbit = BitUtil::GetBits(msbyte, col);
                        bit lsbit = BitUtil::GetBits(lsbyte, col);

                        byte paletteValue = (msbit << 1) | lsbit;
                        curOutputTitle.Set(row, col, paletteValue);
                    }
                }
            }
        }
    }
}

Matrix<byte>& PatternTables::GetTile(int tableNum, int y, int x)
{
    return this->patternTableData.at(tableNum).tiles.Get(y, x);
}