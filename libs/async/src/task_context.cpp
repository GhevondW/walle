#include "task_context.hpp"

namespace walle::async {

task_context_t::task_context_t(task_function_t task_function, do_before_t do_before, do_after_t do_after)
    : _task_status(task_status_e::k_new)
    , _task_function(std::move(task_function))
    , _do_before(std::move(do_before))
    , _do_after(std::move(do_after)) {}

void task_context_t::destroy(task_context_t* context) {
    delete context;
}

void task_context_t::flow([[maybe_unused]] cortex::suspend_context_i& self) {
    _task_status.store(task_status_e::k_running);

    _task_function();

    _task_status.store(task_status_e::k_completed);
}

void task_context_t::destroy() {
    destroy(this);
}

task_context_t* task_context_t::make(task_function_t task_function, do_before_t do_before, do_after_t do_after) {
    return new task_context_t {std::move(task_function), std::move(do_before), std::move(do_after)};
}

task_status_e task_context_t::status() const noexcept {
    return _task_status.load();
}

void task_context_t::start() {
    if (_task_status.load() == task_status_e::k_new) {
        _do_before();
    }

    resume();

    if (_task_status.load() == task_status_e::k_completed) {
        _do_after();
        destroy(this);
    }
}

} // namespace walle::async
