#include "walle/async/this_task.hpp"
#include "task_context.hpp"
#include "walle/async/error/error.hpp"

#include <cassert>
#include <stdexcept>

namespace walle::async {

class task_context;

thread_local task_context* this_task_context = nullptr;

void this_task::yield() {
    if (this_task_context) {
        throw std::logic_error {"yield called from non fiber context"};
    }
    this_task_context->yield();
}

task_id this_task::get_id() {
    throw error::not_implemeted_error_t {"not implemented yet"};
}

void this_task::sleep_for([[maybe_unused]] std::size_t milliseconds) {
    throw error::not_implemeted_error_t {"not implemented yet"};
}

this_task::guard::guard(task_context* ctx) {
    assert(ctx);
    this_task::set(ctx);
}

this_task::guard::~guard() {
    this_task::set(nullptr);
}

void this_task::set(task_context* ctx) noexcept {
    this_task_context = ctx;
}

} // namespace walle::async
