#include "walle/exec/manual_executor.hpp"

namespace walle::exec {

bool manual_executor::submit(task_t task) {
    if (_state == state_e::k_stopped) {
        return false;
    }

    if (task.empty()) {
        throw empty_task {"the input task is empty"};
    }

    _tasks.push(std::move(task));
    return true;
}

void manual_executor::start() {
    _state = state_e::k_running;
}

void manual_executor::stop() {
    _state = state_e::k_stopped;
}

[[nodiscard]] std::size_t manual_executor::tasks_count() const noexcept {
    return _tasks.size();
}

void manual_executor::run_one() {
    if (_tasks.empty()) {
        throw empty_executor {"the manual_executor is empty"};
    }

    // runs task
    _tasks.front()();
    // pop the task
    _tasks.pop();
}

void manual_executor::run_all() {
    while (tasks_count() > 0) {
        run_one();
    }
}

} // namespace walle::exec
