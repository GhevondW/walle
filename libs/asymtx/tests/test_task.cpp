#include "walle/asymtx/scheduler.hpp"
#include <atomic>
#include <cstddef>
#include <gtest/gtest.h>
#include <thread>
#include <walle/asymtx/async_spawn.hpp>
#include <walle/asymtx/mutex.hpp>
#include <walle/asymtx/sync_spawn.hpp>
#include <walle/asymtx/sync_task.hpp>
#include <walle/asymtx/sync_wait.hpp>
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
    walle::asymtx::sync_wait([]() -> walle::asymtx::task_t<> {
        co_await foo();
        co_await bar();
        co_return;
    }());
    EXPECT_EQ(counter, 2);
}

TEST(asymtx_sync_task, just_works_sync_spawn_with_result) {
    auto res = walle::asymtx::sync_wait([]() -> walle::asymtx::task_t<int> {
        int a = co_await foo_a();
        int b = co_await bar_b();
        co_return a + b;
    }());

    EXPECT_EQ(res, 15);
}

// walle::asymtx::mutex_t global_counter_mutex;
// std::size_t global_counter = 0; // Guarded by global_counter_mutex

// TEST(asymtx_sync_task, just_works_async_spawn) {
//     using namespace walle;

//     global_counter = 0;

//     exec::thread_pool pool(1);
//     asymtx::scheduler_t sched(pool);

//     std::cout << "Main thread started : " << std::this_thread::get_id() << std::endl;

//     auto main_task_handle = asymtx::spawn(sched, [](asymtx::scheduler_t& sched) -> walle::asymtx::task_t<> {
//         std::cout << "Main task started : " << std::this_thread::get_id() << std::endl;

//         auto inner = asymtx::async_scope();

//         inner.spawn([&sched]() -> asymtx::task_t<> {
//             co_await sched.schedule();
//             std::cout << "Inner task started : " << std::this_thread::get_id() << std::endl;
//             co_return;
//         }());

//         co_await inner.join();

//         co_return;
//     }(sched));

//     main_task_handle.blocking_join();
//     pool.stop();
// }
