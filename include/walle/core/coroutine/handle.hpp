#pragma once

#include <function2/function2.hpp>
#include <memory>

#include <walle/core/coroutine/stack_allocator.hpp>
#include <walle/core/error.hpp>

namespace walle::core::coroutine {

class handle {
    struct impl;

    explicit handle(std::unique_ptr<impl> impl) noexcept;

public:
    struct resume_on_completed_coroutine_error_t : core::error {
        using error::error;
    };

    struct suspend_context_t {};

    using flow_t = fu2::unique_function<void()>;

    static handle create(flow_t flow, stack_allocator&& alloc);

    ~handle() noexcept;
    handle(const handle&) = delete;
    handle(handle&&) noexcept;

    handle& operator=(const handle&) = delete;
    // The behavior is undefined if destroying is needed and *this does not refer to a suspended coroutine
    handle& operator=(handle&&) noexcept;

    void resume();
    [[nodiscard]] bool is_done() const noexcept;

private:
    std::unique_ptr<impl> _impl;
};

} // namespace walle::core::coroutine
