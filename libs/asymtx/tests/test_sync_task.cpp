#include <gtest/gtest.h>
#include <stdexcept>
#include <walle/asymtx/sync_task.hpp>
#include <walle/core/atomic_single_shot_event.hpp>

TEST(asymtx_sync_task, works_with_void) {
    walle::core::atomic_single_shot_event_t event;

    auto task = []() -> walle::asymtx::sync_task_t<> { co_return; }();

    task.start(&event);
    event.wait();

    EXPECT_TRUE(task.is_valid());
    EXPECT_TRUE(task.is_done());
    EXPECT_NO_THROW(std::move(task).detach());
    EXPECT_FALSE(task.is_valid());
}

TEST(asymtx_sync_task, works_with_int) {
    walle::core::atomic_single_shot_event_t event;

    auto task = []() -> walle::asymtx::sync_task_t<int> { co_return 42; }();

    task.start(&event);
    event.wait();

    EXPECT_TRUE(task.is_valid());
    EXPECT_TRUE(task.is_done());
    EXPECT_EQ(std::move(task).detach(), 42);
    EXPECT_FALSE(task.is_valid());
}

TEST(asymtx_sync_task, throws_on_detach_if_not_started) {
    auto task = []() -> walle::asymtx::sync_task_t<int> { co_return 42; }();
    EXPECT_THROW(auto a = std::move(task).detach(), std::logic_error);
}

TEST(asymtx_sync_task, throws_on_detach_if_not_done) {
    walle::core::atomic_single_shot_event_t event;

    auto task = []() -> walle::asymtx::sync_task_t<int> { co_return 42; }();

    EXPECT_THROW(auto a = std::move(task).detach(), std::logic_error);
}

TEST(asymtx_sync_task, handles_exceptions) {
    walle::core::atomic_single_shot_event_t event;

    auto task = []() -> walle::asymtx::sync_task_t<int> {
        throw std::runtime_error("Test exception");
        co_return 0;
    }();

    task.start(&event);
    event.wait();

    EXPECT_TRUE(task.is_valid());
    EXPECT_TRUE(task.is_done());
    EXPECT_THROW(auto a = std::move(task).detach(), std::runtime_error);
    EXPECT_FALSE(task.is_valid());
}

TEST(asymtx_sync_task, move_assignment) {
    walle::core::atomic_single_shot_event_t event;

    auto task1 = []() -> walle::asymtx::sync_task_t<int> { co_return 100; }();

    auto task2 = std::move(task1);
    task2.start(&event);
    event.wait();

    EXPECT_TRUE(task2.is_valid());
    EXPECT_TRUE(task2.is_done());
    EXPECT_EQ(std::move(task2).detach(), 100);
    EXPECT_FALSE(task2.is_valid());
}

TEST(asymtx_sync_task, throws_on_start_with_null_event) {
    auto task = []() -> walle::asymtx::sync_task_t<int> { co_return 10; }();

    EXPECT_THROW(task.start(nullptr), std::invalid_argument);
}

TEST(asymtx_sync_task, throws_on_resume_after_done) {
    walle::core::atomic_single_shot_event_t event;

    auto task = []() -> walle::asymtx::sync_task_t<> { co_return; }();

    task.start(&event);
    event.wait();
    EXPECT_TRUE(task.is_done());

    EXPECT_THROW(task.start(&event), std::logic_error);
}
