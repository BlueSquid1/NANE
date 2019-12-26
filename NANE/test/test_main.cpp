#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("test1", "[test1]") {
  REQUIRE(1 == 2);
}

TEST_CASE("test3", "[test3]") {
  REQUIRE(1 == 1);
}