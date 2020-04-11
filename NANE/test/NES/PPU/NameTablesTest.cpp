#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/PPU/NameTables.h"
#include "NES/Cartridge/CartridgeMapping/CartridgeMapping0.h"

/**
 * test if NameTableStruct is a Plain-Old Data otherwise compiler might not place struct in the same order defined in the struct
 */
TEST_CASE("make sure NameTableStruct is Plain-Old Data otherwise") {
    REQUIRE(std::is_pod<NameTableStruct>::value == true);
}

/**
 * test to make sure nameTables are cleared after constructor
 */
TEST_CASE("nameTables cleared by default") {
    NameTables nameTables;
    nameTables.SetMirrorType(INes::vertical);

    REQUIRE(nameTables.dataLen > 0);
    for(int i = 0; i < nameTables.dataLen; ++i)
    {
        REQUIRE(nameTables.Read(nameTables.startAddress + i) == 0);
    }
}

/**
 * test to make sure NameTables mirroring works as expected
 */
TEST_CASE("NameTables Vertical mirroring") {
    NameTables verticalNameTables;
    verticalNameTables.SetMirrorType(INes::vertical);
    
    verticalNameTables.Write(0x2058, 129);
    REQUIRE(verticalNameTables.Read(0x2858) == 129);

    verticalNameTables.Write(0x2800, 35);
    REQUIRE(verticalNameTables.Read(0x2000) == 35);

    verticalNameTables.Write(0x2FFF, 43);
    REQUIRE(verticalNameTables.Read(0x27FF) == 43);
}

/**
 * test to make sure NameTables mirroring works as expected
 */
TEST_CASE("NameTables horizontal mirroring") {
    std::unique_ptr<ICartridge> nesRom = std::unique_ptr<CartridgeMapping0>( new CartridgeMapping0());
    NameTables horizontalNameTables;
    horizontalNameTables.SetMirrorType(INes::horizontal);
    
    horizontalNameTables.Write(0x2058, 129);
    REQUIRE(horizontalNameTables.Read(0x2458) == 129);

    horizontalNameTables.Write(0x2400, 35);
    REQUIRE(horizontalNameTables.Read(0x2000) == 35);

    horizontalNameTables.Write(0x27FF, 43);
    REQUIRE(horizontalNameTables.Read(0x23FF) == 43);

    horizontalNameTables.Write(0x2C00, 98);
    REQUIRE(horizontalNameTables.Read(0x2800) == 98);

    horizontalNameTables.Write(0x2FFF, 111);
    REQUIRE(horizontalNameTables.Read(0x2BFF) == 111);
}

/**
 * test to make sure NameTables mirroring works as expected
 */
TEST_CASE("NameTables memory repeating test") 
{
    NameTables verticalNameTables;
    verticalNameTables.SetMirrorType(INes::vertical);
    
    verticalNameTables.Write(0x3000, 129);
    REQUIRE(verticalNameTables.Read(0x2000) == 129);

    verticalNameTables.Write(0x3FFF, 73);
    REQUIRE(verticalNameTables.Read(0x2FFF) == 73);
}

TEST_CASE("Test getting palette colours")
{
    //NameTables verticalNameTables;
    //verticalNameTables.GetPaletteIndex();
}