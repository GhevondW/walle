#include "walle/asymtx/scheduler.hpp"
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>
#include <walle/asymtx/sync_task.hpp>
#include <walle/asymtx/task.hpp>
#include <walle/exec/event_loop.hpp>

#include <iostream>
#include <walle/exec/thread_pool.hpp>

namespace {

std::atomic<std::size_t> counter = 0;

walle::asymtx::task_t foo() {
    std::cout << "Hello foo" << std::endl;
    co_return;
}

walle::asymtx::task_t bar() {
    std::cout << "Hello bar" << std::endl;
    co_return;
}

walle::asymtx::task_t async_foo(walle::asymtx::scheduler_t& sched) {
    std::cout << "Begin foo : " << std::this_thread::get_id() << std::endl;

    co_await sched.schedule();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    counter.fetch_add(1);

    std::cout << "Hello foo : " << std::this_thread::get_id() << std::endl;
    co_return;
}

walle::asymtx::task_t async_bar(walle::asymtx::scheduler_t& sched) {
    std::cout << "Begin bar : " << std::this_thread::get_id() << std::endl;

    co_await sched.schedule();

    counter.fetch_add(1);

    std::cout << "Hello bar : " << std::this_thread::get_id() << std::endl;
    co_return;
}

} // namespace

TEST(asymtx_sync_task, works_with_void) {
    walle::core::atomic_single_shot_event_t event;

    walle::asymtx::sync_wait([]() -> walle::asymtx::task_t {
        co_await foo();
        co_await bar();
        co_return;
    }());
}

TEST(asymtx_sync_task, async_works_with_void) {
    walle::exec::thread_pool pool(4);
    walle::asymtx::scheduler_t scheduler(pool);

    counter.store(0);

    walle::asymtx::sync_wait([&scheduler]() -> walle::asymtx::task_t {
        co_await async_foo(scheduler);
        co_await async_bar(scheduler);
        co_await async_foo(scheduler);
        co_await async_bar(scheduler);
        co_await async_foo(scheduler);
        co_return;
    }());

    EXPECT_EQ(counter.load(), 5);

    pool.stop();
}
