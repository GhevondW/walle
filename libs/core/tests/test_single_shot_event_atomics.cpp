#include <gtest/gtest.h>
#include <thread>
#define SINGLE_SHOT_EVENT_USE_ATOMICS
#include <walle/core/single_shot_event.hpp>

namespace walle::core {

TEST(single_shot_event_test, wait_blocks_wntil_set) {
    single_shot_event_t event;
    std::atomic<bool> event_triggered = false;

    std::thread t([&]() {
        event.wait();
        event_triggered = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(event_triggered);

    event.set();
    t.join();
    EXPECT_TRUE(event_triggered);
}

TEST(single_shot_event_test, set_unblocks_wait) {
    single_shot_event_t event;
    event.set();

    std::atomic<bool> event_triggered = false;

    std::thread t([&]() {
        event.wait();
        event_triggered = true;
    });

    t.join();
    EXPECT_TRUE(event_triggered);
}

TEST(single_shot_event_test, multiple_waits_unblocked_after_set) {
    single_shot_event_t event;
    std::atomic<int> count = 0;

    std::thread t1([&]() {
        event.wait();
        count.fetch_add(1, std::memory_order_relaxed);
    });
    std::thread t2([&]() {
        event.wait();
        count.fetch_add(1, std::memory_order_relaxed);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(count.load(), 0);

    event.set();
    t1.join();
    t2.join();
    EXPECT_EQ(count.load(), 2);
}

TEST(single_shot_event_test, initial_state_true_does_not_block) {
    single_shot_event_t event(true);
    std::atomic<bool> event_triggered = false;

    std::thread t([&]() {
        event.wait();
        event_triggered = true;
    });

    t.join();
    EXPECT_TRUE(event_triggered);
}

TEST(single_shot_event_test, initial_state_false_blocks_until_set) {
    single_shot_event_t event(false);
    std::atomic<bool> event_triggered = false;

    std::thread t([&]() {
        event.wait();
        event_triggered = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(event_triggered);

    event.set();
    t.join();
    EXPECT_TRUE(event_triggered);
}

} // namespace walle::core
