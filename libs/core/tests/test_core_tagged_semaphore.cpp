#include <gtest/gtest.h>
#include <walle/core/tagged_semaphore.hpp>

using namespace std::chrono_literals;

struct my_tag {};

using tsem = walle::core::tagged_semaphore<my_tag>;

TEST(core_test_tagged_semaphore, non_blocking) {
    tsem semaphore {2};

    auto t1 = semaphore.acquire(); // -1
    semaphore.release(std::move(t1)); // +1

    auto t2 = semaphore.acquire(); // -1
    auto t3 = semaphore.acquire(); // -1
    semaphore.release(std::move(t3)); // +1
    semaphore.release(std::move(t2)); // +1
}

// TODO: add additional tests
