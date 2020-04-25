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

    NesController::NesInputs button = NesController::NesInputs::A;
    joy.SetKey(button, true);
    button = NesController::NesInputs::SELECT;
    joy.SetKey(button, true);
    joy.Write(0x4016, 0);
    REQUIRE(joy.Read(0x4016) == true);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == true);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
    REQUIRE(joy.Read(0x4016) == false);
}