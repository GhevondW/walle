#include <gtest/gtest.h>
#include <thread>
#include <walle/core/semaphore.hpp>

using namespace std::chrono_literals;

TEST(core_test_semaphore, non_blocking) {
    walle::core::semaphore semaphore {2};

    semaphore.acquire(); // -1
    semaphore.release(); // +1

    semaphore.acquire(); // -1
    semaphore.acquire(); // -1
    semaphore.release(); // +1
    semaphore.release(); // +1
}

TEST(core_test_semaphore, blocking) {
    walle::core::semaphore semaphore {0};

    bool touched = false;

    std::thread touch([&] {
        semaphore.acquire();
        touched = true;
    });

    std::this_thread::sleep_for(250ms);

    EXPECT_FALSE(touched);

    semaphore.release();
    touch.join();

    EXPECT_TRUE(touched);
}

TEST(core_test_semaphore, ping_pong) {
    walle::core::semaphore my {1};
    walle::core::semaphore that {0};

    int step = 0;

    std::thread opponent([&] {
        that.acquire();
        EXPECT_EQ(step, 1);
        step = 0;
        my.release();
    });

    my.acquire();
    EXPECT_EQ(step, 0);
    step = 1;
    that.release();

    my.acquire();
    EXPECT_EQ(step, 0);

    opponent.join();
}
