#include "walle/asymtx/scheduler.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>
#include <walle/asymtx/sync_task.hpp>
#include <walle/asymtx/task.hpp>
#include <walle/exec/event_loop.hpp>

#include <iostream>
#include <walle/exec/thread_pool.hpp>

namespace {

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

    std::cout << "Hello foo : " << std::this_thread::get_id() << std::endl;
    co_return;
}

walle::asymtx::task_t async_bar(walle::asymtx::scheduler_t& sched) {
    std::cout << "Begin bar : " << std::this_thread::get_id() << std::endl;

    co_await sched.schedule();

    std::cout << "Hello bar : " << std::this_thread::get_id() << std::endl;
    co_return;
}

walle::asymtx::sync_task_t<> sync(walle::asymtx::scheduler_t& sched) {
    co_await async_foo(sched);
    co_await async_bar(sched);
    co_await async_foo(sched);
    co_await async_bar(sched);
    co_await async_foo(sched);
    co_return;
};

} // namespace

TEST(asymtx_sync_task, works_with_void) {
    walle::core::atomic_single_shot_event_t event;

    auto task = []() -> walle::asymtx::sync_task_t<> {
        co_await foo();
        co_await bar();
        co_return;
    }();

    task.start(&event);
    event.wait();

    EXPECT_TRUE(task.is_valid());
    EXPECT_TRUE(task.is_done());
    EXPECT_NO_THROW(std::move(task).detach());
    EXPECT_FALSE(task.is_valid());
}

TEST(asymtx_sync_task, async_works_with_void) {
    walle::exec::thread_pool pool(4);
    walle::asymtx::scheduler_t scheduler(pool);

    walle::core::atomic_single_shot_event_t event;

    auto task = sync(scheduler);

    task.start(&event);
    event.wait();

    EXPECT_TRUE(task.is_valid());
    EXPECT_TRUE(task.is_done());
    EXPECT_NO_THROW(std::move(task).detach());
    EXPECT_FALSE(task.is_valid());

    pool.stop();
}
