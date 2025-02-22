#include <atomic>
#include <cstddef>
#include <gtest/gtest.h>
#include <walle/asymtx/mutex.hpp>
#include <walle/asymtx/sync_spawn.hpp>
#include <walle/asymtx/sync_task.hpp>
#include <walle/asymtx/task.hpp>
#include <walle/core/single_shot_event.hpp>
#include <walle/exec/event_loop.hpp>

#include <iostream>
#include <walle/exec/thread_pool.hpp>

namespace {

std::atomic<std::size_t> counter = 0;

walle::asymtx::task_t<> foo() {
    std::cout << "Hello foo" << std::endl;
    ++counter;
    co_return;
}

walle::asymtx::task_t<> bar() {
    std::cout << "Hello bar" << std::endl;
    ++counter;
    co_return;
}

walle::asymtx::sync_task_t<> sync_task_just_works_with_void() {
    co_await foo();
    co_await bar();
    co_return;
}

walle::asymtx::task_t<int> foo_a() {
    co_await foo();
    co_await bar();

    co_return 10;
}

walle::asymtx::task_t<int> bar_b() {
    co_await foo();
    co_await bar();

    co_return 5;
}

walle::asymtx::sync_task_t<int> just_works_calculate_value() {
    int a = co_await foo_a();
    int b = co_await bar_b();
    co_return a + b;
}

} // namespace

TEST(asymtx_sync_task, just_works_with_void) {
    walle::core::atomic_single_shot_event_t event;

    counter = 0;

    auto sync_task = sync_task_just_works_with_void();

    sync_task.start(&event);
    event.wait();

    EXPECT_EQ(counter, 2);
}

TEST(asymtx_sync_task, just_works_calculate_value) {
    walle::core::atomic_single_shot_event_t event;

    auto sync_task = just_works_calculate_value();

    sync_task.start(&event);
    event.wait();

    EXPECT_EQ(std::move(sync_task).detach(), 15);
}

TEST(asymtx_sync_task, just_works_sync_spawn) {
    counter = 0;
    auto handle = walle::asymtx::sync_spawn([]() -> walle::asymtx::task_t<> {
        co_await foo();
        co_await bar();
        co_return;
    }());
    handle.blocking_join();
    EXPECT_EQ(counter, 2);
}

TEST(asymtx_sync_task, just_works_sync_spawn_with_result) {
    auto handle = walle::asymtx::sync_spawn([]() -> walle::asymtx::task_t<int> {
        int a = co_await foo_a();
        int b = co_await bar_b();
        co_return a + b;
    }());

    EXPECT_EQ(std::move(handle).get(), 15);
}
