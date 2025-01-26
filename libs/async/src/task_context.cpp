#include "task_context.hpp"

#include <cassert>

namespace walle::async {

task_context::task_context(task_function_t func, exec::executor_i* executor)
    : _task_function(std::move(func))
    , _use_count(0)
    , _status(task_status_e::k_new)
    , _executor(executor) {}

boost::intrusive_ptr<task_context> task_context::make_context(task_function_t func, exec::executor_i* executor) {
    assert(executor);
    return boost::intrusive_ptr<task_context> {new task_context(std::move(func), executor)};
}

void task_context::join() {
    // throw error::not_implemeted_error_t{"not implemented yet"};
}

void task_context::blocking_join() {
    // throw error::not_implemeted_error_t{"not implemented yet"};
}

void task_context::run() {
    _task_function();
}

void intrusive_ptr_add_ref(task_context* context) noexcept {
    assert(context);
    context->_use_count.fetch_add(1);
}

void intrusive_ptr_release(task_context* context) noexcept {
    assert(context);
    if (context->_use_count.fetch_sub(1) == 1) {
        delete context;
    }
}

} // namespace walle::async
