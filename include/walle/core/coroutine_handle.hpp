#pragma once

#include <function2/function2.hpp>
#include <memory>

#include <stdexcept>
#include <walle/core/coroutine_stack_allocator.hpp>
#include <walle/core/error.hpp>

namespace walle::core {

// TODO : construct the impl with inplace new on the same stack memory.
class coroutine_handle {
    struct impl;

    explicit coroutine_handle(std::unique_ptr<impl> impl) noexcept;

public:
    struct resume_on_completed_coroutine_error_t : std::logic_error {
        using logic_error::logic_error;
    };

    struct suspend_context {
        suspend_context(void* machine_context, impl* in_impl)
            : _machine_context(machine_context)
            , _impl(in_impl) {}

        // can not be noexcept because of forced unwinding
        void suspend();

    private:
        void* _machine_context = nullptr;
        impl* _impl = nullptr;
    };

    using flow_t = fu2::unique_function<void(suspend_context)>;

    static coroutine_handle create(flow_t flow, coroutine_stack_allocator&& alloc = coroutine_stack_allocator {});

    ~coroutine_handle();
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
