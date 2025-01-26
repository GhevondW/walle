#pragma once

#include <atomic>
#include <cstddef>
namespace walle::async {

class task_context final {
    friend void intrusive_ptr_add_ref(task_context* ctx) noexcept;
    friend void intrusive_ptr_release(task_context* ctx) noexcept;

public:
    task_context() = default;
    ~task_context() noexcept = default;

    void join() {}
    void blocking_join() {}

    std::atomic<std::size_t> _use_count {0};
};

void intrusive_ptr_add_ref(task_context* ctx) noexcept;
void intrusive_ptr_release(task_context* ctx) noexcept;

} // namespace walle::async
