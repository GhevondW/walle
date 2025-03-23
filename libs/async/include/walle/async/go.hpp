#pragma once

#include <walle/async/task_function.hpp>
#include <walle/async/task_handle.hpp>
#include <walle/exec/executor.hpp>

namespace walle::async {

task_handle_t go(exec::executor_i* executor, task_function_t func);
task_handle_t go(task_function_t func);

} // namespace walle::async
