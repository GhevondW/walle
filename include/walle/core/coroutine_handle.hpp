#pragma once

#include <function2/function2.hpp>
#include <memory>

#include <walle/core/error.hpp>

namespace walle::core {

class coroutine_handle {
    struct impl;

    explicit coroutine_handle(std::unique_ptr<impl> impl) noexcept;

public:
    struct resume_on_completed_coroutine_error : core::error {
        using error::error;
    };

    using flow_t = fu2::unique_function<void()>;

    static coroutine_handle create(flow_t flow);

    ~coroutine_handle() noexcept;
    coroutine_handle(const coroutine_handle&) = delete;
    coroutine_handle(coroutine_handle&&) noexcept;

    coroutine_handle& operator=(const coroutine_handle&) = delete;
    // The behavior is undefined if destroying is needed and *this does not refer to a suspended coroutine
    coroutine_handle& operator=(coroutine_handle&&) noexcept;

    void resume();
    [[nodiscard]] bool is_done() const noexcept;

private:
    std::unique_ptr<impl> _impl;
};

} // namespace walle::core
