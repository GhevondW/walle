#include "walle/exec/manual_executor.hpp"
#include "walle/exec/current_executor.hpp"

namespace walle::exec {

void manual_executor::submit(task_t task) {
    if (task.empty()) {
        throw empty_task {"the input task is empty"};
    }

    _tasks.push(std::move(task));
}

[[nodiscard]] std::size_t manual_executor::tasks_count() const noexcept {
    return _tasks.size();
}

void manual_executor::run_one() {
    const current_executor::scope_guard scope(this);

    if (_tasks.empty()) {
        throw empty_executor {"the manual_executor is empty"};
    }

    // runs task
    _tasks.front()();
    // pop the task
    _tasks.pop();
}

void manual_executor::run_all() {
    const current_executor::scope_guard scope(this);

    while (tasks_count() > 0) {
        run_one();
    }
}

} // namespace walle::exec
