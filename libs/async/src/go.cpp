#include "walle/async/go.hpp"

#include "task_context.hpp"
#include <walle/exec/current_executor.hpp>

namespace walle::async {

task_handle go(exec::executor_i& executor, task_function_t func) {
    return task_handle(task_context::make_context(std::move(func), &executor));
}

task_handle go(task_function_t func) {
    auto executor = exec::current_executor::get();
    return task_handle(task_context::make_context(std::move(func), executor));
}

} // namespace walle::async
