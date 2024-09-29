#include <catch2/catch_test_macros.hpp>

#include <walle/sync/tagged_semaphore.hpp>

using namespace std::chrono_literals;

struct my_tag {};

using tsem = walle::sync::tagged_semaphore<my_tag>;

TEST_CASE("walle::sync::tagged_semaphore non blocking", "[walle::sync::tagged_semaphore]") {
    tsem semaphore {2};

    auto t1 = semaphore.acquire(); // -1
    semaphore.release(std::move(t1)); // +1

    auto t2 = semaphore.acquire(); // -1
    auto t3 = semaphore.acquire(); // -1
    semaphore.release(std::move(t3)); // +1
    semaphore.release(std::move(t2)); // +1
}

// TODO : add tests
