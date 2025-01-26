#pragma once

#include <atomic>
#include <cstddef>

#include "blocking_parking_lot.hpp"
#include <walle/async/task_function.hpp>
#include <walle/async/task_status.hpp>
#include <walle/exec/executor.hpp>

#include <boost/intrusive_ptr.hpp>

namespace walle::async {

class task_context final {
private:
    friend void intrusive_ptr_add_ref(task_context* ctx) noexcept;
    friend void intrusive_ptr_release(task_context* ctx) noexcept;

    task_context() = default;
    task_context(task_function_t func, exec::executor_i* executor);

public:
    static boost::intrusive_ptr<task_context> make_context(task_function_t func, exec::executor_i* executor);
    ~task_context() noexcept = default;

    void join();
    void blocking_join();
    void run();

private:
    task_function_t _task_function {nullptr};
    std::atomic<std::size_t> _use_count {0};
    std::atomic<task_status_e> _status {task_status_e::k_invalid};
    [[maybe_unused]] exec::executor_i* _executor {nullptr};

    // This is temporary solution
    blocking_parking_lot_t _parking;
};

void intrusive_ptr_add_ref(task_context* ctx) noexcept;
void intrusive_ptr_release(task_context* ctx) noexcept;

} // namespace walle::async
