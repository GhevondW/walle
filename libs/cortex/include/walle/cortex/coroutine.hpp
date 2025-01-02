#pragma once

#include <function2/function2.hpp>
#include <memory>

#include <stdexcept>

namespace walle::cortex {

class coroutine {
    struct impl;

    explicit coroutine(std::shared_ptr<impl> in_impl) noexcept;

public:
    struct resume_on_completed_coroutine_error_t : std::logic_error {
        using logic_error::logic_error;
    };

    struct suspend_context {
        suspend_context() = default;
        virtual ~suspend_context() = default;

        // can not be noexcept because of forced unwinding
        virtual void suspend() = 0;
    };

    using flow_t = fu2::unique_function<void(suspend_context&)>;

    static coroutine create(flow_t in_flow);

    ~coroutine();
    coroutine(const coroutine&) = delete;
    coroutine(coroutine&&) noexcept;

    coroutine& operator=(const coroutine&) = delete;
    coroutine& operator=(coroutine&&) noexcept = delete;

    void resume();
    [[nodiscard]] bool is_done() const noexcept;

private:
    // TODO : maybe do not use shared_ptr? maybe use fast pimpl?
    std::shared_ptr<impl> _impl;
};

} // namespace walle::cortex
