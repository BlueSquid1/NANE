#include <catch2/catch.hpp>
#include "../../../src/NES/ROM/INesParser.h"
//#include "/Users/clintonpage/Desktop/mygit/NANE/NANE/src/NES/ROM/INesParser.h"

TEST_CASE("parse nestest.nes rom") {
  const std::string nestestPath = "../../resources/nestest.nes";
  std::unique_ptr<INes> ines = INesParser::ParseINes(nestestPath);
  REQUIRE(ines.get() != NULL);
}
