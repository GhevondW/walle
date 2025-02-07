#include <coroutine>
#include <gtest/gtest.h>

#include <walle/asymtx/all.hpp>
#include <walle/asymtx/sync_task.hpp>

TEST(async_test_fiber, api_just_works) {
    auto task = []() -> walle::asymtx::sync_task {
        std::cout << "b" << std::endl;
        co_await std::suspend_always {};
        std::cout << "d" << std::endl;
        co_await std::suspend_always {};
        std::cout << "f" << std::endl;
        co_return;
    }();

    std::cout << "a" << std::endl;
    task.resume();
    std::cout << "c" << std::endl;
    task.resume();
    std::cout << "e" << std::endl;
    task.resume();
}
