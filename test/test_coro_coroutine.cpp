#include <catch2/catch_test_macros.hpp>

#include <walle/coro/coroutine.hpp>

#include <iostream>

TEST_CASE("coro", "[walle::core::semaphore]") {
    int value = 1;

    auto coro = walle::coro::coroutine::create(
        [&value](auto ctx) {
            REQUIRE(value == 1);
            ++value;
            ctx.suspend();
            REQUIRE(value == 3);
            ++value;
            ctx.suspend();
            REQUIRE(value == 5);
            ++value;
        },
        1000000);

    coro.resume();
    REQUIRE(value == 2);
    ++value;
    coro.resume();
    REQUIRE(value == 4);
    ++value;
    coro.resume();
    REQUIRE(value == 6);
    ++value;
}
