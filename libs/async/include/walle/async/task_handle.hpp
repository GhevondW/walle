#pragma once

#include <walle/core/non_copyable.hpp>

#include <walle/async/task_function.hpp>
#include <walle/async/task_id.hpp>

#include <boost/intrusive_ptr.hpp>

namespace walle::exec {
struct executor_i;
}

namespace walle::async {

class task_context;

// TODO : allocator support
class task_handle final : public core::non_copyable {
private:
    friend task_handle go(exec::executor_i& executor, task_function_t func);
    friend task_handle go(task_function_t func);

    task_handle(boost::intrusive_ptr<task_context> impl);

public:
    task_handle(task_handle&&) noexcept;
    task_handle& operator=(task_handle&&) noexcept;
    ~task_handle();

    bool is_valid() const noexcept;

    bool joinable() const noexcept;

    task_id get_id() const noexcept;

    // void detach();

    // coroutine context
    void join();

    // none coroutine context
    void blocking_join();

    void cancel();

    void blocking_cancel();

private:
    boost::intrusive_ptr<task_context> _impl;
};

} // namespace walle::async
