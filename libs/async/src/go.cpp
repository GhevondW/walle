#include "walle/async/go.hpp"
#include "task_context.hpp"
#include "walle/async/error/error.hpp"

#include <memory>
#include <walle/core/single_shot_event.hpp>
#include <walle/exec/current_executor.hpp>

namespace walle::async {

task_handle_t go(exec::executor_i* executor, task_function_t func) {
    // this shared if for experiment
    auto event = std::make_shared<core::atomic_single_shot_event_t>();
    task_context_t* context =
        task_context_t::make(std::move(func), []() {}, [event_ptr = event.get()]() { event_ptr->set(); });
    executor->submit([context]() { context->start(); });
    return task_handle_t(std::move(event), context);
}

task_handle_t go(task_function_t func) {
    auto executor = exec::current_executor::get();
    return go(executor, std::move(func));
}

} // namespace walle::async
