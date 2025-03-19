#include "task_context.hpp"

namespace walle::async {

task_context::task_context(task_function_t task_function, do_before_t do_before, do_after_t do_after)
    : _task_function(std::move(task_function))
    , _do_before(std::move(do_before))
    , _do_after(std::move(do_after)) {}

void task_context::destroy(task_context* context) {
    delete context;
}

void task_context::flow([[maybe_unused]] cortex::suspend_context_i& self) {
    _do_before();

    // maybe i need to handle exceptions here?
    _task_function();

    // maybe this must be noexcept
    _do_after();
    destroy();
}

void task_context::destroy() {
    destroy(this);
}

task_context* task_context::make(task_function_t task_function, do_before_t do_before, do_after_t do_after) {
    return new task_context {std::move(task_function), std::move(do_before), std::move(do_after)};
}

void task_context::start() {
    resume();
}

} // namespace walle::async
