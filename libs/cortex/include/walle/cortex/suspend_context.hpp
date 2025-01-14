#pragma once

namespace walle::cortex {

struct suspend_context_i {
    suspend_context_i() = default;
    virtual ~suspend_context_i() = default;

    // can not be noexcept because of forced unwinding
    virtual void suspend() = 0;
};

} // namespace walle::cortex
