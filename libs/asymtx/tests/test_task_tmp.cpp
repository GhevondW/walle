#include <coroutine>
#include <gtest/gtest.h>

#include <walle/asymtx/all.hpp>
#include <walle/asymtx/sync_task.hpp>

TEST(async_test_fiber, api_just_works) {
    walle::core::atomic_single_shot_event_t event;

    auto task = []() -> walle::asymtx::sync_task {
        std::cout << "Hello World" << std::endl;
        co_return;
    }();

    task.start(&event);
    event.wait();

    // std::cout << "a" << std::endl;
    // task.resume();
    // std::cout << "c" << std::endl;
    // task.resume();
    // std::cout << "e" << std::endl;
    // task.resume();
}
