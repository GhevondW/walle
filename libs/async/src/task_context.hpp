#pragma once

#include <atomic>
// #include <cstddef>

#include <walle/cortex/coroutine_base.hpp>

#include <walle/async/go.hpp>
#include <walle/async/task_function.hpp>

#include <function2/function2.hpp>

namespace walle::async {

enum class task_status_e {
    k_invalid = 0,
    k_new,
    k_running,
    k_suspended,
    k_completed
};

class task_context_t final : public cortex::coroutine_base_i {
private:
    friend task_handle_t go(exec::executor_i* executor, task_function_t func);
    friend task_handle_t go(task_function_t func);

    using do_before_t = fu2::unique_function<void()>;
    using do_after_t = fu2::unique_function<void()>;

    task_context_t(task_function_t task_function, do_before_t do_before, do_after_t do_after);
    ~task_context_t() noexcept = default;

    static void destroy(task_context_t* context);

private:
    void flow(cortex::suspend_context_i& self) override;
    void destroy();

public:
    static task_context_t* make(task_function_t task_function, do_before_t do_before, do_after_t do_after);
    task_status_e status() const noexcept;
    void start();

private:
    std::atomic<task_status_e> _task_status = task_status_e::k_invalid;
    task_function_t _task_function;
    do_before_t _do_before;
    do_after_t _do_after;
};

// void intrusive_ptr_add_ref(task_context* ctx) noexcept;
// void intrusive_ptr_release(task_context* ctx) noexcept;

} // namespace walle::async
