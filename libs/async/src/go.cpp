#include "walle/async/go.hpp"

#include "walle/async/fiber_task.hpp"
#include <walle/exec/current_executor.hpp>

#include "fiber.hpp"

namespace walle::async {

void go(exec::executor_i* executor, fiber_task_t task) {
    auto fiber = fiber_t::make_fiber(executor, std::move(task));
    fiber->schedule();
}

void go(fiber_task_t task) {
    go(exec::current_executor::get(), std::move(task));
}

} // namespace walle::async
