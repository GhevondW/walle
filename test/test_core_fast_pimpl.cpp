#include <catch2/catch_test_macros.hpp>

#include <string>
#include <walle/core/fast_pimpl.hpp>

#include <common/sample_pimpl.hpp>

using namespace std::chrono_literals;

TEST_CASE("walle::core::fast_pimpl") {
    test::common::sample_pimpl pimpl("foo");
    REQUIRE(std::string(pimpl.foo()) == "foo");
}

// TODO : add tests
