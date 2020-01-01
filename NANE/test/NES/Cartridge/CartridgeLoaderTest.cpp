#include <catch2/catch.hpp>

#include "NesTestdata.h"
#include "NES/Util/BitUtil.h"
#include "NES/Cartridge/CartridgeLoader.h"

/**
 * tests if the nestest.nes rom can be parsed properly.
 */
TEST_CASE("parse nestest.nes rom") {
  const std::string nestestPath = "NANE/test/resources/nestest.nes";

  CartridgeLoader inesParser;
  std::unique_ptr<INes> ines = inesParser.ParseINes(nestestPath);
  REQUIRE(ines.get() != NULL);
  REQUIRE(ines->GetPrgRomLen() == 1 * 16384);
  REQUIRE(ines->GetChrRomLen() == 1 * 8192);
  REQUIRE(ines->GetMirroringType() == INes::horizontal);
  REQUIRE(ines->GetBatteryPresent() == false);
  REQUIRE(ines->GetTrainerPresent() == false);
  REQUIRE(ines->GetMapperNumber() == 0);

  std::unique_ptr<std::vector<byte>> prgRomVec = BitUtil::ArrayToVec(nesTestPrgRom, nesTestPrgRomSize);
  REQUIRE( BitUtil::VecEqual(ines->GetPrgRomData().get(),prgRomVec.get()) == true );

  std::unique_ptr<std::vector<byte>> chrRomVec = BitUtil::ArrayToVec(nesTestChrRom, nesTestChrRomSize);
  REQUIRE( BitUtil::VecEqual(ines->GetChrRomData().get(),chrRomVec.get()) == true );

}



