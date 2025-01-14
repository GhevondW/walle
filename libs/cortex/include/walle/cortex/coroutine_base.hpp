#pragma once

#include <walle/cortex/coroutine.hpp>

namespace walle::cortex {

struct coroutine_base_i {
public:
    // TODO : add allocator support
    coroutine_base_i();
    virtual ~coroutine_base_i() = default;

    coroutine_base_i(const coroutine_base_i&) = delete;
    coroutine_base_i(coroutine_base_i&&) noexcept;

    coroutine_base_i& operator=(const coroutine_base_i&) = delete;
    coroutine_base_i& operator=(coroutine_base_i&&) noexcept;

    explicit operator bool() const noexcept;
    bool is_valid() const noexcept;

    // if the coroutine is invalid the all this functions will caose a UB
    // might throw resume_on_completed_coroutine_error_t
    void resume();

    [[nodiscard]] bool is_done() const noexcept;

private:
    virtual void flow(suspend_context_i& self) = 0;
    void run_flow(suspend_context_i& self);

private:
    coroutine_t _coroutine;
};

} // namespace walle::cortex
