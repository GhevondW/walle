#pragma once

#include <cassert>
#include <coroutine>

#include <walle/asymtx/task.hpp>
#include <walle/exec/executor.hpp>

namespace walle::asymtx {

class scheduler_t {
public:
    explicit scheduler_t(exec::executor_i& executor)
        : _executor(executor) {}

    scheduler_t(const scheduler_t&) = delete;
    scheduler_t(scheduler_t&&) = delete;

    scheduler_t& operator=(const scheduler_t&) = delete;
    scheduler_t& operator=(scheduler_t&&) = delete;

    void* operator new(std::size_t) = delete;
    void* operator new[](std::size_t) = delete;

    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;

    class schedule_awaitable {
    public:
        explicit schedule_awaitable(exec::executor_i& executor) noexcept
            : _executor(executor) {}

        bool await_ready() noexcept {
            return false;
        }

        void await_suspend(std::coroutine_handle<> awaiting_coroutine) {
            _executor.submit([coroutine = awaiting_coroutine]() mutable { coroutine.resume(); });
        }

        void await_resume() noexcept {}

    private:
        exec::executor_i& _executor;
    };

    auto schedule() {
        return schedule_awaitable {_executor};
    }

private:
    exec::executor_i& _executor;
};

} // namespace walle::asymtx
