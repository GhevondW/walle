#pragma once

#include <stdexcept>

#include <walle/core/executor.hpp>

namespace walle::core::utils {

struct current_executor {
    struct no_execution_context_error : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    struct scope_guard {
        explicit scope_guard(executor_i* current) noexcept;
        scope_guard(const scope_guard&) = delete;
        scope_guard(scope_guard&&) noexcept = delete;
        scope_guard& operator=(const scope_guard&) = delete;
        scope_guard& operator=(scope_guard&&) noexcept = delete;
        ~scope_guard();

    private:
        executor_i* _prev_executor = nullptr;
    };

    static executor_i* get();

private:
    static void set(executor_i* current) noexcept;

    static thread_local executor_i* current_;
};

} // namespace walle::core::utils
