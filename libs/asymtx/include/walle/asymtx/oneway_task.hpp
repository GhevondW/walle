#pragma once

#include <coroutine>
#include <exception>

namespace walle::asymtx {

struct oneway_task {
    struct promise_type {
        std::suspend_never initial_suspend() {
            return {};
        }
        std::suspend_never final_suspend() noexcept {
            return {};
        }
        void unhandled_exception() {
            std::terminate();
        }
        oneway_task get_return_object() {
            return {};
        }
        void return_void() {}
    };
};

} // namespace walle::asymtx
