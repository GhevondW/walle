#include "walle/async/go.hpp"
#include "detail/fiber.hpp"

#include "walle/async/error/error.hpp"

#include <walle/exec/current_executor.hpp>

namespace walle::async {

void go(exec::executor_i& executor, fiber_task_t task) {
    auto* fiber = new detail::fiber_t(executor, std::move(task));
    fiber->schedule();
}

void go(fiber_task_t task) {
    auto current_executo = exec::current_executor::unsafe_get();
    if (current_executo == nullptr) {
        throw error::fiber_creation_in_invalid_execution_context {"fiber creation attempt in no execution context."};
    }

    go(*current_executo, std::move(task));
}

} // namespace walle::async
