#include <gtest/gtest.h>
#include <walle/core/coroutine_handle.hpp>

#include <iostream>

TEST(CoroutineHandle, JustWorks) {
    auto handle = walle::core::coroutine_handle::create([](auto& ctx) {
        std::cout << "coro -> #1" << std::endl;
        ctx.suspend();
        std::cout << "coro -> #3" << std::endl;
        ctx.suspend();
        std::cout << "coro -> #5" << std::endl;
    });

    handle.resume();
    std::cout << "main -> #2" << std::endl;
    handle.resume();
    std::cout << "main -> #4" << std::endl;
    handle.resume();
    std::cout << "main -> #6" << std::endl;
}
