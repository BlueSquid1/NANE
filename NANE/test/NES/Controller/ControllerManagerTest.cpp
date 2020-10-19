#include <catch2/catch.hpp>

#include "NES/Controller/ControllerManager.h"

TEST_CASE("strobe signal is set to all controllers") {
    ControllerManager mgr;
    //before strobe signal all controllers return their default value (65).
    for(dword address = mgr.startAddress; address < mgr.endAddress; ++address)
    {
        for(int i = 0; i < 8; ++i)
        {
            REQUIRE(mgr.Read(address) == 65);
        }
    }

    mgr.Write(0x4016, 0);
    //after a strobe to a controller ALL controllers set strobe and return their button state.
    for(dword address = mgr.startAddress; address < mgr.endAddress; ++address)
    {
        for(int i = 0; i < 8; ++i)
        {
            //64 means the button isn't being pressed
            REQUIRE(mgr.Read(address) == 64);
        }
    }
}