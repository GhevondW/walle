#pragma once

#include <function2/function2.hpp>
#include <memory>

#include <walle/core/coroutine_stack_allocator.hpp>
#include <walle/core/error.hpp>

namespace walle::core::coroutine {

class coroutine_handle {
    struct impl;

    explicit coroutine_handle(std::unique_ptr<impl> impl) noexcept;

public:
    struct resume_on_completed_coroutine_error_t : core::error {
        using error::error;
    };

    struct suspend_context_t {};

    using flow_t = fu2::unique_function<void()>;

    static coroutine_handle create(flow_t flow, coroutine_stack_allocator&& alloc);

    ~coroutine_handle() noexcept;
    coroutine_handle(const coroutine_handle&) = delete;
    coroutine_handle(coroutine_handle&&) noexcept;

    coroutine_handle& operator=(const coroutine_handle&) = delete;
    coroutine_handle& operator=(coroutine_handle&&) noexcept = delete;

    void resume();
    [[nodiscard]] bool is_done() const noexcept;

private:
    std::unique_ptr<impl> _impl;
};

} // namespace walle::core::coroutine
