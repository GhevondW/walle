#pragma once

#include <cstddef>
#include <memory>

#include <function2/function2.hpp>

#include <walle/core/error.hpp>

namespace walle::coro {

class coroutine {
public:
    struct impl;

    struct stack_allocation_error_t : core::error {
    public:
        using error::error;
    };

    // FIx this void* design
    struct suspend_context_t {
    public:
        explicit suspend_context_t(void* cnt)
            : _cnt(cnt) {}
        void suspend();

    private:
        void* _cnt;
    };
    using flow_t = fu2::unique_function<void(suspend_context_t)>;

    [[nodiscard]] static coroutine create(flow_t flow, std::size_t stack_size);

    ~coroutine() noexcept;
    coroutine(const coroutine& other) = delete;
    coroutine(coroutine&& other) noexcept;

    coroutine& operator=(const coroutine& other) = delete;
    coroutine& operator=(coroutine&& other) noexcept = delete;

    void resume();

private:
    explicit coroutine(std::unique_ptr<impl> impl) noexcept;

    std::unique_ptr<impl> _impl;
};

} // namespace walle::coro
