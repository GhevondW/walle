#include <catch2/catch_test_macros.hpp>

#include <walle/sync/semaphore.hpp>

#include <atomic>
#include <chrono>
#include <deque>
#include <random>
#include <string>
#include <thread>

using namespace std::chrono_literals;


TEST_CASE("walle::core::semaphore non blocking", "[walle::core::semaphore]") {
    walle::sync::semaphore semaphore{2};

    semaphore.acquire();  // -1
    semaphore.release();  // +1

    semaphore.acquire();  // -1
    semaphore.acquire();  // -1
    semaphore.release();  // +1
    semaphore.release();  // +1
}

TEST_CASE("walle::core::semaphore blocking", "[walle::core::semaphore]") {
    walle::sync::semaphore semaphore{0};

    bool touched = false;

    std::thread touch([&] {
        semaphore.acquire();
        touched = true;
    });

    std::this_thread::sleep_for(250ms);

    REQUIRE_FALSE(touched);

    semaphore.release();
    touch.join();

    REQUIRE(touched);
}

TEST_CASE("walle::core::semaphore ping pong", "[walle::core::semaphore]") {
    walle::sync::semaphore my{1};
    walle::sync::semaphore that{0};

    int step = 0;

    std::thread opponent([&] {
        that.acquire();
        REQUIRE(step == 1);
        step = 0;
        my.release();
    });

    my.acquire();
    REQUIRE(step == 0);
    step = 1;
    that.release();

    my.acquire();
    REQUIRE(step == 0);

    opponent.join();
}
