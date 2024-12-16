#pragma once

#include <memory>
#include <stdexcept>

#include <function2/function2.hpp>

namespace cortex {

class coroutine {
    struct coroutine_impl;

    explicit coroutine(std::unique_ptr<coroutine_impl> impl) noexcept;

public:
    struct resume_on_completed_coroutine_error_t : std::logic_error {
        using logic_error::logic_error;
    };

    struct suspend_context {
        suspend_context() noexcept = default;
        suspend_context(const suspend_context&) = delete;
        suspend_context(suspend_context&&) noexcept = delete;
        suspend_context& operator=(const suspend_context&) = delete;
        suspend_context& operator=(suspend_context&&) noexcept = delete;
        virtual ~suspend_context() = default;

        virtual void suspend() = 0;
    };

    using flow_t = fu2::unique_function<void(suspend_context&)>;

    static coroutine create(flow_t flow);

    ~coroutine();
    coroutine(const coroutine&) = delete;
    coroutine(coroutine&&) noexcept = delete;

    coroutine& operator=(const coroutine&) = delete;
    coroutine& operator=(coroutine&&) noexcept = delete;

    void resume();
    [[nodiscard]] bool is_done() const noexcept;

private:
    // TODO : maybe do not use unique_ptr?
    std::unique_ptr<coroutine_impl> _impl;
};

} // namespace cortex
