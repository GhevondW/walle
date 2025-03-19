#pragma once

// #include <atomic>
// #include <cstddef>

#include <walle/cortex/coroutine_base.hpp>

#include <walle/async/task_function.hpp>

#include <function2/function2.hpp>

namespace walle::async {

class task_context final : public cortex::coroutine_base_i {
private:
    using do_before_t = fu2::unique_function<void()>;
    using do_after_t = fu2::unique_function<void()>;

    task_context(task_function_t task_function, do_before_t do_before, do_after_t do_after);
    ~task_context() noexcept = default;

    static void destroy(task_context* context);

private:
    void flow(cortex::suspend_context_i& self) override;
    void destroy();

public:
    static task_context* make(task_function_t task_function, do_before_t do_before, do_after_t do_after);

    void start();

private:
    task_function_t _task_function;
    do_before_t _do_before;
    do_after_t _do_after;
};

// void intrusive_ptr_add_ref(task_context* ctx) noexcept;
// void intrusive_ptr_release(task_context* ctx) noexcept;

} // namespace walle::async
