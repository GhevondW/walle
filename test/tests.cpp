#include <catch2/catch_test_macros.hpp>

#include <walle/walle.hpp>

TEST_CASE("plus_10 are computed", "[walle::plus_10]") {
    REQUIRE(walle::plus_10(0) == 10);
    REQUIRE(walle::plus_10(1) == 11);
    REQUIRE(walle::plus_10(2) == 12);
    REQUIRE(walle::plus_10(3) == 13);
    REQUIRE(walle::plus_10(10) == 20);
}

TEST_CASE("hi are computed", "[walle::hi]") {
    REQUIRE(walle::hi("Ghevond") == "hi : Ghevond");
    REQUIRE(walle::hi("Walle") == "hi : Walle");
}
