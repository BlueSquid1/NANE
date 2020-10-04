#include <catch2/catch.hpp>

#include "NES/Controller/NesController.h"

TEST_CASE("default state") {
    NesController joy(0x4016);
    REQUIRE(joy.Contains(0x4016) == true);
    for(int i = 0; i < 8; ++i)
    {
        REQUIRE(joy.Read(0x4016) == true);
    }
}

TEST_CASE("user inputs") {
    NesController joy(0x4016);
    joy.Write(0x4016, 0);
    for(int i = 0; i < 8; ++i)
    {
        REQUIRE(joy.Read(0x4016) == false);
    }
    for(int i = 0; i < 8; ++i)
    {
        REQUIRE(joy.Read(0x4016) == true);
    }

    NesController::NesInputs aButton = NesController::NesInputs::A;
    joy.SetKey(aButton, true);
    NesController::NesInputs selectButton = NesController::NesInputs::SELECT;
    joy.SetKey(selectButton, true);
    joy.Write(0x4016, 0);
    // read from controller
    REQUIRE(joy.Read(0x4016) == true);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == true);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);

    //after controller further reads return true
    REQUIRE(joy.Read(0x4016) == true);
    REQUIRE(joy.Read(0x4016) == true);

    //release the A key and checkout output
    joy.SetKey(aButton, false);
    joy.Write(0x4016, 0);
    // read from controller
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == true);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
}