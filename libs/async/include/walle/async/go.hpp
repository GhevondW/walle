#pragma once

#include <walle/async/task_function.hpp>
#include <walle/async/task_handle.hpp>
#include <walle/exec/executor.hpp>

namespace walle::async {

task_handle go(exec::executor_i* executor, task_function_t func);
task_handle go(task_function_t func);

} // namespace walle::async
