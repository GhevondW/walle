#include <gtest/gtest.h>

#include <walle/asymtx/all.hpp>
#include <walle/asymtx/sync_task.hpp>

TEST(asymtx_sync_task, just_works_void) {
    walle::core::atomic_single_shot_event_t event;

    auto task = []() -> walle::asymtx::sync_task<> {
        std::cout << "Hello from sync_task<>" << std::endl;
        co_return;
    }();

    task.start(&event);
    event.wait();

    EXPECT_TRUE(task.is_valid());
    EXPECT_TRUE(task.is_done());
    EXPECT_NO_THROW(std::move(task).detach());
    EXPECT_FALSE(task.is_valid());
}

TEST(asymtx_sync_task, just_works_int) {
    walle::core::atomic_single_shot_event_t event;

    auto compute_task = []() -> walle::asymtx::sync_task<int> {
        std::cout << "Hello from sync_task<int>" << std::endl;
        co_return 12;
    }();

    compute_task.start(&event);
    event.wait();

    EXPECT_TRUE(compute_task.is_valid());
    EXPECT_TRUE(compute_task.is_done());
    int val = std::move(compute_task).detach();
    EXPECT_FALSE(compute_task.is_valid());
    EXPECT_EQ(val, 12);
}
