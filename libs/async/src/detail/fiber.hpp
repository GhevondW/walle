#pragma once

#include "walle/async/error/error.hpp"
#include "walle/async/fiber_task.hpp"

#include <walle/cortex/coroutine.hpp>
#include <walle/exec/executor.hpp>

namespace walle::async::detail {

class fiber_t {
public:
    fiber_t(exec::executor_i& executor, fiber_task_t task) noexcept
        : _executor(executor)
        , _coroutine(create_coroutine(std::move(task))) {}

    void schedule() {}

private:
    static cortex::coroutine create_coroutine([[maybe_unused]] fiber_task_t task) {
        throw error::not_implemeted_error_t {"not implemented"};
    }

private:
    [[maybe_unused]] exec::executor_i& _executor;
    cortex::coroutine _coroutine;
};

} // namespace walle::async::detail
