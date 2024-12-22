#include <chrono>
#include <gtest/gtest.h>
#include <thread>
#include <walle/sync/semaphore.hpp>

using namespace std::chrono_literals;

TEST(SemaphoreTest, NonBlocking) {
    walle::sync::semaphore semaphore {2};

    semaphore.acquire(); // -1
    semaphore.release(); // +1

    semaphore.acquire(); // -1
    semaphore.acquire(); // -1
    semaphore.release(); // +1
    semaphore.release(); // +1
}

TEST(SemaphoreTest, Blocking) {
    walle::sync::semaphore semaphore {0};

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

TEST(SemaphoreTest, PingPong) {
    walle::sync::semaphore my {1};
    walle::sync::semaphore that {0};

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
