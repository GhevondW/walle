#include "task_context.hpp"
#include "walle/async/task_status.hpp"
#include "walle/async/this_task.hpp"

#include <cassert>

namespace walle::async {

namespace {

cortex::coroutine_t make_task_context_coroutine(task_context_weak_ptr context, task_function_t task_function) {
    return cortex::coroutine_t::create([context, func = std::move(task_function)]([[maybe_unused]] auto& self) mutable {
        context->set_suspend_context(&self);
        func();
    });
}

} // namespace

task_context::task_context(task_function_t func, exec::executor_i* executor)
    : _coro(cortex::coroutine_t::create())
    , _use_count(0)
    , _status(task_status_e::k_new)
    , _executor(executor)
    , _parking() {
    _coro = make_task_context_coroutine(this, std::move(func));
}

boost::intrusive_ptr<task_context> task_context::make_context(task_function_t func, exec::executor_i* executor) {
    assert(executor);
    return boost::intrusive_ptr<task_context> {new task_context(std::move(func), executor)};
}

task_context::~task_context() noexcept {
    // _parking.wake_and_close();
}

void task_context::schedule() {
    assert(_executor);
    task_context_weak_ptr wptr = this;
    _executor->submit([wptr]() { wptr->run(); });
}

void task_context::terminate() {
    _status.store(task_status_e::k_completed);
    _parking.unpark_all();
}

void task_context::yield() {
    _suspend_context->suspend();
    schedule();
}

void task_context::join() {
    // This does blocking_join for now :) will fix
    blocking_join();
}

void task_context::blocking_join() {
    // throw error::not_implemeted_error_t{"not implemented yet"};
    if (_status.load() == task_status_e::k_completed) {
        return;
    }
    _parking.park();
}

void task_context::run() {
    // maybe i do not need this
    assert(_status.load() != task_status_e::k_running);

    {
        const this_task::guard guard(this);
        assert(_coro.is_valid());

        if (_status.load() == task_status_e::k_completed) {
            return;
        }

        _status.store(task_status_e::k_running);
        _coro.resume();

        if (!_coro.is_done()) {
            _status.store(task_status_e::k_suspended);
            return;
        }
    }

    terminate();
}

task_status_e task_context::status() const noexcept {
    return _status.load();
}

exec::executor_i* task_context::execotor() const noexcept {
    return _executor;
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
