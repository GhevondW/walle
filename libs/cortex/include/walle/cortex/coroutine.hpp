#pragma once

#include <function2/function2.hpp>
#include <memory>
#include <walle/cortex/suspend_context.hpp>

namespace walle::cortex {

class coroutine_t {
    struct impl;

    explicit coroutine_t(std::shared_ptr<impl> in_impl) noexcept;

public:
    using flow_t = fu2::unique_function<void(suspend_context_i&)>;

    static coroutine_t create(flow_t in_flow);
    static coroutine_t create();

    ~coroutine_t();
    coroutine_t(const coroutine_t&) = delete;
    coroutine_t(coroutine_t&&) noexcept;

    coroutine_t& operator=(const coroutine_t&) = delete;
    coroutine_t& operator=(coroutine_t&&) noexcept;

    explicit operator bool() const noexcept;
    bool is_valid() const noexcept;

    // if the coroutine is invalid the all this functions will caose a UB
    // might throw resume_on_completed_coroutine_error_t
    void resume();

    [[nodiscard]] bool is_done() const noexcept;

private:
    // TODO : maybe do not use shared_ptr? maybe use fast pimpl?
    std::shared_ptr<impl> _impl;
};

} // namespace walle::cortex
