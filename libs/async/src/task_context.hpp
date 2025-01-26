#pragma once

#include <atomic>
#include <cstddef>

#include <blocking_parking_lot.hpp>
#include <walle/async/task_function.hpp>
#include <walle/async/task_status.hpp>
#include <walle/cortex/coroutine.hpp>
#include <walle/cortex/suspend_context.hpp>
#include <walle/exec/executor.hpp>

#include <boost/intrusive_ptr.hpp>

// This is an experimental implementation for learning purposes. I will improve it later.
// I am not considering memory order for now; I will address it later.

namespace walle::async {

class task_context final {
private:
    friend void intrusive_ptr_add_ref(task_context* ctx) noexcept;
    friend void intrusive_ptr_release(task_context* ctx) noexcept;

    task_context() = default;
    task_context(task_function_t func, exec::executor_i* executor);

public:
    static boost::intrusive_ptr<task_context> make_context(task_function_t func, exec::executor_i* executor);
    ~task_context() noexcept;

    void schedule();
    void terminate();
    void yield();
    void join();
    void blocking_join();
    void run();

    task_status_e status() const noexcept;
    exec::executor_i* execotor() const noexcept;

    // TODO : make this private
    void set_suspend_context(cortex::suspend_context_i* ctx) noexcept {
        _suspend_context = ctx;
    }

private:
    // coroutine
    cortex::coroutine_t _coro {cortex::coroutine_t::create()};
    cortex::suspend_context_i* _suspend_context {nullptr};

    // state
    std::atomic<std::size_t> _use_count {0};
    std::atomic<task_status_e> _status {task_status_e::k_invalid};

    // execution
    [[maybe_unused]] exec::executor_i* _executor {nullptr};

    // synchronization
    // This is temporary solution
    blocking_parking_lot_t _parking {};
};

using task_context_weak_ptr = task_context*;

void intrusive_ptr_add_ref(task_context* ctx) noexcept;
void intrusive_ptr_release(task_context* ctx) noexcept;

} // namespace walle::async
