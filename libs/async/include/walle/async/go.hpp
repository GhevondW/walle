#pragma once

#include <walle/async/fiber_task.hpp>
#include <walle/exec/executor.hpp>

namespace walle::async {

void go(exec::executor_i& executor, fiber_task_t task);
void go(fiber_task_t task);

} // namespace walle::async
