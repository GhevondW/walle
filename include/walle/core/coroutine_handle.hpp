#pragma once

#include <function2/function2.hpp>
#include <memory>

#include <walle/core/coroutine_stack_allocator.hpp>
#include <walle/core/error.hpp>

namespace walle::core {

class coroutine_handle {
    struct impl;

    explicit coroutine_handle(std::unique_ptr<impl> impl) noexcept;

public:
    struct resume_on_completed_coroutine_error_t : core::error {
        using error::error;
    };

    struct suspend_context_t {
        suspend_context_t() noexcept = default;
        virtual ~suspend_context_t() noexcept = default;
        suspend_context_t(const suspend_context_t&) = delete;
        suspend_context_t(suspend_context_t&&) noexcept = delete;
        suspend_context_t& operator=(const suspend_context_t&) = delete;
        suspend_context_t& operator=(suspend_context_t&&) noexcept = delete;

        virtual void suspend() = 0; // maybe noexcept and const
    };

    using flow_t = fu2::unique_function<void(suspend_context_t&)>;

    static coroutine_handle create(flow_t flow, coroutine_stack_allocator&& alloc = coroutine_stack_allocator {});

    ~coroutine_handle() noexcept;
    coroutine_handle(const coroutine_handle&) = delete;
    coroutine_handle(coroutine_handle&&) noexcept;

    coroutine_handle& operator=(const coroutine_handle&) = delete;
    coroutine_handle& operator=(coroutine_handle&&) noexcept = delete;

    void resume();
    [[nodiscard]] bool is_done() const noexcept;

private:
    // TODO : maybe do not use unique_ptr?
    std::unique_ptr<impl> _impl;
};

} // namespace walle::core
