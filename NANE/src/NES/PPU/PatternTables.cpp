#include "PatternTables.h"

const byte PatternTables::NUM_OF_TABLES;
const byte PatternTables::TABLE_WIDTH;
const byte PatternTables::TABLE_HEIGHT;
const byte PatternTables::TILE_WIDTH;
const byte PatternTables::TILE_HEIGHT;

PatternTables::PatternTables(std::shared_ptr<std::vector<byte>> chrRomVec)
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
                    byte lsbyte = curInputTile.lsbPlane[row];
                    byte msbyte = curInputTile.msbPlane[row];
                    for(int col = 0; col < TILE_WIDTH; ++col)
                    {
                        bit msbit = BitUtil::GetBits(msbyte, col);
                        bit lsbit = BitUtil::GetBits(lsbyte, col);

                        byte paletteValue = (msbit << 1) | lsbit;

                        int pixelY = row;
                        int pixelX = 7 - col; //NES draws from right to left but we will be drawing from left to right
                        curOutputTitle.Set(pixelY, pixelX, paletteValue);
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

std::unique_ptr<Matrix<rawColour>> PatternTables::GeneratePatternDisplay(const ColourPalettes& colourPalettes, byte disassemblePalette)
{
    int outputWidth = PatternTables::NUM_OF_TABLES * PatternTables::TILE_WIDTH * PatternTables::TABLE_WIDTH;
    int outputHeight = PatternTables::TILE_HEIGHT * PatternTables::TABLE_HEIGHT;
    std::unique_ptr<Matrix<rawColour>> outputPatterns = std::unique_ptr<Matrix<rawColour>>( new Matrix<rawColour>(outputWidth, outputHeight) );

    for(int tableNum = 0; tableNum < PatternTables::NUM_OF_TABLES; ++tableNum)
    {
        for(int y = 0; y < PatternTables::TABLE_HEIGHT; ++y)
        {
            for(int x = 0; x < PatternTables::TABLE_WIDTH; ++x)
            {
                Matrix<byte> tilePattern = this->GetTile(tableNum, y, x);
                for(int row = 0; row < PatternTables::TILE_HEIGHT; ++row)
                {
                    for(int col = 0; col < PatternTables::TILE_WIDTH; ++col)
                    {
                        byte patternVal = tilePattern.Get(row, col);

                        rawColour pixelVal = colourPalettes.PatternValueToColour(disassemblePalette, patternVal);

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